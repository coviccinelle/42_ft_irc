#include "../include/Client.hpp"

Client::Client(const string &pass) :
	_fd(0),
	_addr(),
	_addrSize(sizeof(_addr)),
	_ip(""),
	_servPass(pass),
	_cmds(0),
	_mapCmd(),
	_validPass(false),
	_uinfo(INF_CLI_SIZE)
{
	memset(&_addr, 0, sizeof(_addr));
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

	_cmds = src._cmds;
	_mapCmd = src._mapCmd;

	_validPass = src._validPass;
	_uinfo = src._uinfo;
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

	_cmds = rhs._cmds;
	_mapCmd = rhs._mapCmd;

	_validPass = rhs._validPass;
	_uinfo = rhs._uinfo;

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
		std::cout << "no param" << std::endl;
		return ;
	}
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
		std::cout << "username : " <<_uinfo[username] << std::endl;
		std::cout << "hostname : " << _uinfo[hostname] << std::endl;
		std::cout << "servername : " << _uinfo[servername] << std::endl;
		std::cout << "realname : " << _uinfo[realname] << std::endl;
		SendData(RPL_WELCOME(_uinfo[nickname], _uinfo[username], _uinfo[hostname]));
	}
}

void	Client::_Nick(cst_vec_str &cmd)
{
	if (cmd.size() == 1)
	{
		std::cout << "no param" << std::endl;
		return ;
	}
	vec_str p(Split(cmd[1]));
	if (p.size() != 1  || _validPass == false)
	{
		//TODO: WARNINGGGGG : Check if NICK is unique
		// Idea : Pass a pointer to a vector of 
		// clients from the server to each Client
		std::cout << "Invalid param" << std::endl;
		return ;
	}
	else
	{
		_uinfo[nickname] = p[0];
		std::cout << "NICK has been set to " << _uinfo[nickname] << std::endl;
	}
}

void	Client::_Pass(cst_vec_str &cmd)
{
	if (cmd.size() == 1)
	{
		SendData(ERR_NEEDMOREPARAMS(cmd[0]));
		return ;
	}
	if (_validPass)
	{
		SendData(ERR_ALREADYREGISTERED);
		return ;
	}
	vec_str p(Split(cmd[1]));
	if (p[0] == _servPass)
	{
		std::cout << "ℹ️  irc server:\033[0;32m valid pass \033[0;37mfrom " << _ip << " on socket " << _fd << std::endl;
		_validPass = true;
	}
	else
		std::cout << "ℹ️  irc server:\033[0;31m invalid pass \033[0;37mfrom " << _ip << " on socket " << _fd << std::endl;
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

int	Client::ParseRecv(const string &buf)
{
	SplitCmds(buf);

	if (_cmds.empty())
	{
		std::cerr << "⚠️  warning : empty commands" << std::endl;
		return (-1);
	}

	while (_cmds.empty() == 0)
	{
		ExecCommand(_cmds[0]);

//		for (vec_str::const_iterator j = start; j != end; ++j)
//		{
//			std::cout <<  "[" << *j << "]";
//		}
//		std::cout << std::endl;
		_cmds.erase(_cmds.begin());
	}
	return (0);
}

void Client::SendData(const string &msg) const
{
	std::cout << msg << std::endl;
	ssize_t ret = send(_fd, msg.data(), msg.size() + 1, 0);
	if (ret == -1)
		std::cerr << "⚠️ warning : send err" << std::endl;
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
