#include <iostream>
#include "include/Command.hpp"

int	main(void)
{
	Command parser;

	parser.ParseCommand(":irc PASS toto :coucou");
	parser.DebugCommand();

	parser.ParseTarget("toto,tata,titi");
	parser.DebugTarget();

//	parser.ParseChannel("#channel");
//	parser.DebugChannel();
	return (0);
}
