#ifndef PARSER_HPP
#define PARSER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Command.hpp"
#include "../include/ChannelParse.hpp"
#include "../include/TargetParse.hpp"

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

		// Call Tokenizer
		const std::vector< Token >	&Tokens() const;
		// ParseCommand
		void						Parse(const string &str);
		// Check Nickname
		bool						isValidNick(const string &str);
		bool						isValidUserMode(const string &str);
		void						ParseChannel(const string &str);

		const Command				&GetCommand() const;
		const ChannelParse 			&GetChan() const;
		const TargetParse			&GetTarget() const;

	private:
		// Tokenizer
		Token					_GetToken();

		// General command parser
		void					_InitCmd();
		void					_Command();

		void					_Wrapper();
		void					_Message();
		void					_Nickname();
		void					_Prefix();
		void					_User();
		void					_Host();
		void					_Param();
		void					_Middle();
		void					_Trailing();
		void					_MsgTo();

		// Target Parse
		void					_InitTarget();
		void					_Target();

		// Channel Parse
		void					_InitChan();
		void					_Channel();

		void					_ChannelPrefix();
		void					_ChannelId();
		void					_ChannelSuffix();
		void					_ChannelString();

		// Private Member
		string::iterator		_it;
		Token					_current;
		std::vector< Token >	_tokens;
		string					_input;
		Command					*_cmd;
		ChannelParse			*_chan;
		TargetParse				*_target;
};

bool	isspecial(string::const_iterator it);

#endif
