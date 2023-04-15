#include <iostream>
#include "include/CommandParser.hpp"

int	main(void)
{
	CommandParser cp;
	cp.ParseCommand(":irc PASS toto :coucou");
	cp.DebugCommand();
	return (0);
}
