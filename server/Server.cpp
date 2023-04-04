#include "../include/Server.hpp"

Server::Server(const std::string &port, const std::string &pass) :
	_portNumber(port),
	_password(pass),
	_listener(0),
	_pollfds(1),
	_poll_count(0),
	_mapCmd(),
	_data("")
{
	_mapCmd.insert(std::make_pair(string("CAP"), CAP));
	_mapCmd.insert(std::make_pair(string("PASS"), PASS));
	_mapCmd.insert(std::make_pair(string("NICK"), NICK));
	_mapCmd.insert(std::make_pair(string("USER"), USER));
	_mapCmd.insert(std::make_pair(string("PRIVMSG"), PRIVMSG));
}

Server::~Server()
{
    close(_listener);
}

/*
 *
 * man socket(2), setsockopt(2), bind(2), listen(2)
 *
 * Only waiting for ipv4 connection now
 *
*/

// Mapping between string comands name and enum type ex: "PASS" (string) -> PASS (int)
// Used for switch case
CmdVal	Server::_ResolveOption(const string &input)
{
	if (input.empty())
		return (UNKNOWN);
	std::map<string, CmdVal >::const_iterator it(_mapCmd.find(input));
	if(it != _mapCmd.end())
		return (it->second);
	return (UNKNOWN); 
}

void	Server::_ExecCommand(const Command &cmd, Client &client)
{
	cmd.Debug();
	switch (_ResolveOption(cmd.command))
	{
		case CAP:
		{
			_CapLs(cmd, client);
			break ;
		}
		case PASS:
		{
			_Pass(cmd, client);
			break ;
		}
		case NICK:
		{
			_Nick(cmd, client);
			break ;
		}
		case USER:
		{
			_User(cmd, client);
			break ;
		}
		case PRIVMSG:
		{
			_PrivMsg(cmd, client);
			break ;
		}
		default :
			std::cout << "Unknow command" << std::endl;
	}
}

void Server::SendData(int fd)
{
	std::cout << "Sending data :[" << _data << "]" << std::endl;
	if (send(fd, _data.data(), _data.size(), 0) == -1)
		std::cerr << "⚠️ warning : send err" << std::endl;
	_data.clear();
}

void	Server::AddData(const string &from, const string &message)
{
	_data += ":" + from + " " + message;
}

void	Server::_User(const Command &cmd, Client &client)
{
	vec_str	ui = client.GetUinfo();

	if (ui[password] != _password)
	{
		AddData(SERVER_NAME, ERR_NEEDMOREPARAMS("PASS"));
		SendData(client.GetFd());
		throw irc_error("error: invalid password", CLOSE_CONNECTION);
	}
	if (cmd.middle.size() == 0)
		return AddData(SERVER_NAME, ERR_NEEDMOREPARAMS(cmd.command));
	//TODO: SendData(ERR_ALREADYREGISTERED);
	if (cmd.middle.size() < 3 || cmd.trailing.empty() == true)
	{
		std::cout << "Invalid param" << std::endl;
		return ;
	}
	else
	{
		ui[username] = cmd.middle[0];
		ui[hostname] = cmd.middle[2];
		ui[realname] = cmd.trailing;
		client.SetUinfo(ui);
		if (client.IsRegistd() == false && ui[nickname].empty() == false)
		{
			client.SetRegistd();
			AddData(SERVER_NAME, RPL_WELCOME(ui[nickname], ui[username], ui[hostname]));
		}
	}
}

void	Server::_Nick(const Command &cmd, Client &client)
{
	vec_str			ui = client.GetUinfo();

	if (ui[password] != _password)
	{
		AddData(SERVER_NAME, ERR_NEEDMOREPARAMS("PASS"));
		SendData(client.GetFd());
		throw irc_error("error: invalid password", CLOSE_CONNECTION);
	}
	if (cmd.target.size() != 1)
		return AddData(SERVER_NAME, ERR_NONICKNAMEGIVEN);
	if (_parser.isValidNick(cmd.target[0]) == false)
		return AddData(SERVER_NAME, ERR_ERRONEUSNICKNAME(cmd.target[0]));
	if (_FindNickname(cmd.target[0], &client) != NULL)
	{
		string tmp = "*";
		if (ui[nickname].empty() == false)
			tmp = ui[nickname];
		return AddData(SERVER_NAME, ERR_NICKNAMEINUSE(tmp, cmd.target[0]));
	}

	string from;
	if (ui[nickname].empty() == false) {
		from = ui[nickname];
		if (ui[username].empty() == false)
			from += "!" + ui[username];
		if (ui[hostname].empty() == false)
			from += "@" + ui[hostname];
	}
	else {
		from = SERVER_NAME;
	}
	ui[nickname] = cmd.target[0];
	client.SetUinfo(ui);
	string data;
	AddData(from, "NICK " + ui[nickname] + "\r\n");
	if (client.IsRegistd() == false && ui[username].empty() == false)
	{
		client.SetRegistd();
		AddData(SERVER_NAME, RPL_WELCOME(ui[nickname], ui[username], ui[hostname]));
	}
}

