#ifndef IRC_ERROR_HPP
#define IRC_ERROR_HPP
#include <exception>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <string>

typedef std::string string;

enum Errors {
	SUCCESS = 0,
	INVALID_PASS,
	INVALID_NICK,
};

class irc_error: public std::exception
{
	public:
		irc_error();
		~irc_error() throw();
		irc_error &operator=(const irc_error &rhs);
		irc_error(const irc_error &src);

		irc_error(const string &error, int code = 0);

		virtual const char* what() const throw();
		virtual int			code() const throw();
	private:
		string	_str;
		int		_code;
};

#endif
