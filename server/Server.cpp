#include "../include/Server.hpp"

Server::Server(const string &port, const string &pass, const string &operPass) :
	_portNumber(port),
	_password(pass),
	_operPass(operPass),
	_listener(0),
	_pollfds(1),
	_poll_count(0),
	_mapCmd(),
	_funcTable(),
	_data("")
{
	_funcTable.push_back(&Server::_CapLs);
	_funcTable.push_back(&Server::_Pass);
	_funcTable.push_back(&Server::_Nick);
	_funcTable.push_back(&Server::_User);
	_funcTable.push_back(&Server::_Pong);
	_funcTable.push_back(&Server::_PrivMsg);
	_funcTable.push_back(&Server::_Mode);
	_funcTable.push_back(&Server::_Notice);
	_funcTable.push_back(&Server::_Oper);
	_funcTable.push_back(&Server::_Join);
	_funcTable.push_back(&Server::_Quit);
	_funcTable.push_back(&Server::_Kill);
	_funcTable.push_back(&Server::_Part);
	_funcTable.push_back(&Server::_Topic);
	_funcTable.push_back(&Server::_Names);
	_funcTable.push_back(&Server::_List);
	_funcTable.push_back(&Server::_Invite);
	_funcTable.push_back(&Server::_Kick);
	_mapCmd.insert(std::make_pair(string("CAP"), CAP));
	_mapCmd.insert(std::make_pair(string("PASS"), PASS));
	_mapCmd.insert(std::make_pair(string("NICK"), NICK));
	_mapCmd.insert(std::make_pair(string("USER"), USER));
	_mapCmd.insert(std::make_pair(string("PING"), PING));
	_mapCmd.insert(std::make_pair(string("PRIVMSG"), PRIVMSG));
	_mapCmd.insert(std::make_pair(string("MODE"), MODE));
	_mapCmd.insert(std::make_pair(string("NOTICE"), NOTICE));
	_mapCmd.insert(std::make_pair(string("OPER"), OPER));
	_mapCmd.insert(std::make_pair(string("JOIN"), JOIN));
	_mapCmd.insert(std::make_pair(string("QUIT"), QUIT));
	_mapCmd.insert(std::make_pair(string("kill"), KILL));
	_mapCmd.insert(std::make_pair(string("PART"), PART));
	_mapCmd.insert(std::make_pair(string("TOPIC"), TOPIC));
	_mapCmd.insert(std::make_pair(string("NAMES"), NAMES));
	_mapCmd.insert(std::make_pair(string("LIST"), LIST));
	_mapCmd.insert(std::make_pair(string("INVITE"), INVITE));
	_mapCmd.insert(std::make_pair(string("KICK"), KICK));
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

				//------------------------------------//
				//	 *** 		BASIC LOOP		***   //
				//------------------------------------//

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
		if (it->second.GetUinfo()[realname].empty() == false)
			std::cout << "		Realname: " << it->second.GetUinfo()[realname] << std::endl;
		if (it->second.GetChannels().empty() == false)
		{
			std::cout << "		Channels: ";
			for (std::list< Channel* >::const_iterator chan = (it->second).GetChannels().begin(); chan != (it->second).GetChannels().end();)
			{
				std::cout << (*(*chan)).GetName();
				if (++chan != it->second.GetChannels().end())
					std::cout << ",";
			}
			std::cout << std::endl;
		}
	}
	std::cout << "=======================" << std::endl;
}

void Server::ConnectionLoop()
{
	std::cout << "port [" << _portNumber << "] password [" << _password << "]" << std::endl;
	while (1)
	{
		if ((_poll_count = poll(_pollfds.data(), _pollfds.size(), -1)) == -1)
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

void	Server::SendChannel(const string &chanstr, const string &message, const string &from, const Client *skip)
{
	lst_chan::iterator chan = _FindChannel(chanstr);
	if (chan == _channels.end())
		throw irc_error(ERR_NOSUCHNICK(chanstr), NO_CHAN);

	cst_lst_pcli &clients = chan->GetUsers();
	for (lst_pcli::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == skip)
			continue ;
		AddData(message, from);
		SendData((*it)->GetFd());
	}
}

void Server::SendData(int fd)
{
	std::cout << "Sending data :[" << _data.data() << "]" << std::endl;
	if (send(fd, _data.data(), _data.size(), 0) == -1)
		std::cerr << "⚠️ warning : send err" << std::endl;
	_data.clear();
}

void	Server::AddData(const string &message, const string &from)
{
	_data += ":" + from + " " + message;
}

				//------------------------------------//
				//	 *** 		PRIVATE 		***   //
				//------------------------------------//

void Server::_AcceptNewConnection()
{
	Client			client;
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
			
			client.ParseRecv(string(buf));
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
			if (_data.empty() == false)
				SendData(pfd.fd);
		}
	}
}

