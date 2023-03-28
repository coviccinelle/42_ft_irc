#include "../include/irc_error.hpp"

irc_error::irc_error() : _str(std::strerror(errno)), _code(errno)
{
	return ;
}

irc_error::irc_error(const std::string &error, int code) : _str(error), _code(code)
{
	return ;
}

irc_error::~irc_error() throw()
{
	return ;
}

irc_error::irc_error(const irc_error &src)
{
	_str = src._str;
	_code = src._code;

	return ;
}

irc_error &irc_error::operator=(const irc_error &rhs)
{
	if (this == &rhs)
		return (*this);
	_str = rhs._str;
	_code = rhs._code;

	return (*this);
}

const char* irc_error::what() const throw() 
{
	return (_str.c_str());
}

int	irc_error::code() const throw() 
{
	return (_code);
}
