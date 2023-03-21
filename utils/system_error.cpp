#include "../include/system_error.hpp"

system_error::system_error() : _str(std::strerror(errno))
{
	return ;
}

system_error::system_error(const std::string &error) : _str(error + ": " + std::strerror(errno))
{
	return ;
}

system_error::~system_error() throw()
{
	return ;
}

system_error::system_error(system_error const &src)
{
	_str = src._str;

	return ;
}

system_error &system_error::operator=(system_error const &rhs)
{
	_str = rhs._str;

	return (*this);
}

const char* system_error::what() const throw() 
{
	return (_str.c_str());
}
