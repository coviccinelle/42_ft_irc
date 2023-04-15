#ifndef PARSER_HPP
#define PARSER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"

#define NUL 0x00
#define SPACE 0x20

enum Token {
	space = 0,
	nospcl,
	letter,
	digit,
	colon,
	at,
	excl_mark,
	dot,
	dash,
	special,
	comma,
	sha,
	percent,
	amp,
	plus,
	eoi,
	dollar,
	error
};

// Base Parser class
class Parser
{
	public:
		Parser(void);
		virtual ~Parser(void);
		Parser(Parser const &src);
		Parser	&operator=(Parser const &rhs);

		// ParseCommand
		virtual void				Parse(const string &str) = 0;
		virtual void				Debug() const = 0;

		bool	isValidNick(const string &str);
		bool	isValidUserMode(const string &str);

	protected:
		// Call the next token; Use Wrapper instead
		Token					_GetToken();
		// Wrap GetToken function
		void					_Wrapper();

		// Tokenizer utils
		string::iterator		_it;
		Token					_current;
		std::vector< Token >	_tokens;
		string					_input;
};

bool	isspecial(string::const_iterator it);

#endif
