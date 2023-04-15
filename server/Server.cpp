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

void Server::SendData(int fd)
{
	std::cout << "Sending data :[" << _data.data() << "]" << std::endl;
	if (send(fd, _data.data(), _data.size(), 0) == -1)
		std::cerr << "⚠️ warning : send err" << std::endl;
	_data.clear();
}

void	Server::AddData(const string &from, const string &message, int n)
{
	if (n != 0)
	{
		_data = message;
		return ;
	}
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


void	Server::_ExecCommand(const Command &cmd, Client &client)
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
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (&it->second != skip && it->second.GetUinfo()[nickname] == nick)
			return (&it->second);
	}
	return (NULL);
}

Client* Server::_FindUsername(const string &name, Client *skip) 
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (&it->second != skip && it->second.GetUinfo()[username] == name)
			return (&it->second);
	}
	return (NULL);
}

void	Server::_NoticeServ(const string str, Client &client, int q)
{
	//if int q != 0 -> notice a quit from someone
	if (client.isOperator() == false && q == 0)
		return (AddData(SERVER_NAME, ERR_NOPRIVILEGES));
	for (std::map< int, Client >::iterator _it = _clients.begin(); _it != _clients.end(); ++_it)
	{
		if (&client == &_it->second)
			continue ;
		string msg = "NOTICE " + (_it->second).GetUinfo()[nickname] + " :" + str + "\r\n";
		AddData(SERVER_NAME, msg);
		SendData((_it->second).GetFd());
	}
}
				//------------------------------------//
				//	 *** 	SERVER-COMMANDS		 ***  //
				//------------------------------------//

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

void Server::_Oper(const Command &cmd, Client &client)
{
	if (cmd.GetMiddle().size() < 1)
		return (AddData(SERVER_NAME, ERR_NEEDMOREPARAMS("MODE")));
	if (cmd.GetMiddle().size() < 2 || _operPass != string(cmd.GetCinfo()[params].begin() + (cmd.GetMiddle()[0].size() + 1), cmd.GetCinfo()[params].end()))
		return (AddData(SERVER_NAME, ERR_PASSWDMISMATCH(client.GetPrefix())));
	if (client.GetUinfo()[nickname] != cmd.GetMiddle()[0])
		return (AddData(SERVER_NAME, ERR_NOOPERHOST(client.GetUinfo()[nickname])));
	client.SetMode('o', true);
	AddData(SERVER_NAME, string("MODE ") + client.GetUinfo()[nickname] + " +o\r\n");
	AddData(SERVER_NAME, RPL_YOUREOPER(client.GetUinfo()[nickname]));
}

void	Server::_Pass(const Command &cmd, Client &client)
{
	vec_str			ui = client.GetUinfo();

	if (cmd.GetMiddle().empty() || cmd.GetMiddle().size() < 1)
		return AddData(SERVER_NAME, ERR_NEEDMOREPARAMS("PASS"));
	if (client.IsRegistd())
		return AddData(SERVER_NAME, ERR_ALREADYREGISTERED);
	ui[password] = cmd.GetCinfo()[params];
	client.SetUinfo(ui);
}

void	Server::_Nick(const Command &cmd, Client &client)
{
	vec_str			ui = client.GetUinfo();
	TargetParser	parser;
	cst_vec_str		*targets;

	try {
		parser.ParseTarget(cmd.GetMiddle()[0]);
		parser.DebugTarget();
		targets = &parser.GetTargets();
	}
	catch (irc_error &e)
	{
		std::cout << "warning : " << e.what() << std::endl;
		return ;
	}

	if (ui[password] != _password)
	{
		AddData(SERVER_NAME, ERR_NEEDMOREPARAMS("PASS"));
		SendData(client.GetFd());
		throw irc_error("error: invalid password", CLOSE_CONNECTION);
	}
	if (targets->size() != 1)
		return AddData(SERVER_NAME, ERR_NONICKNAMEGIVEN);
	if (parser.isValidNick((*targets)[0]) == false)
		return AddData(SERVER_NAME, ERR_ERRONEUSNICKNAME((*targets)[0]));
	if (_FindNickname((*targets)[0], &client) != NULL)
	{
		string tmp = "*";
		if (ui[nickname].empty() == false)
			tmp = ui[nickname];
		return AddData(SERVER_NAME, ERR_NICKNAMEINUSE(tmp, (*targets)[0]));
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
	ui[nickname] = (*targets)[0];
	client.SetUinfo(ui);
	string data;
	if (client.IsRegistd() == true)
		AddData(from, "NICK " + ui[nickname] + "\r\n");
	if (client.IsRegistd() == false && ui[username].empty() == false)
	{
		client.SetRegistd();
		AddData(SERVER_NAME, RPL_WELCOME(ui[nickname], ui[username], ui[hostname]));
	}
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
	if (cmd.GetMiddle().size() == 0)
		return AddData(SERVER_NAME, ERR_NEEDMOREPARAMS(cmd.GetCinfo()[command]));
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
			AddData(SERVER_NAME, RPL_WELCOME(ui[nickname], ui[username], ui[hostname]));
		}
	}
}

