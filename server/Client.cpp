#include "../include/Client.hpp"

Client::Client() :
	_fd(0),
	_addrSize(sizeof(_addr)),
	_ip(""),
	_cmds(0)
{
	memset(&_addr, 0, sizeof(_addr));
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
	vec_str	s(Split(str, delimiter));

	for (vec_str::iterator it = s.begin(); it != s.end(); ++it)
		_cmds.push_back(Split(*it));
}

int	Client::ParseRecv(const string &buf)
{
	SplitCmds(buf);

	if (_cmds.empty())
	{
		std::cerr << "⚠️  warning : empty commands" << std::endl;
		return (-1);
	}
	for (size_t i = 0; i < _cmds.size(); ++i)
	{
		vec_str::const_iterator start = _cmds[i].begin();
		vec_str::const_iterator end = _cmds[i].end();
		for (vec_str::const_iterator j = start; j != end; ++j)
		{
			if (*j == string("CAP") &&
				j + 1 != end &&
				*(j + 1) == string("LS") &&
				j + 2 == end)
			{
				std::cout << "CAP LS checked" << std::endl;
				break ;
			}
			else
			{
				std::cout << "CAP LS failed" << std::endl;
				break ;
			}
		}
		_cmds.erase(_cmds.begin() + i);
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
