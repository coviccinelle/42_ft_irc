#include "../include/Command.hpp"

Command::Command(void) :
	message(""),
	prefix(""),
	user(""),
	host(""),
	nickname(""),
	command("")
{
	return ;
}

Command::~Command(void)
{
	return ;
}

Command::Command(Command const &src)
{
	message = src.message;
	prefix = src.prefix;
	user = src.user;
	host = src.host;
	nickname = src.nickname;
	command = src.command;
	middle = src.middle;
	target = src.target;

	return ;
}

Command &Command::operator=(Command const &rhs)
{
	std::cout << "Command copy assignment operator called" << std::endl;
	if (&rhs == this)
		return (*this);

	message = rhs.message;
	prefix = rhs.prefix;
	user = rhs.user;
	host = rhs.host;
	nickname = rhs.nickname;
	command = rhs.command;
	middle = rhs.middle;
	target = rhs.target;

	return (*this);
}

void Command::Debug()
{
	std::cout << "===========[ DEBUG ]===========" << std::endl;
	std::cout << "Message :[" << message << "]" << std::endl;
	std::cout << "Prefix :[" << prefix << "]" << std::endl;
	std::cout << "User :[" << user << "]" << std::endl;
	std::cout << "Host :[" << host << "]" << std::endl;
	std::cout << "Nickname :[" << nickname << "]" << std::endl;
	std::cout << "Command :[" << command << "]" << std::endl;
	std::cout << "Middle : " << std::endl;
	for (std::vector<string>::iterator it = middle.begin(); it != middle.end(); ++it)
		std::cout << " :[" << *it << "]" << std::endl;

	std::cout << "Target : " << std::endl;
	for (std::vector<string>::iterator it = target.begin(); it != target.end(); ++it)
		std::cout << " :[" << *it << "]" << std::endl;
	std::cout << "Trailing : " << trailing << std::endl;
	std::cout << "===============================" << std::endl;
}
