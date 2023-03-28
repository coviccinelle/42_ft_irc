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

cst_vec_vec_str	&Client::GetCmds() const
{
	return (_cmds);
}

void Client::SplitCmds(const string &str, const string delimiter)
{
	vec_str	tmp(Split(trim(str), delimiter));

	for (vec_str::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		int		start(0);
		int 	end(it->find(" "));
		vec_str	v;
		if (end != -1)
		{
			v.push_back(trim(it->substr(start, end - start)));
			v.push_back(trim(it->substr(end, it->size())));
		}
		else
			v.push_back(it->substr(start, str.size()));
		_cmds.push_back(v);
	}
}



void	Client::_User(cst_vec_str &cmd)
{
	if (cmd.size() == 1)
	{
		SendData(ERR_NEEDMOREPARAMS(cmd[0]));
		return ;
	}
	//TODO: SendData(ERR_ALREADYREGISTERED);
	vec_str p(Split(cmd[1]));
	if (p.size() < 4 || _uinfo[nickname].empty() || cmd[1].rfind(":") == string::npos)
	{
		std::cout << "Invalid param" << std::endl;
		return ;
	}
	else
	{
		string rn(cmd[1].substr(cmd[1].rfind(":") + 1));
		if (rn.empty())
		{
			std::cout << "empty realname" << std::endl;
			return ;
		}
		_uinfo[username] = p[0];
		_uinfo[hostname] = p[1];
		_uinfo[servername] = p[2];
		_uinfo[realname] = rn;
		SendData(RPL_WELCOME(_uinfo[nickname], _uinfo[username], _uinfo[hostname]));
	}
}

void	Client::_Nick(cst_vec_str &cmd)
{
	if (cmd.size() == 1)
	{
		SendData(ERR_NONICKNAMEGIVEN);
		return ;
	}
	vec_str p(Split(cmd[1]));
	if (p.size() != 1  || _validPass == false)
	{
		std::cout << "Invalid param" << std::endl;
		return ;
	}
	if (ValidNickname(cmd[1]) == 0)
		throw std::invalid_argument("invalid nickname");
	else
	{
		_uinfo[nickname] = p[0];
		std::cout << "NICK has been set to " << _uinfo[nickname] << std::endl;
	}
}

void	Client::_Pass(cst_vec_str &cmd)
{
	if (cmd.size() == 1)
		throw irc_error(ERR_NEEDMOREPARAMS(cmd[0]), CLOSE_CONNECTION);
	if (_validPass)
		throw irc_error(ERR_ALREADYREGISTERED, CLOSE_CONNECTION);
	if (cmd[1] != _servPass)
		throw irc_error("invalid pass", CLOSE_CONNECTION);
	std::cout << "ℹ️  irc server:\033[0;32m valid pass \033[0;37mfrom " << _ip << " on socket " << _fd << std::endl;
	_validPass = true;
}

void	Client::_Ping(cst_vec_str &cmd)
{
	(void)cmd;
	std::cout << "ping command received" << std::endl;
}

void	Client::_CapLs(cst_vec_str &cmd)
{
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

void	Client::ExecCommand(cst_vec_str &cmd)
{
	switch (ResolveOption(cmd[0]))
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

void	Client::ParseRecv(const string &buf)
{
	size_t pos;
	_buf += buf;
	if ((pos = _buf.find_last_of("\n")) == string::npos)
		return ;
	string tmp = _buf.substr(0, (_buf.begin() + pos) - _buf.begin());
	SplitCmds(trim(tmp));
	tmp = _buf.substr(pos, _buf.end() - (_buf.begin() + pos));
	_buf = tmp;

	if (_cmds.empty())
	{
		std::cerr << "⚠️  warning : empty commands" << std::endl;
		return ;
	}

	while (_cmds.empty() == 0)
	{
		ExecCommand(_cmds[0]);
			// printer
		for (vec_str::const_iterator j = _cmds[0].begin(); j != _cmds[0].end(); ++j)
			std::cout <<  "[" << *j << "]";
		std::cout << std::endl;
			//
		_cmds.erase(_cmds.begin());
	}
	return ;
}

void Client::SendData(const string &msg) const
{
	std::cout << msg << std::endl;
	ssize_t ret = send(_fd, msg.data(), msg.size() + 1, 0);
	if (ret == -1)
		std::cerr << "⚠️ warning : send err" << std::endl;
}

int	Client::ValidNickname(const string &nick)
{
	if (nick.size() > 9)
	{
		std::cout << "ERR: Nickname is longer than 9 characters" << std::endl;
		SendData(ERR_ERRONEUSNICKNAME(nick));
		return 0;
	}
	string s("-_[]{}\\`|");
	for (string::const_iterator i = nick.begin(); i != nick.end(); ++i)
	{
		if (std::isalnum(*i) == 0 && s.find(*i) == string::npos)
		{
			SendData(ERR_ERRONEUSNICKNAME(nick));
			return (0);
		}
	}
	for (std::map< int, Client >::const_iterator it = _clients->begin(); it != _clients->end(); ++it)
	{
		if (it->second.GetUinfo()[nickname] == nick)
		{
			SendData(ERR_NICKNAMEINUSE(nick));
			return (0);
		}
	}
	return (1);
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
