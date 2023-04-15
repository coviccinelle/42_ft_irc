#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <iostream>
#include "../utils/utils.hpp"
#include "../include/CommandParser.hpp"

#define INF_CMD_SIZE 8

class Command : public CommandParser
{
	public:
		Command(void);
		~Command(void);
		Command(Command const &src);
		Command	&operator=(Command const &rhs);

};

#endif
