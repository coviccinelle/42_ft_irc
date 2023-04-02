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
		string					params;
		string					user;
		string					host;
		string					nickname;
		string					command;
		string					trailing;
		// First param after command is evaluated as the target;
		// It is a vec of string splitted on commas ','
		// Ex : "target1,target2" -> ["target1", "target2"]
		std::vector< string >	target;
		// other params
		std::vector< string >	middle;
};

#endif
