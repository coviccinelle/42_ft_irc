#include "Command.hpp"

Command::Command(void)
{
	return ;
}

Command::~Command(void)
{
	return ;
}

Command::Command(Command const &src)
{

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
