#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP
#include "../utils/utils.hpp"
#include "../include/Parser.hpp"

class CommandParser : public Parser
{
	public:
		CommandParser();
		virtual ~CommandParser();
		CommandParser(CommandParser const &src);
		CommandParser	&operator=(CommandParser const &rhs);
		
		void					ParseCommand(const string &str);
		void					DebugCommand() const;

	private:
		void					Parse(const string &str);
		void					Debug() const;

		void					_Message();
		void					_Command();
		void					_Nickname();
		void					_Middle();
		void					_Host();
		void					_User();
		void					_Prefix();
		void					_Trailing();
		void					_Param();
};

#endif
