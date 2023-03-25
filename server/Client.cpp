#include "../include/Client.hpp"

Client::Client(const string &pass) :
	_fd(0),
	_addrSize(sizeof(_addr)),
	_ip(""),
	_cmds(0),
	_validPass(false),
	_servPass(pass)
{
	memset(&_addr, 0, sizeof(_addr));
	_mapCmd.insert(std::make_pair(string("PASS"), PASS));
	return ;
}

Client::~Client(void)
{
	return ;
}

Client::Client(Client const &src)
{
	memcpy(&_addr, &src._addr, sizeof(src._addr));
	memcpy(&_addrSize, &src._addrSize, sizeof(src._addrSize));
	_fd = src._fd;
	_ip = src._ip;
	_cmds = src._cmds;
	_validPass = src._validPass;
	_servPass = src._servPass;
	_mapCmd = src._mapCmd;
	return ;
}

Client &Client::operator=(Client const &rhs)
{
	if (this == &rhs)
		return (*this);
	memcpy(&_addr, &rhs._addr, sizeof(rhs._addr));
	memcpy(&_addrSize, &rhs._addrSize, sizeof(rhs._addrSize));
	_fd = rhs._fd;
	_ip = rhs._ip;
	_cmds = rhs._cmds;
	_validPass = rhs._validPass;
	_servPass = rhs._servPass;
	_mapCmd = rhs._mapCmd;

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

void	Client::_Path(const string &param)
{
	vec_str p(Split(param));
	if (p.size() != 1)
	{
		//handle invalid nb of args here
		return ;
	}
	std::cout << p[0] << std::endl;
	if (p[0] == _servPass)
		_validPass = true;
}

CmdVal	Client::ResolveOption(const string &input)
{
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
			_Path(cmd[1]);
			break ;
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