void	Server::_Pong(const Command &cmd, Client &client)
{
	TargetParser	parser;
	cst_vec_str		*targets;

	try {
		parser.ParseTarget(cmd.GetMiddle()[0]);
		parser.DebugTarget();
		targets = &parser.GetTargets();
	}
	catch (irc_error &e)
	{
		std::cout << "warning : " << e.what() << std::endl;
		return ;
	}
	//409    ERR_NOORIGIN ":No origin specified"
	//402    ERR_NOSUCHSERVER "<server name> :No such server"
	AddData(client.GetPrefix(), "PONG " + (*targets)[0] + " irc\r\n");
	SendData(client.GetFd());
}

void	Server::_PrivMsg(const Command &cmd, Client &client)
{
	Client			*receiver;

	TargetParser	parser;
	cst_vec_str		*targets;
	try {
		parser.ParseTarget(cmd.GetMiddle()[0]);
		parser.DebugTarget();
		targets = &parser.GetTargets();
	}
	catch (irc_error &e)
	{
		std::cout << "warning : " << e.what() << std::endl;
		return ;
	}

	if (cmd.GetMiddle().size() == 0)
		return AddData(SERVER_NAME, ERR_NORECIPIENT(cmd.GetCinfo()[message]));
	if (cmd.GetMiddle().size() > 1)
		return AddData(SERVER_NAME, ERR_TOOMANYTARGETS(cmd.GetMiddle()[1], cmd.GetCinfo()[message]));
	if (cmd.GetCinfo()[trailing].empty())
		return AddData(SERVER_NAME, ERR_NOTEXTTOSEND);
	if ((receiver = _FindNickname((*targets)[0])) == NULL)
		return AddData(SERVER_NAME, ERR_NOSUCHNICK((*targets)[0]));
	// TODO: Check also in the list of #channels

	const string msg = "PRIVMSG " + (*targets)[0] + " :" + cmd.GetCinfo()[trailing]+ "\r\n";
	std::cout << "fd : " << client.GetFd() << std::endl;
	std::cout << "nickname : " << client.GetUinfo()[nickname] << std::endl;
	std::cout << "prefix : " << client.GetPrefix() << std::endl;
	AddData(client.GetPrefix(), msg); 
	SendData(receiver->GetFd());
}

void	Server::_Mode(const Command &cmd, Client &client)
{
	TargetParser	parser;
	cst_vec_str		*targets;
	try {
		parser.ParseTarget(cmd.GetMiddle()[0]);
		targets = &parser.GetTargets();
	}
	catch (irc_error &e)
	{
		std::cout << "warning : " << e.what() << std::endl;
		return ;
	}

	if (cmd.GetMiddle().size() == 0)
		return (AddData(SERVER_NAME, ERR_NEEDMOREPARAMS("MODE")));
	if ((*targets)[0] != client.GetUinfo()[nickname])
		return (AddData(SERVER_NAME, ERR_USERSDONTMATCH((*targets)[0])));
	if (cmd.GetMiddle().size() > 1)
	{
		if (parser.isValidUserMode(cmd.GetMiddle()[1]) == false)
			return (AddData(SERVER_NAME, ERR_UMODEUNKNOWNFLAG(cmd.GetMiddle()[1])));
		try {
			client.SetStrMode(cmd.GetMiddle()[1]);
		}
		catch (irc_error &e)
		{
			return (AddData(SERVER_NAME, e.what()));
		}
		return (AddData(SERVER_NAME, string("MODE ") + client.GetUinfo()[nickname] + " " + cmd.GetMiddle()[1] + "\r\n"));
	}
	return (AddData(SERVER_NAME, RPL_UMODEIS(client.GetUinfo()[nickname], client.GetStrMode())));
}

