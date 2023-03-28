#ifndef PARSER_HPP
#define PARSER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"

#define CR 0x0D
#define LF 0x0A
#define NUL 0x00
#define SPACE 0x20

enum Token {
	colon,
	prefix,
	space,
	command,
	params,
	crlf,
	servername,
	nickname,
	exclamation_mark,
	user,
	at,
	host,
	letter,
	digit,
	middle,
	trailing,
	nospcrlfcl,
	eoi,
	error
};

class Parser
{
	public:
		Parser(void);
		~Parser(void);
		Parser(Parser const &src);
		Parser	&operator=(Parser const &rhs);

		void	Parse(const string &str);
	private:
		Token					_GetToken() const;
		Token					_current;
		string					_input;
		string::const_iterator	_it;
};

#endif
