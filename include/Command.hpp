#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <iostream>
#include "../utils/utils.hpp"
#include "../include/CommandParser.hpp"
#include "../include/TargetParser.hpp"
//#include "../include/ChannelParser.hpp"

class Command : public CommandParser, public TargetParser
{
	public:
		Command(void);
		~Command(void);
		Command(Command const &src);
		Command	&operator=(Command const &rhs);

		// Utils
		cst_vec_str	&GetCinfo() const;
		cst_vec_str	&GetMiddle() const;
		cst_vec_str	&GetTargets() const;
};

#endif
