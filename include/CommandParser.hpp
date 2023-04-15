#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP
#include "../utils/utils.hpp"
#include "../include/Parser.hpp"
#include "../include/Command.hpp"

class CommandParser : public Parser
{
	public:
		CommandParser(void);
		~CommandParser(void);
		CommandParser(CommandParser const &src);
		CommandParser	&operator=(CommandParser const &rhs);
		
		void	Parse(const string &str);
		void	Debug() const;

	private:
		void					_Message();
		void					_Command();
		void					_Nickname();
		void					_Middle();
		void					_Host();
		void					_User();
		void					_Prefix();
		void					_Trailing();
		void					_Param();

		Command					_cmd;
};

#endif
