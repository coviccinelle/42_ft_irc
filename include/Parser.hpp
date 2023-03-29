#ifndef PARSER_HPP
#define PARSER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"

#define NUL 0x00
#define SPACE 0x20

enum Token {
	space,
	nosp,
	nospcl,
	letter,
	digit,
	colon,
	at,
	excl_mark,

	special,
	dash,
	dot,
	eoi,
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

		void	Parse(const string &str);
		void	wrap();
	private:
		Token					_GetToken();
		string					_input;
		string::iterator		_it;
		Token					_current;
		std::vector< Token >			_tokens;
};

int	isspecial(int ch);

#endif