void	Server::_Pass(const Command &cmd, Client &client)
{
	vec_str			ui = client.GetUinfo();

	if (cmd.middle.size() < 1)
		return AddData(SERVER_NAME, ERR_NEEDMOREPARAMS(cmd.middle[0]));
	if (client.IsRegistd())
		return AddData(SERVER_NAME, ERR_ALREADYREGISTERED);
	ui[password] = cmd.params;
	client.SetUinfo(ui);
}

void	Server::_Ping(const Command &cmd, Client &client)
{
	(void)cmd;
	(void)client;
	/*
	(void)cmd;
	std::cout << "ping command received" << std::endl;
	*/
}

Client* Server::_FindNickname(const string &nick, Client *skip) 
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (&it->second != skip && it->second.GetUinfo()[nickname] == nick)
			return (&it->second);
	}
	return (NULL);
}

void	Server::_PrivMsg(const Command &cmd, Client &client)
{
	vec_str			ui = client.GetUinfo();
	Client			*receiver;

	std::cout << "Hello i'm PrivMsg" << std::endl;

	if (cmd.middle.size() == 0)
	{
		std::cout << "NO RECIPIENT moth*r Flower " << std::endl;
		return AddData(SERVER_NAME, ERR_NORECIPIENT(cmd.message));
	}
	else if (cmd.middle.size() > 1)
		return AddData(SERVER_NAME, ERR_TOOMANYTARGETS(cmd.middle[1], cmd.message));
	else if (cmd.trailing.empty())
		return AddData(SERVER_NAME, ERR_NOTEXTTOSEND);
	else if ((receiver = _FindNickname(cmd.target[0])) == NULL)
		return AddData(SERVER_NAME, ERR_NOSUCHNICK(cmd.target[0]));
//	else if (cmd.target.status == away)
//		throw irc_error(RPL_AWAY, SEND_ERROR);
	else
		std::cout << "Message to send: " << cmd.trailing << std::endl;
	const string msg = "PRIVMSG " + cmd.target[0] + " :" + cmd.trailing;
//			:nickname!user@host PRIVMSG target_nickname :Message

	AddData(client.GetPrefix(), msg); 

//		ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
//		ERR_WILDTOPLEVEL          		RPL_AWAY
}

void	Server::_CapLs(const Command &cmd, Client &client)
{
	(void)cmd;
	(void)client;
	/*
	if (cmd.size() != 2 || cmd[1] != "LS")
	{
		std::cout << "CAP LS invalid" << std::endl;
		return ;
	}
	else
	{
		std::cout << "CAP LS ok" << std::endl;
		return ;
	}
	*/
}


