#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "../utils/utils.hpp"

class Command
{
	public:
		Command(void);
		~Command(void);
		Command(Command const &src);
		Command	&operator=(Command const &rhs);
		
		void	Debug();

		string					message;
		string					prefix;
		string					user;
		string					host;
		string					nickname;
		string					command;
		std::vector< string >	middle;
		std::vector< string >	target;
};

#endif
