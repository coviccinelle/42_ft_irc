#ifndef PARSER_HPP
#define PARSER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"

#define NUL 0x00
#define SPACE 0x20

enum Token {
	space,
	nospcl,
	letter,
	digit,
	colon,
	at,
	excl_mark,
	dot,
	dash,
	eoi,
	special,

	comma,
	percent,
	plus,
	sha,
	amp,
	dollar,
	error
};

class Parser
{
	public:
		Parser(void);
		~Parser(void);
		Parser(Parser const &src);
		Parser	&operator=(Parser const &rhs);

		const std::vector< Token >	&Parse(const string &str);
	private:
		Token					_GetToken();
		void					_Wrapper();
		Token					_current;
		string					_input;
		void					_Message();
		void					_Command();
		void					_Prefix();
		void					_Param();
		string::iterator		_it;
		std::vector< Token >	_tokens;
};

bool	isspecial(string::const_iterator it);

#endif
