#include "Command.hpp"

Command::Command(void) :
	cinfo(INF_CMD_SIZE),
	middle()
{
	return ;
}

Command::~Command(void)
{
	return ;
}

Command::Command(Command const &src)
{
	_cinfo = src._cinfo;
	_middle = src._middle;

	return ;
}

Command &Command::operator=(Command const &rhs)
{
	if (this == &rhs)
		return (*this);
	_cinfo = rhs._cinfo;
	_middle = rhs._middle;
	return (*this);
}

cst_vec_str	&GetCinfo() const
{
	return (_cinfo);
}

cst_vec_str	&GetMiddle() const
{
	return (_middle);
}

void	Command::SetMessage(const string &s)
{
	_cmd[message] = s;
}

void	Command::SetPrefix(const string &s)
{
	_cmd[prefix] = s;
}

void	Command::SetParams(const string &s)
{
	_cmd[params] = s;
}

void	Command::SetUser(const string &s)
{
	_cmd[user] = s;
}

void	Command::SetHost(const string &s)
{
	_cmd[host] = s;
}

void	Command::SetNickname(const string &s)
{
	_cmd[nick] = s;
}

void	Command::SetCommand(const string &s)
{
	_cmd[command] = s;
}

void	Command::SetTrailing(const string &s)
{
	_cmd[trailing] = s;
}

void	Command::AddMiddle(const string &s)
{
	_middle.push_back(s);
}
