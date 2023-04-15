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

#define INF_CMD_SIZE 8
enum InfoCommand {
	message = 0,
	prefix,
	params,
	user,
	host,
	nick,
	command,
	trailing
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
		// Check Nickname
		bool						isValidNick(const string &str);
		bool						isValidUserMode(const string &str);

		// Utils
		cst_vec_str	&GetCinfo() const;
		cst_vec_str	&GetMiddle() const;
		cst_vec_str	&GetTargets() const;

	protected:
		// Tokenizer
		Token					_GetToken();
		// Get next Token
		void					_Wrapper();

		void	SetMessage(const string &s);
		void	SetPrefix(const string &s);
		void	SetParams(const string &s);
		void	SetUser(const string &s);
		void	SetHost(const string &s);
		void	SetNickname(const string &s);
		void	SetCommand(const string &s);
		void	SetTrailing(const string &s);
		void	AddMiddle(const string &s);
		void	AddTarget(const string &s);

		// Tokenizer utils
		string::iterator		_it;
		Token					_current;
		std::vector< Token >	_tokens;
		string					_input;

		// Data for utils
		vec_str					_cinfo;
		vec_str					_middle;
		vec_str					_targets;
};

bool	isspecial(string::const_iterator it);

#endif
