#ifndef PARSER_HPP
#define PARSER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Command.hpp"
#include "../include/ChannelParse.hpp"

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

class Parser
{
	public:
		Parser(void);
		~Parser(void);
		Parser(Parser const &src);
		Parser	&operator=(Parser const &rhs);

		void						Parse(const string &str);
		bool						isValidNick(const string &str);
		const Command				&GetCommand() const;
		const ChannelParse 			&GetChan() const;
		const std::vector< Token >	&Tokens() const;
		bool						ParseUserMode(const string &str);
		void						ParseJoin(const string &str);

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
		void					_Trailing();
		void					_Target();
		void					_InitCmd();
		void					_InitChan();
		void					_MsgTo();

		void					_Channel();
		void					_ChannelPrefix();
		void					_ChannelId();
		void					_ChannelSuffix();

		string::iterator		_it;
		Token					_current;
		std::vector< Token >	_tokens;
		string					_input;
		Command					*_cmd;
		ChannelParse			*_chan;
};

bool	isspecial(string::const_iterator it);

#endif
