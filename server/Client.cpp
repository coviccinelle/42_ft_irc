#include "../include/Client.hpp"

Client::Client() :
	_fd(0),
	_addr(),
	_addrSize(sizeof(_addr)),
	_ip(""),
	_buf(""),
	_cmds(0),
	_registd(false),
	_uinfo(INF_CLI_SIZE),
	_mode()
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
	_mode = src._mode;

	_parser = src._parser;

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
	_mode = rhs._mode;

	_parser = rhs._parser;

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

const std::bitset< MODE_SIZE >	&Client::GetMode() const
{
	return (_mode);
}

string	Client::GetStrMode() const
{
	string res;

	for (int i = 0; i < MODE_SIZE; ++i)
	{
		if (_mode[i])
			res += USER_MODE[i];
	}
	return (res);
}

void	Client::SetMode(const string &mode)
{
	bool state = false;

	for (string::const_iterator it = mode.begin(); it != mode.end(); ++it)
	{
		if (*it == '+')
			state = true;
		else if (*it == '-')
			state = false;
		else
		{
			if (*it == 'a' && isOperator() == false)
				throw irc_error("NOTICE " + _uinfo[nickname] + " :The flag 'a' SHALL NOT be toggled by the user using the MODE command, instead use of the AWAY command is REQUIRED.\r\n", ERR_NOTOPE);
			if (state == true && (*it == 'o' || *it == 'O'))
				throw irc_error("NOTICE " + _uinfo[nickname] + " :Please be more creative :) You will not grant yourself Operator's privilege that easily.\r\n", ERR_NOTOPE);
			if (state == false && *it == 'r')
				throw irc_error("NOTICE " + _uinfo[nickname] + " :You are in restricted mode.\r\n", ERR_NOTOPE);
			if (state == true)
				_mode.set(USER_MODE.find(*it), true);
			else
				_mode.set(USER_MODE.find(*it), false);
		}
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
	_ParseBuf(buf);
	if (_cmds.empty())
		std::cerr << "⚠️  warning : empty commands" << std::endl;

	return ;
}

bool	Client::isInvisible() const
{
	return (_mode[USER_MODE.find('i')]);
}

bool	Client::isWallops() const
{
	return (_mode[USER_MODE.find('w')]);
}

bool	Client::isOperator() const
{
	return (_mode[USER_MODE.find('o')]);
}

bool	Client::isServNotice() const
{
	return (_mode[USER_MODE.find('s')]);
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
