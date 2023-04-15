#include "../include/Command.hpp"

Command::Command(void) :
	CommandParser(),
	TargetParser(),
	ChannelParser()
{
	return ;
}

Command::~Command(void)
{
	return ;
}

Command::Command(Command const &src) :
	CommandParser(src),
	TargetParser(src),
	ChannelParser(src)
{
	return ;
}

Command &Command::operator=(Command const &rhs)
{
	if (this == &rhs)
		return (*this);
	return (*this);
}
