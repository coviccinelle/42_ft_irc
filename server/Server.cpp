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
	_funcTable.push_back(&Server::_Who);
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
	_mapCmd.insert(std::make_pair(string("WHO"), WHO));
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

void	Server::SendChannel(lst_pchan::const_iterator chan, const string &message, const string &from, const Client *skip)
{
	cst_map_pcli &clients = (*chan)->GetUsers();
	for (map_pcli::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->first == skip)
			continue ;
		AddData(message, from);
		SendData(it->first->GetFd());
	}
}

void	Server::SendChannel(lst_chan::const_iterator chan, const string &message, const string &from, const Client *skip)
{
	if (chan == _channels.end())
		return ;

	cst_map_pcli &clients = chan->GetUsers();
	for (map_pcli::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->first == skip)
			continue ;
		AddData(message, from);
		SendData(it->first->GetFd());
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

void	Server::_LeaveAllChans(Client &client)
{
	while (client.GetChannels().empty() == false)
		_LeaveChannel(client.GetChannels().begin(), client);
}

void	Server::_LeaveChannel(lst_pchan::const_iterator pIt, Client &client)
{
	(*pIt)->leaveChannel(client);
	if (((*pIt)->GetUsers()).empty())
	{
		for (lst_chan::iterator chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt)
		{
			if (&*chanIt == *pIt)
			{
				_channels.erase(chanIt);
				break;
			}
		}
	}
}

void	Server::_LeaveChannel(lst_chan::iterator chanIt, Client &client)
{
	chanIt->leaveChannel(client);
	if (chanIt->GetUsers().empty())
		_channels.erase(chanIt);
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
	_LeaveAllChans(_clients[pfd.fd]);
	_clients.erase(pfd.fd);
	close(pfd.fd);
	_pollfds.erase(vec_pfd::iterator(&pfd));
}

void	Server::_CloseConnection(Client &client)
{
	std::cout << "ℹ️  irc server:\033[0;31m connection close \033[0;37mfor "<< client.GetUinfo()[nickname] << " on socket " << client.GetFd() << std::endl;
	_LeaveAllChans(client);
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
		cmd.ClearChannels();
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
		cmd.ClearTargets();
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
	{
		for (lst_pchan::const_iterator it = client.GetChannels().begin(); it != client.GetChannels().end(); ++it)
			SendChannel(it, "NICK " + ui[nickname] + "\r\n", from, &client);
		AddData("NICK " + ui[nickname] + "\r\n", from);
	}
	if (client.IsRegistd() == false && ui[username].empty() == false)
	{
		client.SetRegistd();
		AddData(RPL_WELCOME(ui[nickname], ui[username], ui[hostname]));
		AddData(RPL_YOURHOST(ui[nickname], SERVER_NAME, SERVER_VERSION));
		AddData(RPL_CREATED(ui[nickname], SERVER_DATE));
		AddData(RPL_MYINFO(ui[nickname], SERVER_NAME, SERVER_VERSION, USER_MODE, MEMBER_MODE, CHAN_MODE));
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
			AddData(RPL_YOURHOST(ui[nickname], SERVER_NAME, SERVER_VERSION));
			AddData(RPL_CREATED(ui[nickname], SERVER_DATE));
			AddData(RPL_MYINFO(ui[nickname], SERVER_NAME, SERVER_VERSION, USER_MODE, MEMBER_MODE, CHAN_MODE));
		}
	}
}

void	Server::_Pong(Command &cmd, Client &client)
{
	cst_vec_str		targets = _WrapTargets(cmd, 0);

	if (targets.empty())
		return ;
	AddData("PONG " + targets[0] + " irc\r\n", client.GetPrefix());
	SendData(client.GetFd());
}