void	Server::_Notice(const Command &cmd, Client &client)
{
	(void)client;
	Client			*receiver;

	TargetParser	parser;
	cst_vec_str		*targets;
	try {
		parser.ParseTarget(cmd.GetMiddle()[0]);
		parser.DebugTarget();
		targets = &parser.GetTargets();
	}
	catch (irc_error &e)
	{
		std::cout << "warning : " << e.what() << std::endl;
		return ;
	}

	if (cmd.GetCinfo()[trailing].empty()) //no text to send
		return ;
	for (std::vector<string>::const_iterator it = targets->begin(); it != targets->end(); ++it)
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
			AddData(SERVER_NAME, msg);
			SendData(receiver->GetFd());
		}
	}
}


void	Server::_Kill(const Command &cmd, Client &client)
{
	Client *receiver;

	TargetParser	parser;
	cst_vec_str		*targets;
	try {
		parser.ParseTarget(cmd.GetMiddle()[0]);
		parser.DebugTarget();
		targets = &parser.GetTargets();
	}
	catch (irc_error &e)
	{
		std::cout << "warning : " << e.what() << std::endl;
		return ;
	}

	if (client.isOperator() == false)
		return (AddData(SERVER_NAME, ERR_NOPRIVILEGES));
	if (cmd.GetCinfo()[params].empty() == true || cmd.GetCinfo()[trailing].empty() == true || (*targets).empty() == true)
		return AddData(SERVER_NAME, ERR_NEEDMOREPARAMS("KILL"));
	if ((*targets)[0] == SERVER_NAME)
		return AddData(SERVER_NAME, ERR_CANTKILLSERVER(client.GetPrefix()));
	if ((receiver = _FindNickname((*targets)[0], &client)) == NULL)
		return AddData(SERVER_NAME, ERR_NOSUCHNICK((*targets)[0]));

	string msg = " 💀 🥷 ☠️  ⚰️ 👋 \033[0;214m " + receiver->GetUinfo()[nickname] + "'s connection has been \033[0;31mkilled\033[0;37m because :" + cmd.GetCinfo()[trailing] + "\r\n";
	_NoticeServ(msg, client, 1);
	_CloseConnection(*receiver);
}

void	Server::_Quit(const Command &cmd, Client &client)
{
	string msg = " 👋 \033[0;214m " + client.GetUinfo()[nickname] + " has \033[0;31mquit\033[0;37m because :" + cmd.GetCinfo()[trailing] + "\r\n";
	_NoticeServ(msg, client, 1);
	AddData(client.GetPrefix(), "ERROR :" + cmd.GetCinfo()[trailing] + "\r\n");
	SendData(client.GetFd());
	throw irc_error("⚠️  warning: closing connection", CLOSE_CONNECTION);
}

				//------------------------------------//
				//	 *** 	CHANNEL-COMMANDS	***   //
				//------------------------------------//

void	Server::_Join(const Command &cmd, Client &client)
{
	(void)cmd;
	(void)client;

	if (cmd.GetCinfo()[params].empty() == true)
		return AddData(SERVER_NAME, ERR_NEEDMOREPARAMS("JOIN"));
	try
	{
//		_parser.Parse(cmd.GetCinfo()[params]);
//		ChannelParse cp = _parser.GetChan();
//		cp.Debug();
	}
	catch (irc_error &e)
	{
		std::cout << "⚠️  warning: " << e.what() << std::endl;
		return ;
	}
	_channels.push_back(Channel());
	Channel &ch = _channels.back();
	ch.joinChannel(client);

	return ;
}

void	Server::_Part(const Command &cmd, Client &client)
{
	std::cout << "Hey I'm command Part ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}

void	Server::_Topic(const Command &cmd, Client &client)
{
	std::cout << "Hey I'm command Topic ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}

void	Server::_Names(const Command &cmd, Client &client)
{
	std::cout << "Hey I'm command Names ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}

void	Server::_List(const Command &cmd, Client &client)
{
	std::cout << "Hey I'm command List ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}

void	Server::_Invite(const Command &cmd, Client &client)
{
	std::cout << "Hey I'm command Invite ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}

void	Server::_Kick(const Command &cmd, Client &client)
{
	std::cout << "Hey I'm command Kick ! Nice to meet you" << std::endl;
	(void)cmd;
	(void)client;
}