void	Server::AwaitingConnectionQueue()
{
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *p;
	int	sockopt = 1;
	int ret_value;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((ret_value = getaddrinfo(NULL, _portNumber.c_str(), &hints, &res)) != 0)
		throw irc_error("getaddrinfo error");

	for (p = res; p != NULL; p = p->ai_next)
	{
		if ((_listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		{
			std::cerr << "⚠️  warning: server socket creation failed" << std::endl;
			continue;
		}
		if (fcntl(_listener, F_SETFL, O_NONBLOCK) == -1)
			throw irc_error("fcntl");
		if (setsockopt(_listener, SOL_SOCKET, SO_REUSEPORT, &sockopt, sizeof(sockopt)) == -1)
			throw irc_error("setsockopt");
		if (bind(_listener, res->ai_addr, res->ai_addrlen) == -1)
		{
			close(_listener);
			std::cerr << "⚠️   warning: server bind failed" << std::endl;
			continue;
		}
		break;
	}
	if (p == NULL)
		throw irc_error("💀 fatal: socket binding failed");
	freeaddrinfo(res);
	if (listen(_listener, MAX_LISTEN) == -1)
		throw irc_error("listen failed");
}

void Server::_AcceptNewConnection()
{
	Client			client(_password, _clients);
	int				new_fd;

	new_fd = client.AcceptClient(_listener);
	if (new_fd == -1)
		std::cerr << "⚠️  warning: accept failed" << std::endl;
	else
	{
		struct pollfd	new_pfd;

		new_pfd.fd = new_fd;
		new_pfd.events = POLLIN;

		_pollfds.push_back(new_pfd);
		_clients.insert(std::make_pair(new_fd, client));
		std::cout << "ℹ️  irc server:\033[0;32m connection etablished\033[0;37m from " << client.GetIp() << " on socket " << client.GetFd() << std::endl;
	}
}

void	Server::_CloseConnection(struct pollfd &pfd)
{
	std::cout << "ℹ️  irc server:\033[0;31m connection close \033[0;37mfrom " << _clients[pfd.fd].GetIp() << " on socket " << pfd.fd << std::endl;
	_clients.erase(pfd.fd);
	close(pfd.fd);
	_pollfds.erase(vec_pfd::iterator(&pfd));
}

void	Server::_ReceiveData(struct pollfd &pfd)
{
	if (pfd.fd == _listener)
		_AcceptNewConnection();
	else
	{
		int ret;
		char buf[512];

		pfd.events = POLLIN;
		memset(&buf, 0, sizeof(buf));
		ret = recv(pfd.fd, buf, sizeof buf, 0); 
		if (ret == 0)
			_CloseConnection(pfd);
		else if (ret < 0)
			std::cerr << "⚠️  warning : recv err" << std::endl;
		else
		{
			Client &client(_clients[pfd.fd]);
			
			try {
				client.ParseRecv(string(buf));
			}
			catch (irc_error &e)
			{
				std::cout << "⚠️  " <<  e.what() << std::endl;
				return ;
			}
			try {
				while (client.GetCmds().empty() == 0)
				{
					_ExecCommand(*client.GetCmds().begin(), client);
					client.PopCmd();
				}
			}
			catch (irc_error &e)
			{
				client.PopCmd();
				if (e.code() == CLOSE_CONNECTION)
					return _CloseConnection(pfd);
				return ;
			}
			SendData(pfd.fd);
				/*
			catch (irc_error &e)
			{
				client.PopCmd();
				if (e.code() == CLOSE_CONNECTION)
					{ AddData(SERVER_NAME, e.what()); _CloseConnection(pfd); }
				else if (e.code() == NO_SEND)
					std::cout << e.what() << std::endl;
				else if (e.code() == SEND_ERROR)
					AddData(SERVER_NAME, e.what());
				else
					std::cout << "⚠️  Unhandle exception catch !!! WARNING : " << e.what() << std::endl;
			}
			*/
		}
	}
}

void Server::InitConnectionLoop()
{
	_pollfds[0].fd = _listener;
	_pollfds[0].events = POLLIN;
	_pollfds[0].revents = 0;
}

void Server::Logs() const
{
	std::cout << "=======================" << std::endl;
	std::cout << "Connection Logs:" << std::endl;
	for (std::map< int, Client >::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::cout << "	clients info:" << std::endl;
		std::cout << "		IP: " << it->second.GetIp() << std::endl;
		std::cout << "		Socket: " << it->second.GetFd() << std::endl;
		if (it->second.GetUinfo()[nickname].empty() == false)
			std::cout << "		Nick: " << it->second.GetUinfo()[nickname] << std::endl;
		if (it->second.GetUinfo()[username].empty() == false)
			std::cout << "		User: " << it->second.GetUinfo()[username] << std::endl;
		if (it->second.GetUinfo()[hostname].empty() == false)
			std::cout << "		Host: " << it->second.GetUinfo()[hostname] << std::endl;
		if (it->second.GetUinfo()[servername].empty() == false)
			std::cout << "		Server: " << it->second.GetUinfo()[servername] << std::endl;
		if (it->second.GetUinfo()[realname].empty() == false)
			std::cout << "		Realname: " << it->second.GetUinfo()[realname] << std::endl;
	}
	std::cout << "=======================" << std::endl;
}

void Server::ConnectionLoop()
{
	std::cout << "port [" << _portNumber << "] password [" << _password << "]" << std::endl;
	while (1)
	{
		if ((_poll_count = poll(_pollfds.data(), _pollfds.size(), -1)) == 10)
			throw irc_error("poll failed");
		//system("clear");
		std::cout << "------------[ IRC ]------------" << std::endl;
		std::cout << "port [" << _portNumber << "] password [" << _password << "]" << std::endl;
		for (size_t i = 0; i < _pollfds.size(); ++i)
		{
			if (_pollfds[i].revents & POLLIN)
				_ReceiveData(_pollfds[i]);
		}
		Logs();
	}
}