// Parameters: <msgtarget> <text to be sent>
void	Server::_PrivMsg(Command &cmd, Client &client)
{
	Client				*receiver = NULL;
	cst_vec_str			targets = _WrapTargets(cmd, 0);
	cst_vec_vec_str		chanparse = _WrapChannels(cmd, 0);

	if (targets.empty() && chanparse.empty())
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
	else if (!chanparse.empty())
	{
		lst_chan::iterator	chanIt;

		if ((chanIt = _FindChannel(chanparse[0][chan])) == _channels.end())
			AddData(ERR_NOSUCHCHANNEL(chanparse[0][chan]));
		else if (chanIt->GetUsers().count(&client) == 0) 
			AddData(ERR_NOTONCHANNEL(chanIt->GetName()));
		else if (chanIt->IsBanned(client) || (chanIt->IsModerated() && chanIt->IsVoiced(client) == false))
			AddData(ERR_CANNOTSENDTOCHAN(chanIt->GetNickname(client), chanparse[0][chan]));
		else
			SendChannel(chanIt, "PRIVMSG " + chanparse[0][chan] + " :" + cmd.GetCinfo()[trailing]+ "\r\n", chanIt->GetOrigin(client), &client);
	}
}

void	Server::_ModeClient(Command &cmd, Client &client, const string &target)
{
	if (target != client.GetUinfo()[nickname])
		return (AddData(ERR_USERSDONTMATCH(target)));
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
		return (AddData(string("MODE ") + client.GetUinfo()[nickname] + " :" + cmd.GetMiddle()[1] + "\r\n"));
	}
	return (AddData(RPL_UMODEIS(client.GetUinfo()[nickname], client.GetStrMode())));
}

void	Server::_ModeServer(Command &cmd, Client &client, const string &channel)
{
	lst_chan::iterator	chanIt;
	if ((chanIt = _FindChannel(channel)) == _channels.end())
		AddData(ERR_NOSUCHCHANNEL(channel));
	else if (cmd.GetMiddle().size() == 1)
		AddData(RPL_CHANNELMODEIS(chanIt->GetNickname(client), channel, (chanIt->GetStrChanMode().empty() ? "" : "+") + chanIt->GetStrChanMode()));
	else if (cmd.GetMiddle().size() == 2 && (cmd.GetMiddle()[1] == "b" || cmd.GetMiddle()[1] == "+b"))
	{
		cst_lst_ban	banList = chanIt->GetBanList();
		for (lst_ban::const_iterator it = banList.begin(); it != banList.end(); ++it)
			AddData(RPL_BANLIST(chanIt->GetNickname(client), channel, it->GetMask(), it->GetFrom(), it->GetTime()));
		AddData(RPL_ENDOFBANLIST(chanIt->GetNickname(client), channel));
	}
	else if (chanIt->IsOperator(client) == false)
		AddData(ERR_CHANOPRIVSNEEDED(channel));
	else if (cmd.GetMiddle().size() == 2)
	{
		bool status = false;
		string	modeA;
		string	modeR;
		string mode;

		for (string::const_iterator it = cmd.GetMiddle()[1].begin(); it != cmd.GetMiddle()[1].end(); ++it)
		{
			if (*it == '+')
				status = true;
			else if (*it == '-')
				status = false;
			else if (CHAN_MODE.find(*it) != string::npos)
			{
				if (*it == 'a')
				{
					if (status == false)
						continue ;
					for (map_pcli::const_iterator it = chanIt->GetUsers().begin(); it != chanIt->GetUsers().end(); ++it)
						SendChannel(chanIt, "NICK anonymous\r\n", it->first->GetPrefix(), it->first);
				}
				if (status)
					modeA += *it;
				else
					modeR += *it;
				chanIt->SetChanMode(*it, status);
			}
			else
				AddData(ERR_UNKNOWNMODE(client.GetUinfo()[nickname], string(1, *it)));
		}
		if (modeA.empty() == false)
			mode += (modeA.empty() ? "" : "+" ) + modeA;
		if (modeR.empty() == false)
			mode += (modeR.empty() ? "" : "-" ) + modeR;
		if (mode.empty() == false)
			SendChannel(chanIt, "MODE " + channel + " " + mode + " \r\n", chanIt->GetOrigin(client));
	}
	else
	{
		if (cmd.GetMiddle()[1] == "+b")
		{
			chanIt->AddToBanList(chanIt->GetOrigin(client), cmd.GetMiddle()[2]);
			SendChannel(chanIt, "MODE " + channel + " +b "  + cmd.GetMiddle()[2] + "\r\n", chanIt->GetOrigin(client));
		}
		else if (cmd.GetMiddle()[1] == "-b")
		{
			chanIt->RemoveFromBanList(cmd.GetMiddle()[2]);
			SendChannel(chanIt, "MODE " + channel + " -b "  + cmd.GetMiddle()[2] + "\r\n", chanIt->GetOrigin(client));
		}
		else if (cmd.GetMiddle()[1] == "+v")
		{
			map_pcli::iterator It = chanIt->findUserIter(cmd.GetMiddle()[2]);
			if (It == chanIt->GetUsers().end())
				AddData(ERR_USERNOTINCHANNEL(cmd.GetMiddle()[2], channel));
			else
			{
				chanIt->SetMemberMode(*(It->first), 'v', true);
				SendChannel(chanIt, "MODE " + channel + " +v "  + cmd.GetMiddle()[2] + "\r\n", chanIt->GetOrigin(client));
			}
		}
		else if (cmd.GetMiddle()[1] == "-v")
		{
			Client *unVoice = _FindNickname(cmd.GetMiddle()[2]);
			if (unVoice == NULL)
				AddData(ERR_USERNOTINCHANNEL(cmd.GetMiddle()[2], channel));
			else
			{
				chanIt->SetMemberMode(*unVoice, 'v', false);
				SendChannel(chanIt, "MODE " + channel + " -v "  + cmd.GetMiddle()[2] + "\r\n", chanIt->GetOrigin(client));
			}
		}
		else if (cmd.GetMiddle()[1] == "-o")
		{
			Client *unOp = _FindNickname(cmd.GetMiddle()[2]);
			if (unOp == NULL)
				AddData(ERR_USERNOTINCHANNEL(cmd.GetMiddle()[2], channel));
			else
			{
				chanIt->SetMemberMode(*unOp, 'o', false);
				SendChannel(chanIt, "MODE " + channel + " -o "  + cmd.GetMiddle()[2] + "\r\n", chanIt->GetOrigin(client));
			}
		}
		else if (cmd.GetMiddle()[1] == "+o")
		{
			Client *unOp = _FindNickname(cmd.GetMiddle()[2]);
			if (unOp == NULL)
				AddData(ERR_USERNOTINCHANNEL(cmd.GetMiddle()[2], channel));
			else
			{
				chanIt->SetMemberMode(*unOp, 'o', true);
				SendChannel(chanIt, "MODE " + channel + " +o "  + cmd.GetMiddle()[2] + "\r\n", chanIt->GetOrigin(client));
			}
		}
		else
			AddData(ERR_UNKNOWNMODE(cmd.GetMiddle()[1], channel));
	}

	return ;
}

