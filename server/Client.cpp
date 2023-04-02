#include "../include/Client.hpp"

Client::Client() :
	_fd(0),
	_addr(),
	_addrSize(sizeof(_addr)),
	_ip(""),
	_servPass(""),
	_buf(""),
	_cmds(0),
	_mapCmd(),
	_validPass(false),
	_uinfo(INF_CLI_SIZE),
	_clients(NULL)
{
	// Containers are going to complain if not existing
	std::cout << "Warning: Coplien only but should never be called" << std::endl;
}

Client::Client(const string &pass, const std::map< int, Client > &clients) :
	_fd(0),
	_addr(),
	_addrSize(sizeof(_addr)),
	_ip(""),
	_servPass(pass),
	_buf(""),
	_cmds(0),
	_mapCmd(),
	_validPass(false),
	_uinfo(INF_CLI_SIZE),
	_clients(&clients)
{
	memset(&_addr, 0, sizeof(_addr));
	_mapCmd.insert(std::make_pair(string("CAP"), CAP));
	_mapCmd.insert(std::make_pair(string("PASS"), PASS));
	_mapCmd.insert(std::make_pair(string("NICK"), NICK));
	_mapCmd.insert(std::make_pair(string("USER"), USER));
	return ;
}

Client::~Client(void)
{
	return ;
}

Client::Client(Client const &src)
{
	_fd = src._fd;
	memcpy(&_addr, &src._addr, sizeof(src._addr));
	memcpy(&_addrSize, &src._addrSize, sizeof(src._addrSize));
	_ip = src._ip;
	_servPass = src._servPass;

	_buf = src._buf;
	_cmds = src._cmds;
	_mapCmd = src._mapCmd;

	_validPass = src._validPass;
	_uinfo = src._uinfo;

	_clients = src._clients;
	return ;
}

Client &Client::operator=(Client const &rhs)
{
	if (this == &rhs)
		return (*this);
	_fd = rhs._fd;
	memcpy(&_addr, &rhs._addr, sizeof(rhs._addr));
	memcpy(&_addrSize, &rhs._addrSize, sizeof(rhs._addrSize));
	_ip = rhs._ip;
	_servPass = rhs._servPass;

	_buf = rhs._buf;
	_cmds = rhs._cmds;
	_mapCmd = rhs._mapCmd;

	_validPass = rhs._validPass;
	_uinfo = rhs._uinfo;

	_clients = rhs._clients;
	return (*this);
}

int	Client::AcceptClient(int listener)
{
	char ip[INET6_ADDRSTRLEN];

	_fd = accept(listener, (struct sockaddr *)&_addr, &_addrSize);
	if (_fd != -1)
	{
		inet_ntop(_addr.ss_family, GetInAddr((struct sockaddr *)&_addr), ip, INET6_ADDRSTRLEN);
		_ip = ip;
	}
	else
		std::cerr << "⚠️  warning : accept failed" << std::endl;

	return (_fd);
}

int	Client::GetFd() const
{
	return (_fd);
}


const string	&Client::GetIp() const
{
	return (_ip);
}

cst_vec_str	&Client::GetUinfo() const
{
	return (_uinfo);
}

const std::vector< Command >	&Client::GetCmds() const
{
	return (_cmds);
}

void	Client::_User(Command &cmd)
{
	if (cmd.middle.size() == 0)
	{
		SendData(ERR_NEEDMOREPARAMS(cmd.command));
		return ;
	}
	//TODO: SendData(ERR_ALREADYREGISTERED);
	if (cmd.middle.size() < 3 || _uinfo[nickname].empty() || cmd.trailing.empty() == true)
	{
		std::cout << "Invalid param" << std::endl;
		return ;
	}
	else
	{
		_uinfo[username] = cmd.middle[0];
		_uinfo[hostname] = cmd.middle[1];
		_uinfo[servername] = cmd.middle[2];
		_uinfo[realname] = cmd.trailing;
		SendData(RPL_WELCOME(_uinfo[nickname], _uinfo[username], _uinfo[hostname]));
	}
}

void	Client::_Nick(Command &cmd)
{
	if (_validPass == false)
		throw irc_error(string(":server@127.0.0.1") + string(" please register PASS before retry.\r\n"), SEND_ERROR);
	if (cmd.target.size() != 1)
		throw irc_error(ERR_NONICKNAMEGIVEN, SEND_ERROR);
	_uinfo[nickname] = cmd.target[0];
	string msg = ":" + _uinfo[nickname];
	if (_uinfo[username].empty() == false)
		msg += "!" + _uinfo[username];
	if (_uinfo[hostname].empty() == false)
		msg += "@" + _uinfo[hostname];
	msg +=  + " NICK " + _uinfo[nickname] + "\r\n";
	SendData(msg);
	std::cout << "NICK has been set to " << _uinfo[nickname] << std::endl;
}

