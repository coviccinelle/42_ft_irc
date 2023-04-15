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
		
		void	Debug() const;

		string					message;
		string					prefix;
		string					params;
		string					user;
		string					host;
		string					nickname;
		string					command;
		string					trailing;
		// other params
		std::vector< string >	middle;
};

#endif