void	Server::_CloseConnection(struct pollfd &pfd)
{
	std::cout << "ℹ️  irc server:\033[0;31m connection close \033[0;37mfrom " << _clients[pfd.fd].GetIp() << " on socket " << pfd.fd << std::endl;
	_clients.erase(pfd.fd);
	close(pfd.fd);
	_pollfds.erase(vec_pfd::iterator(&pfd));
}

void	Server::_CloseConnection(Client &client)
{
	std::cout << "ℹ️  irc server:\033[0;31m connection close \033[0;37mfor "<< client.GetUinfo()[nickname] << " on socket " << client.GetFd() << std::endl;
	_clients.erase(client.GetFd());
	close(client.GetFd());
	_pollfds.erase(_GetPfdFromFd(client.GetFd()));
}

vec_pfd::iterator	Server::_GetPfdFromFd(int fd)
{
	for (vec_pfd::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
	{
		if (it->fd == fd)
			return (it);
	}
	return (_pollfds.end());
}


void	Server::_ExecCommand(Command cmd, Client &client)
{
	cmd.DebugCommand();

	int	res = _ResolveOption(cmd.GetCinfo()[command]);
	if (res == UNKNOWN)
	{
		std::cout << "Unknow command" << std::endl;
		return ;
	}
	CALL_MEMBER_FN(*this, _funcTable[_ResolveOption(cmd.GetCinfo()[command])]) (cmd, client);
}

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

Client* Server::_FindNickname(const string &nick, Client *skip) 
{
	for (map_int_cli::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (&it->second != skip && it->second.GetUinfo()[nickname] == nick)
			return (&it->second);
	}
	return (NULL);
}

Client* Server::_FindUsername(const string &name, Client *skip) 
{
	for (map_int_cli::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (&it->second != skip && it->second.GetUinfo()[username] == name)
			return (&it->second);
	}
	return (NULL);
}

lst_chan::iterator	Server::_FindChannel(const string &name)
{
	for (lst_chan::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (it->GetName() == name)
				return (it);
	}
	return (_channels.end());
}

void	Server::_NoticeServ(const string str, Client &client, int q)
{
	//if int q != 0 -> notice a quit from someone
	if (client.isOperator() == false && q == 0)
		return (AddData(ERR_NOPRIVILEGES));
	for (std::map< int, Client >::iterator _it = _clients.begin(); _it != _clients.end(); ++_it)
	{
		if (&client == &_it->second)
			continue ;
		string msg = "NOTICE " + (_it->second).GetUinfo()[nickname] + " :" + str + "\r\n";
		AddData(msg);
		SendData((_it->second).GetFd());
	}
}

cst_vec_vec_str	&Server::_WrapChannels(Command &cmd, size_t pos)
{
	try {
		if (cmd.GetMiddle().size() <= pos)
			throw irc_error("no channels", 0);
		cmd.ParseChannel(cmd.GetMiddle()[pos]);
		cmd.DebugChannel();
	}
	catch (irc_error &e)
	{
		std::cout << "warning : " << e.what() << std::endl;
	}
	return (cmd.GetChannels());
}

cst_vec_str	&Server::_WrapTargets(Command &cmd, size_t pos)
{
	try {
		if (cmd.GetMiddle().size() <= pos)
			throw irc_error("no targets", 0);
		cmd.ParseTarget(cmd.GetMiddle()[pos]);
		cmd.DebugTarget();
	}
	catch (irc_error &e)
	{
		std::cout << "warning : " << e.what() << std::endl;
	}
	return (cmd.GetTargets());
}

				//------------------------------------//
				//	 *** 	SERVER-COMMANDS		 ***  //
				//------------------------------------//

void	Server::_CapLs(Command &cmd, Client &client)
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

void Server::_Oper(Command &cmd, Client &client)
{
	if (cmd.GetMiddle().size() < 1)
		return (AddData(ERR_NEEDMOREPARAMS("MODE")));
	if (cmd.GetMiddle().size() < 2 || _operPass != string(cmd.GetCinfo()[params].begin() + (cmd.GetMiddle()[0].size() + 1), cmd.GetCinfo()[params].end()))
		return (AddData(ERR_PASSWDMISMATCH(client.GetPrefix())));
	if (client.GetUinfo()[nickname] != cmd.GetMiddle()[0])
		return (AddData(ERR_NOOPERHOST(client.GetUinfo()[nickname])));
	client.SetMode('o', true);
	AddData(string("MODE ") + client.GetUinfo()[nickname] + " +o\r\n");
	AddData(RPL_YOUREOPER(client.GetUinfo()[nickname]));
}

void	Server::_Pass(Command &cmd, Client &client)
{
	vec_str			ui = client.GetUinfo();

	if (cmd.GetMiddle().empty() || cmd.GetMiddle().size() < 1)
		return AddData(ERR_NEEDMOREPARAMS("PASS"));
	if (client.IsRegistd())
		return AddData(ERR_ALREADYREGISTERED);
	ui[password] = cmd.GetCinfo()[params];
	client.SetUinfo(ui);
}

void	Server::_Nick(Command &cmd, Client &client)
{
	vec_str			ui = client.GetUinfo();
	cst_vec_str		targets = _WrapTargets(cmd, 0);

	if (ui[password] != _password)
	{
		AddData(ERR_NEEDMOREPARAMS("PASS"));
		SendData(client.GetFd());
		throw irc_error("error: invalid password", CLOSE_CONNECTION);
	}
	if (targets.empty())
		return AddData(ERR_NONICKNAMEGIVEN);
	if (cmd.isValidNick(targets[0]) == false)
		return AddData(ERR_ERRONEUSNICKNAME(targets[0]));
	if (_FindNickname(targets[0], &client) != NULL)
	{
		string tmp = "*";
		if (ui[nickname].empty() == false)
			tmp = ui[nickname];
		return AddData(ERR_NICKNAMEINUSE(tmp, targets[0]));
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
	ui[nickname] = targets[0];
	client.SetUinfo(ui);
	string data;
	if (client.IsRegistd() == true)
		AddData(from, "NICK " + ui[nickname] + "\r\n");
	if (client.IsRegistd() == false && ui[username].empty() == false)
	{
		client.SetRegistd();
		AddData(RPL_WELCOME(ui[nickname], ui[username], ui[hostname]));
	}
}

void	Server::_User(Command &cmd, Client &client)
{
	vec_str	ui = client.GetUinfo();

	if (ui[password] != _password)
	{
		AddData(ERR_NEEDMOREPARAMS("PASS"));
		SendData(client.GetFd());
		throw irc_error("error: invalid password", CLOSE_CONNECTION);
	}
	if (cmd.GetMiddle().size() == 0)
		return AddData(ERR_NEEDMOREPARAMS(cmd.GetCinfo()[command]));
	//TODO: SendData(ERR_ALREADYREGISTERED);
	if (cmd.GetMiddle().size() < 3 || cmd.GetCinfo()[trailing].empty() == true)
	{
		std::cout << "Invalid param" << std::endl;
		return ;
	}
	else
	{
		ui[username] = cmd.GetMiddle()[0];
		ui[hostname] = cmd.GetMiddle()[2];
		ui[realname] = cmd.GetCinfo()[trailing];
		client.SetUinfo(ui);
		if (client.IsRegistd() == false && ui[nickname].empty() == false)
		{
			client.SetRegistd();
			AddData(RPL_WELCOME(ui[nickname], ui[username], ui[hostname]));
		}
	}
}

void	Server::_Pong(Command &cmd, Client &client)
{
	cst_vec_str		targets = _WrapTargets(cmd, 0);

	if (targets.empty())
		return ;
	//409    ERR_NOORIGIN ":No origin specified"
	//402    ERR_NOSUCHSERVER "<server name> :No such server"
	AddData("PONG " + targets[0] + " irc\r\n", client.GetPrefix());
	SendData(client.GetFd());
}

//      ok   ERR_NORECIPIENT           ok    ERR_NOTEXTTOSEND
//           ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
//           ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
//      ok   ERR_NOSUCHNICK			         RPL_AWAY
// Parameters: <msgtarget> <text to be sent>
void	Server::_PrivMsg(Command &cmd, Client &client)
{
	Client				*receiver = NULL;
	cst_vec_str			targets = _WrapTargets(cmd, 0);
	cst_vec_vec_str		chans = _WrapChannels(cmd, 0);

	if (targets.empty() && chans.empty())
		return AddData(ERR_NORECIPIENT(cmd.GetCinfo()[message]));
	if (cmd.GetMiddle().size() > 1)
		return AddData(ERR_TOOMANYTARGETS(cmd.GetMiddle()[1], cmd.GetCinfo()[message]));
	if (cmd.GetCinfo()[trailing].empty())
		return AddData(ERR_NOTEXTTOSEND);
	if (!targets.empty())
	{
		if ((receiver = _FindNickname(targets[0])) == NULL)
			return AddData(ERR_NOSUCHNICK(targets[0]));
		AddData("PRIVMSG " + targets[0] + " :" + cmd.GetCinfo()[trailing]+ "\r\n", client.GetPrefix()); 
		SendData(receiver->GetFd());
	}
	else if (!chans.empty())
	{
		try {
			SendChannel(chans[0][chan], "PRIVMSG " + chans[0][chan] + " :" + cmd.GetCinfo()[trailing]+ "\r\n", client.GetPrefix(), &client);
		}
		catch (irc_error &e)
		{
			return AddData(e.what());
		}
	}
}

void	Server::_Mode(Command &cmd, Client &client)
{
	cst_vec_str		targets = _WrapTargets(cmd, 0);
	cst_vec_vec_str	chanparse = _WrapChannels(cmd, 0);

	if (targets.empty() && chanparse.empty())
		return (AddData(ERR_NEEDMOREPARAMS("MODE")));

	if (targets.empty() == false)
	{
		if (targets[0] != client.GetUinfo()[nickname])
			return (AddData(ERR_USERSDONTMATCH(targets[0])));
		if (cmd.GetMiddle().size() > 1)
		{
			if (cmd.isValidUserMode(cmd.GetMiddle()[1]) == false)
				return (AddData(ERR_UMODEUNKNOWNFLAG(cmd.GetMiddle()[1])));
			try {
				client.SetStrMode(cmd.GetMiddle()[1]);
			}
			catch (irc_error &e)
			{
				return (AddData(e.what()));
			}
			return (AddData(string("MODE ") + client.GetUinfo()[nickname] + " " + cmd.GetMiddle()[1] + "\r\n"));
		}
		return (AddData(RPL_UMODEIS(client.GetUinfo()[nickname], client.GetStrMode())));
	}

	return (AddData(RPL_CHANNELMODEIS(chanparse[0][chan]) /*+mode here*/ ));
}

void	Server::_Notice(Command &cmd, Client &client)
{
	(void)client;
	Client			*receiver;
	cst_vec_str		targets = _WrapTargets(cmd, 0);

	if (targets.empty())
		return ;
	if (cmd.GetCinfo()[trailing].empty()) //no text to send
		return ;
	for (std::vector<string>::const_iterator it = targets.begin(); it != targets.end(); ++it)
	{
		if (*it == SERVER_NAME)
		{
			_NoticeServ(cmd.GetCinfo()[trailing], client);
			continue ;
		}
		else if ((receiver = _FindNickname(*it)) == NULL)
			continue ;
		else
		{
			string msg = "NOTICE " + *it + " :" + cmd.GetCinfo()[trailing ]+ "\r\n";
			AddData(msg);
			SendData(receiver->GetFd());
		}
	}
}


void	Server::_Kill(Command &cmd, Client &client)
{
	Client *receiver;

	cst_vec_str		targets = _WrapTargets(cmd, 0);

	if (targets.empty())
		return ;
	if (client.isOperator() == false)
		return (AddData(ERR_NOPRIVILEGES));
	if (cmd.GetCinfo()[params].empty() == true || cmd.GetCinfo()[trailing].empty() == true || targets.empty() == true)
		return AddData(ERR_NEEDMOREPARAMS("KILL"));
	if (targets[0] == SERVER_NAME)
		return AddData(ERR_CANTKILLSERVER(client.GetPrefix()));
	if ((receiver = _FindNickname(targets[0], &client)) == NULL)
		return AddData(ERR_NOSUCHNICK(targets[0]));

	string msg = " 💀 🥷 ☠️  ⚰️ 👋 \033[0;214m " + receiver->GetUinfo()[nickname] + "'s connection has been \033[0;31mkilled\033[0;37m because :" + cmd.GetCinfo()[trailing];
	_NoticeServ(msg, client, 1);
	_CloseConnection(*receiver);
}

void	Server::_Quit(Command &cmd, Client &client)
{
	string msg = " 👋 \033[0;214m " + client.GetUinfo()[nickname] + " has \033[0;31mquit\033[0;37m because :" + cmd.GetCinfo()[trailing];
	_NoticeServ(msg, client, 1);
	AddData(client.GetPrefix(), "ERROR :" + cmd.GetCinfo()[trailing] + "\r\n");
	SendData(client.GetFd());
	throw irc_error("⚠️  warning: closing connection", CLOSE_CONNECTION);
}

				//------------------------------------//
				//	 *** 	CHANNEL-COMMANDS	***   //
				//------------------------------------//

void	Server::_Join(Command &cmd, Client &client)
{
	cst_vec_vec_str	chanparse = _WrapChannels(cmd, 0);
	lst_chan::iterator	it;

	if (chanparse.empty())
		return AddData(ERR_NEEDMOREPARAMS("JOIN"));
	for (size_t i = 0; i < chanparse.size(); ++i)
	{
		if ((it = _FindChannel(chanparse[i][chan])) == _channels.end())
		{
			_channels.push_back(Channel(chanparse[i][chan]));
			_channels.back().joinChannel(client);
			AddData(client.GetPrefix(), "JOIN " + chanparse[i][chan] + "\r\n");
			if (0)
				AddData(RPL_NOTOPIC(client.GetUinfo()[nickname], client.GetUinfo()[username], client.GetUinfo()[hostname], _channels.back().GetName()));
			else
				AddData(RPL_TOPIC(client.GetUinfo()[nickname], client.GetUinfo()[username], client.GetUinfo()[hostname], _channels.back().GetName(), "todo : call _channels.back().GetTopic()"));
			AddData(RPL_NAMREPLY(client.GetUinfo()[nickname], chanparse[i][chan]) + "@" + client.GetUinfo()[nickname] + "\r\n");
			AddData(RPL_ENDOFNAMES(client.GetUinfo()[nickname], chanparse[i][chan]));
			SendData(client.GetFd());
		}
		else
		{
			it->joinChannel(client);

			SendChannel(chanparse[i][chan], "JOIN " + chanparse[i][chan] + "\r\n", client.GetPrefix());
			if (0)
				AddData(RPL_NOTOPIC(client.GetUinfo()[nickname], client.GetUinfo()[username], client.GetUinfo()[hostname], _channels.back().GetName()));
			else
				AddData(RPL_TOPIC(client.GetUinfo()[nickname], client.GetUinfo()[username], client.GetUinfo()[hostname], _channels.back().GetName(), "todo : call _channels.back().GetTopic()"));

			string reply = RPL_NAMREPLY(client.GetUinfo()[nickname], chanparse[i][chan]) + "@" + (*it->GetUsers().begin())->GetUinfo()[nickname];
			for (lst_pcli::const_iterator i = ++it->GetUsers().begin(); i != it->GetUsers().end(); ++i)
				reply += " " + (*i)->GetUinfo()[nickname];
			reply += "\r\n";
			AddData(reply);
			AddData(RPL_ENDOFNAMES(client.GetUinfo()[nickname], chanparse[i][chan]));
			SendData(client.GetFd());
		}
	}

	return ;
}

//Parameters: <channel> *( "," <channel> ) [ <Part Message> ]
void	Server::_Part(Command &cmd, Client &client)
{
	std::list < Channel >::iterator target;
	std::cout << "Hey I'm command Part ! Nice to meet you" << std::endl;
	cst_vec_vec_str	channels = _WrapChannels(cmd, 0);

	if (channels.empty())
		return AddData(ERR_NEEDMOREPARAMS("PART"));
	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::cout << "channels[i][0] = " << channels[i][chanstring] << std::endl;
		target = _FindChannel(channels[i][chan]);
		if (target == _channels.end())
			return AddData(ERR_NOSUCHCHANNEL(channels[i][chan]));
		if (target->_findUserIter(client.GetUinfo()[nickname]) == target->GetUser().end())
			return (AddData(ERR_NOTONCHANNEL(channels[i][chan])));
		std::cout << "step 3 :TODO Leave channel" << std::endl;
//		leave_client_from_channel(it);
	}
}

// Parameters: <channel> [ <topic> ]
//            ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
// 	          RPL_NOTOPIC                     RPL_TOPIC
//            ERR_CHANOPRIVSNEEDED            ERR_NOCHANMODES
void	Server::_Topic(Command &cmd, Client &client)
{
	std::list < Channel >::iterator channel;
	cst_vec_vec_str		chans = _WrapChannels(cmd, 0);
	cst_vec_str			targets = _WrapTargets(cmd, 0);

	std::cout << "Hey I'm command Topic: to change or view topic of chan" << std::endl;
	if (chans.empty())
		return AddData(ERR_NEEDMOREPARAMS("TOPIC"));
	if ((channel = _FindChannel(chans[0][chan])) == _channels.end())
			continue ;
		if (channel->_findUserIter(client.GetUinfo()[nickname]) == channel->GetUser().end())
			return (AddData(ERR_NOTONCHANNEL(chans[0][chan])));
	if (cmd.GetMiddle().size() == 1)
	{
		//Send topic of the channel back to the client
		if (channel->GetTopic().empty())
			AddData(RPL_NOTOPIC(client.GetUinfo()[nickname], client.GetUinfo()[username], client.GetUinfo()[hostname], channel->GetName()));
		else
			AddData(RPL_TOPIC(client.GetUinfo()[nickname], client.GetUinfo()[username], client.GetUinfo()[hostname], channel->GetName(), channel->GetTopic()));
	}
	else
	{
		//check if the client is an operator of the channel
		if (channel->_findUserIter(client.GetUinfo()[nickname])->GetMode() != '@')
			return AddData(ERR_CHANOPRIVSNEEDED(chans[0][chan]));
		//change the topic of the channel
		if (cmd.GetCinfo()[trailing].empty())
			channel->SetTopic("");
		else
			channel->SetTopic(cmd.GetCinfo()[trailing]);
		//send the new topic to all the users of the channel
		SendChannel(channel->GetName(), "TOPIC " + channel->GetName() + " :" + channel->GetTopic() + "\r\n");
	}
}

////-----------------
//void	Server::_PrivMsg(Command &cmd, Client &client)
//{
//	Client				*receiver = NULL;
//	cst_vec_str			targets = _WrapTargets(cmd, 0);
//	cst_vec_vec_str		chans = _WrapChannels(cmd, 0);
//
//	if (targets.empty() && chans.empty())
//		return AddData(ERR_NORECIPIENT(cmd.GetCinfo()[message]));
//	if (cmd.GetMiddle().size() > 1)
//		return AddData(ERR_TOOMANYTARGETS(cmd.GetMiddle()[1], cmd.GetCinfo()[message]));
//	if (cmd.GetCinfo()[trailing].empty())
//		return AddData(ERR_NOTEXTTOSEND);
//	if (!targets.empty())
//	{
//		if ((receiver = _FindNickname(targets[0])) == NULL)
//			return AddData(ERR_NOSUCHNICK(targets[0]));
//		AddData("PRIVMSG " + targets[0] + " :" + cmd.GetCinfo()[trailing]+ "\r\n", client.GetPrefix()); 
//		SendData(receiver->GetFd());
//	}
//	else if (!chans.empty())
//	{
//		try {
//			SendChannel(chans[0][chan], "PRIVMSG " + chans[0][chan] + " :" + cmd.GetCinfo()[trailing]+ "\r\n", client.GetPrefix(), &client);
//		}
//		catch (irc_error &e)
//		{
//			return AddData(e.what());
//		}
//	}
//}
////-----------------
//

//Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]
//            ERR_TOOMANYMATCHES              ERR_NOSUCHSERVER
//            RPL_NAMREPLY                    RPL_ENDOFNAMES
void	Server::_Names(Command &cmd, Client &client)
{
	std::cout << "Hey I'm command Names ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}

//Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]
//           ERR_TOOMANYMATCHES              ERR_NOSUCHSERVED
//           RPL_LIST                        RPL_LISTEND
void	Server::_List(Command &cmd, Client &client)
{
	std::cout << "Hey I'm command List ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}

//   Parameters: <nickname> <channel>
//              ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
//	            ERR_NOTONCHANNEL                ERR_USERONCHANNEL
//	            ERR_CHANOPRIVSNEEDED
//	            RPL_INVITING                    RPL_AWAY
void	Server::_Invite(Command &cmd, Client &client)
{
	std::cout << "Hey I'm command Invite ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}

//   Parameters: <channel> *( "," <channel> ) <user> *( "," <user> )
//               [<comment>]
//
//           ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
//           ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
//           ERR_USERNOTINCHANNEL            ERR_NOTONCHANNEL
void	Server::_Kick(Command &cmd, Client &client)
{
	std::cout << "Hey I'm command Kick ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}