void	Client::_Pass(Command &cmd)
{
	if (cmd.middle.size() < 1)
		throw irc_error(ERR_NEEDMOREPARAMS(cmd.middle[0]), SEND_ERROR);
	if (_validPass)
		throw irc_error(ERR_ALREADYREGISTERED, SEND_ERROR);
	if (cmd.middle[0] != _servPass)
		throw irc_error(string(":") + _ip + string(" invalid password please retry.\r\n"), SEND_ERROR);
	std::cout << "ℹ️  irc server:\033[0;32m valid pass \033[0;37mfrom " << _ip << " on socket " << _fd << std::endl;
	_validPass = true;
}

void	Client::_Ping(Command &cmd)
{
	(void)cmd;
	/*
	(void)cmd;
	std::cout << "ping command received" << std::endl;
	*/
}

void	Client::_CapLs(Command &cmd)
{
	(void)cmd;
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

// Mapping between string comands name and enum type ex: "PASS" (string) -> PASS (int)
// Used for switch case
CmdVal	Client::ResolveOption(const string &input)
{
	if (input.empty())
		return (UNKNOWN);
	std::map<string, CmdVal >::const_iterator it(_mapCmd.find(input));
	if(it != _mapCmd.end())
		return (it->second);
	return (UNKNOWN); 
}

void	Client::ExecCommand(Command &cmd)
{
	cmd.Debug();
	switch (ResolveOption(cmd.command))
	{
		case CAP:
		{
			_CapLs(cmd);
			break ;
		}
		case PASS:
		{
			_Pass(cmd);
			break ;
		}
		case NICK:
		{
			_Nick(cmd);
			break ;
		}
		case USER:
		{
			_User(cmd);
			break ;
		}
		default :
			std::cout << "Unknow command" << std::endl;
	}
}

void	Client::_ParseBuf(const string &buf)
{
	vec_str	raw_cmds;

	size_t pos;
	_buf += buf;
	if ((pos = _buf.find_last_of("\n")) == string::npos)
		return ;
	raw_cmds = Split(string(_buf.begin(), _buf.begin() + pos), "\r\n");
	_buf = trim(string(_buf.begin() + pos, _buf.end()));
	for (vec_str::iterator it = raw_cmds.begin(); it != raw_cmds.end(); ++it)
	{
		try
		{
			_cmds.push_back(_parser.Parse(*it));
		}
		catch (irc_error &e)
		{
			std::cout << e.what() << std::endl;
			throw ;
		}
	}
}

void	Client::ParseRecv(const string &buf)
{
	_ParseBuf(buf);
	if (_cmds.empty())
	{
		std::cerr << "⚠️  warning : empty commands" << std::endl;
		return ;
	}

	while (_cmds.empty() == 0)
	{
		try {
			ExecCommand(_cmds[0]);
		}
		catch (irc_error &e)
		{
			_cmds.erase(_cmds.begin());
			throw;
		}
		_cmds.erase(_cmds.begin());
	}
	return ;
}

void Client::SendData(const string &msg) const
{
	std::cout << "Sending data :[" << msg << "]" << std::endl;
	ssize_t ret = send(_fd, msg.data(), msg.size(), 0);
	if (ret == -1)
		std::cerr << "⚠️ warning : send err" << std::endl;
}

void	Client::ValidNickname(const string &nick)
{
	if (nick.size() > 9)
		throw irc_error(ERR_ERRONEUSNICKNAME(nick), SEND_ERROR);
	string s("-_[]{}\\`|");
	for (string::const_iterator i = nick.begin(); i != nick.end(); ++i)
	{
		if (std::isalnum(*i) == 0 && s.find(*i) == string::npos)
			throw irc_error(ERR_ERRONEUSNICKNAME(nick), SEND_ERROR);
	}
	for (std::map< int, Client >::const_iterator it = _clients->begin(); it != _clients->end(); ++it)
	{
		if (&it->second != this && it->second.GetUinfo()[nickname] == nick)
		{
			_uinfo[nickname] = "";
			throw irc_error(ERR_NICKNAMEINUSE(nick), SEND_ERROR);
		}
	}
}

// Non-member Function

void	*GetInAddr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

vec_str	Split(const string &str, const string delimiter)
{
	int		start(0);
	int 	end(str.find(delimiter));
	vec_str	res;

	if (str.empty())
		return (res);
	while (end != -1)
	{
		if (str.substr(start, end - start) != string(""))
			res.push_back(str.substr(start, end - start));
		start = end + delimiter.size();
		end = str.find(delimiter, start);
	}
	if (str.substr(start, end - start) != string(""))
		res.push_back(str.substr(start, end - start));
	return (res);
}
