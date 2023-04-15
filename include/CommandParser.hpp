#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP
#include "../utils/utils.hpp"
#include "../include/Parser.hpp"

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

class CommandParser : public Parser
{
	public:
		CommandParser();
		virtual ~CommandParser();
		CommandParser(CommandParser const &src);
		CommandParser	&operator=(CommandParser const &rhs);
		
		void		ParseCommand(const string &str);
		void		DebugCommand() const;

		cst_vec_str	&GetCinfo() const;
		cst_vec_str	&GetMiddle() const;

	private:
		void		Parse(const string &str);
		void		Debug() const;

		void		SetMessage(const string &s);
		void		SetPrefix(const string &s);
		void		SetParams(const string &s);
		void		SetUser(const string &s);
		void		SetHost(const string &s);
		void		SetNickname(const string &s);
		void		SetCommand(const string &s);
		void		SetTrailing(const string &s);
		void		AddMiddle(const string &s);

		void		_Message();
		void		_Command();
		void		_Nickname();
		void		_Middle();
		void		_Host();
		void		_User();
		void		_Prefix();
		void		_Trailing();
		void		_Param();

		vec_str		_cinfo;
		vec_str		_middle;
};

#endif
