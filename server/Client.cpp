#include "../include/Client.hpp"

Client::Client() :
	_fd(0),
	_addr(),
	_addrSize(sizeof(_addr)),
	_ip(""),
	_buf(""),
	_cmds(0),
	_registd(false),
	_uinfo(INF_CLI_SIZE)
{
	memset(&_addr, 0, sizeof(_addr));
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

	_buf = src._buf;
	_cmds = src._cmds;

	_registd = src._registd;
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

	_buf = rhs._buf;
	_cmds = rhs._cmds;

	_registd = rhs._registd;
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

bool	Client::IsRegistd() const
{
	return (_registd);
}

cst_vec_str	&Client::GetUinfo() const
{
	return (_uinfo);
}

string	Client::GetPrefix() const
{
	return (_uinfo[nickname] + "!" + _uinfo[username] + "@" + _uinfo[hostname]);
}

const std::list< Command >	&Client::GetCmds() const
{
	return (_cmds);
}

void	Client::SetUinfo(const vec_str &uinfo)
{
	if (&uinfo == &_uinfo)
		return ;
	_uinfo = uinfo;
}

void	Client::SetRegistd()
{
	_registd = true;
}

void	Client::PopCmd()
{
	_cmds.pop_front();
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
			_parser.Parse(trim(*it));
		}
		catch (irc_error &e)
		{
			std::cout << "⚠️  " <<  e.what() << std::endl;
		}
		_cmds.push_back(_parser.GetCommand());
	}
}

void	Client::ParseRecv(const string &buf)
{
	if (buf == "\n" || buf == "\r\n")
	{	
		std::cerr << "⚠️  warning : \\r\\n only" << std::endl;
		return ;
	}
	_ParseBuf(buf);
	if (_cmds.empty())
		std::cerr << "⚠️  warning : empty commands" << std::endl;

	return ;
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
