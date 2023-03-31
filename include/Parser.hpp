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

struct Command {
	string					message;
	string					prefix;
	string					user;
	string					host;
	string					nickname;
	string					command;
	std::vector< string >	middle;
	std::vector< string >	target;
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
		void					_Message();
		void					_Command();
		void					_Nickname();
		void					_Prefix();
		void					_User();
		void					_Host();
		void					_Param();
		void					_Middle();
		void					_Target();
		void					_ParseInit();
		void					_MsgTo();

		string::iterator		_it;
		Token					_current;
		std::vector< Token >	_tokens;
		string					_input;
		Command					_cmd;
};

bool	isspecial(string::const_iterator it);

#endif