void	Server::_Mode(Command &cmd, Client &client)
{
	cst_vec_str		targets = _WrapTargets(cmd, 0);
	cst_vec_vec_str	chanparse = _WrapChannels(cmd, 0);

	if (chanparse.empty() && targets.empty())
		AddData(ERR_NEEDMOREPARAMS("MODE"));
	else if (chanparse.empty())
		_ModeClient(cmd, client, targets[0]);
	else
		_ModeServer(cmd, client, chanparse[0][chan]);

	return ;
}

void	Server::_Notice(Command &cmd, Client &client)
{
	(void)client;
	Client			*receiver;
	cst_vec_str		targets = _WrapTargets(cmd, 0);
	lst_chan::iterator	chanIt;
	

	if (cmd.GetCinfo()[trailing].empty() || cmd.GetMiddle().size() != 1)
		return ;
	if (cmd.GetMiddle()[0] == SERVER_NAME)
		_NoticeServ(cmd.GetCinfo()[trailing], client);
	else if ((receiver = _FindNickname(cmd.GetMiddle()[0])) != NULL)
	{
		string msg = "NOTICE " + cmd.GetMiddle()[0]+ " :" + cmd.GetCinfo()[trailing] + "\r\n";
		AddData(msg, client.GetPrefix());
		SendData(receiver->GetFd());
	}
	else if ((chanIt = _FindChannel(cmd.GetMiddle()[0])) != _channels.end())
	{
		string msg = "NOTICE " + cmd.GetMiddle()[0] + " :" + cmd.GetCinfo()[trailing] + "\r\n";
		SendChannel(chanIt, msg, chanIt->GetOrigin(client), &client);
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
	for (std::list< Channel* >::const_iterator it = client.GetChannels().begin(); it != client.GetChannels().end(); ++it)
	{
		if ((*it)->IsAnon())
			SendChannel(it, string("PART ") + (*it)->GetName() + (cmd.GetCinfo()[trailing].empty() ? string("") : string(" :") + cmd.GetCinfo()[trailing]) + "\r\n", (*it)->GetOrigin(client));
		else
			SendChannel(it, "QUIT " + cmd.GetCinfo()[trailing] + "\r\n", (*it)->GetOrigin(client), &client);
	}
	AddData("ERROR :" + cmd.GetCinfo()[trailing] + "\r\n", client.GetPrefix());
	SendData(client.GetFd());
	throw irc_error("⚠️  warning: closing connection", CLOSE_CONNECTION);
}

				//------------------------------------//
				//	 *** 	CHANNEL-COMMANDS	***   //
				//------------------------------------//

void	Server::_Join(Command &cmd, Client &client)
{
	cst_vec_vec_str	chanparse = _WrapChannels(cmd, 0);
	lst_chan::iterator	chanIt;

	if (chanparse.empty())
		return AddData(ERR_NEEDMOREPARAMS("JOIN"));
	for (size_t i = 0; i < chanparse.size(); ++i)
	{
		if ((chanIt = _FindChannel(chanparse[i][chan])) == _channels.end())
		{
			_channels.push_back(Channel(chanparse[i][chan]));
			--chanIt;
		}

		if (chanIt->IsBanned(client) == true)
			AddData(ERR_BANNEDFROMCHAN(chanIt->GetNickname(client), chanparse[0][chan]));
		else if  (chanIt->IsInvited(client) == false)
			AddData(ERR_INVITEONLYCHAN(chanIt->GetNickname(client), chanparse[0][chan]));
		else
		{
			chanIt->joinChannel(client);
			SendChannel(chanIt, string("JOIN " + chanparse[i][chan] + "\r\n"), chanIt->GetOrigin(client));
			AddData("MODE " + chanparse[i][chan] + " " + string("+") + chanIt->GetStrChanMode() + "\r\n");
			AddData(RPL_TOPIC(chanIt->GetOrigin(client), chanIt->GetName(), chanIt->GetTopic()));
			AddData(RPL_TOPICWHOTIME(chanIt->GetNickname(client), chanIt->GetName(), chanIt->GetTopicStat()));
			AddData(RPL_NAMREPLY(chanIt->GetNickname(client), chanparse[i][chan]) + chanIt->GetLstNickname() + "\r\n");
			AddData(RPL_ENDOFNAMES(chanIt->GetNickname(client), chanparse[i][chan]));
			SendData(client.GetFd());
			if (chanIt->IsOperator(client))
				SendChannel(chanIt, "MODE " + chanIt->GetName() + " +o "  + chanIt->GetNickname(client) + "\r\n", chanIt->GetOrigin(client), &client);
		}
	}

	return ;
}


void	Server::_Part(Command &cmd, Client &client)
{
	cst_vec_vec_str	chanparse = _WrapChannels(cmd, 0);
	lst_chan::iterator	chanIt;

	if (chanparse.empty())
		return AddData(ERR_NEEDMOREPARAMS("PART"));
	for (size_t i = 0; i < chanparse.size(); ++i)
	{
		if ((chanIt = _FindChannel(chanparse[i][chan])) == _channels.end())
			AddData(ERR_NOSUCHCHANNEL(chanparse[i][chan]));
		else if (chanIt->GetUsers().count(&client) == 0)
			AddData(ERR_NOTONCHANNEL(chanIt->GetName()));
		else
		{
			_LeaveChannel(chanIt, client);
			SendChannel(chanIt, string("PART ") + chanIt->GetName() + (cmd.GetCinfo()[trailing].empty() ? string("") : string(" :") + cmd.GetCinfo()[trailing]) + "\r\n", chanIt->GetOrigin(client));
			AddData(string("PART ") + chanIt->GetName() + (cmd.GetCinfo()[trailing].empty() ? string("") : string(" :") + cmd.GetCinfo()[trailing]) + "\r\n", client.GetPrefix());
		}
	}
}

// Parameters: <channel> [ <topic> ]
//            ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
// 	          RPL_NOTOPIC                     RPL_TOPIC
//            ERR_CHANOPRIVSNEEDED            ERR_NOCHANMODES
void	Server::_Topic(Command &cmd, Client &client)
{
	cst_vec_vec_str		chans = _WrapChannels(cmd, 0);
	lst_chan::iterator	chanIt;

	if (chans.empty())
		AddData(ERR_NEEDMOREPARAMS("TOPIC"));
	else if ((chanIt = _FindChannel(chans[0][chan])) == _channels.end())
		AddData(ERR_NOSUCHCHANNEL(chans[0][chan]));
	else if (chanIt->findUserIter(chanIt->GetNickname(client)) == chanIt->GetUsers().end())
		AddData(ERR_NOTONCHANNEL(chans[0][chan]));
	else if (cmd.GetCinfo()[trailing].empty())
	{
		AddData(RPL_TOPIC(chanIt->GetNickname(client), chanIt->GetName(), chanIt->GetTopic()), chanIt->GetOrigin(client));
		SendData(client.GetFd());
	}
	else if (chanIt->IsOpTopicOnly() == true && chanIt->IsOperator(client) == false)
		AddData(ERR_CHANOPRIVSNEEDED(chans[0][chan]));
	else
	{
		chanIt->SetTopic(cmd.GetCinfo()[trailing]);
		SendChannel(chanIt, "TOPIC " + chanIt->GetName() + " :" + chanIt->GetTopic() + "\r\n", chanIt->GetOrigin(client));
	}

	return ;
}

//Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]
//            ERR_TOOMANYMATCHES              
//            RPL_NAMREPLY                    RPL_ENDOFNAMES
void	Server::_Names(Command &cmd, Client &client)
{
	cst_vec_vec_str		chans = _WrapChannels(cmd, 0);

	if (chans.empty() && cmd.GetMiddle().empty() && cmd.GetCinfo()[trailing].empty())
	{
		for (lst_chan::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			AddData(RPL_NAMREPLY(it->GetNickname(client), it->GetName()) + it->GetLstNickname() + "\r\n");
			AddData(RPL_ENDOFNAMES(it->GetNickname(client), it->GetName()));
		}
		return ;
	}
	else if (chans.size() > 0 && cmd.GetCinfo()[trailing].empty())
	{
		lst_chan::iterator	chanIt;
		for (size_t i = 0; i < chans.size(); ++i)
		{
			if ((chanIt = _FindChannel(chans[i][chan])) == _channels.end())
				AddData(ERR_NOSUCHCHANNEL(chans[i][chan]));
			else
			{
				AddData(RPL_NAMREPLY(chanIt->GetNickname(client), chanIt->GetName()) + chanIt->GetLstNickname() + "\r\n");
				AddData(RPL_ENDOFNAMES(chanIt->GetNickname(client), chanIt->GetName()));
			}
		}
		return ;
	}
	else
	{
		AddData("NAMES: Parameters: [ <channel> *( \",\" <channel> )\r\n");
		return ;
	}
}

//Parameters: [ <channel> *( "," <channel> ) [ <target> ] ]
void	Server::_List(Command &cmd, Client &client)
{
	if (cmd.GetMiddle().empty())
	{
		for (lst_chan::iterator it = _channels.begin(); it != _channels.end(); ++it)
			AddData(RPL_LIST(it->GetNickname(client), (*it).GetName(), (*it).GetTopic()));
		AddData(RPL_LISTEND(client.GetUinfo()[nickname]));
	}
	else
	{
		AddData("List: how to use : \"/list -YES\" on irssi OR \"LIST\" on netcat\r\n");
		return ;
	}
}

void	Server::_Invite(Command &cmd, Client &client)
{
	cst_vec_vec_str	chanparse = _WrapChannels(cmd, 1);
	lst_chan::iterator			chanIt;
	Client						*toInvite;

	if (cmd.GetMiddle().size() < 2)
		AddData(ERR_NEEDMOREPARAMS("INVITE"));
	else if (chanparse.empty() || chanparse[0].empty())
		AddData(ERR_NOTONCHANNEL(cmd.GetMiddle()[1]));
	else if ((chanIt = _FindChannel(chanparse[0][chan])) == _channels.end())
		AddData(ERR_NOSUCHCHANNEL(chanparse[0][chan]));
	else if (chanIt->findUserIter(chanIt->GetNickname(client)) == chanIt->GetUsers().end())
		AddData(ERR_NOTONCHANNEL(chanIt->GetName()));
	else if (chanIt->IsInvite() && chanIt->IsOperator(client) == false)
		AddData(ERR_CHANOPRIVSNEEDED(cmd.GetMiddle()[1]));
	else if ((toInvite = _FindNickname(cmd.GetMiddle()[0], &client)) == NULL)
		AddData(ERR_NOSUCHNICK(cmd.GetMiddle()[0]));
	else if (chanIt->findUserIter(cmd.GetMiddle()[0]) != chanIt->GetUsers().end())
		AddData(ERR_USERONCHANNEL(cmd.GetMiddle()[0], chanparse[0][chan]));
	else
	{
		AddData(string("INVITE ") + toInvite->GetUinfo()[nickname] + " " + chanIt->GetName() + "\r\n", client.GetPrefix());
		SendData(toInvite->GetFd());
		chanIt->AddToInviteList(chanIt->GetNickname(client), cmd.GetMiddle()[0]);
		AddData(RPL_INVITING(client.GetPrefix(), cmd.GetMiddle()[0], chanparse[0][chan]));
	}
}

void	Server::_Kick(Command &cmd, Client &client)
{
	cst_vec_vec_str	chanparse = _WrapChannels(cmd, 0);
	lst_chan::iterator			chanIt;
	map_pcli::const_iterator	toKick;	

	if (chanparse.empty())
		AddData(ERR_NEEDMOREPARAMS("JOIN"));
	else if ((chanIt = _FindChannel(chanparse[0][chan])) == _channels.end())
		AddData(ERR_NOSUCHCHANNEL(chanparse[0][chan]));
	else if (chanIt->GetUsers().count(&client) == 0)
		AddData(ERR_NOTONCHANNEL(chanparse[0][chan]));
	else if ((toKick = chanIt->findUserIter(cmd.GetMiddle()[1])) == chanIt->GetUsers().end())
		AddData(ERR_USERNOTINCHANNEL(cmd.GetMiddle()[1], chanparse[0][chan]));
	else if (chanIt->IsOperator(client) == false)
		AddData(ERR_CHANOPRIVSNEEDED(chanparse[0][chan]));
	else
	{
		SendChannel(chanIt, string("KICK ") + chanIt->GetName() + " " + toKick->first->GetUinfo()[nickname] + " :" + cmd.GetCinfo()[trailing] + "\r\n", chanIt->GetOrigin(client));
		_LeaveChannel(chanIt, *(toKick->first));
	}
}

void	Server::_Who(Command &cmd, Client &client)
{
	cst_vec_str		targets = _WrapTargets(cmd, 0);
	cst_vec_vec_str	chanparse = _WrapChannels(cmd, 0);

	if (chanparse.empty() && targets.empty())
		AddData(ERR_NEEDMOREPARAMS("MODE"));
	else if (chanparse.empty())
	{
		;
	}
	else
	{
		string channel = chanparse[0][chan];
		lst_chan::iterator	chanIt = _FindChannel(channel);
		AddData(RPL_CREATIONTIME(chanIt->GetNickname(client), channel, chanIt->GetCtime()));
		AddData(RPL_WHOREPLY(chanIt->GetNickname(client), channel, client.GetUinfo()[username], client.GetUinfo()[hostname], SERVER_NAME, client.GetUinfo()[realname]));
		AddData(RPL_ENDOFWHO(chanIt->GetNickname(client), channel));
	}
}
