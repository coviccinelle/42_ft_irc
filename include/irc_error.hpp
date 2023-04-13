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
	CLOSE_CONNECTION,
	NO_SEND,
	SEND_ERROR,
	ERR_HOST,
	ERR_NICK,
	ERR_USER,
	ERR_PREFIX,
	ERR_COMMAND,
	ERR_MIDDLE,
	ERR_PARAM,
	ERR_MESSAGE,
	ERR_NOTOPE,
	ERR_CHANNEL,
	ERR_CHANNELPREFIX,
	ERR_CHANNELID,
	ERR_CHANNELSUFFIX
};

class irc_error: public std::exception
{
	public:
		irc_error();
		~irc_error() throw();
		irc_error &operator=(const irc_error &rhs);
		irc_error(const irc_error &src);

		irc_error(const string &error, int code = SUCCESS);

		virtual const char* what() const throw();
		int					code() const throw();
	private:
		string	_str;
		int		_code;
};

#endif
