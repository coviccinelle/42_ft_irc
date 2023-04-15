#include <iostream>
#include "include/Command.hpp"

int	main(void)
{
	Command cmd;

	cmd.ParseCommand(":irc PASS toto :coucou");
	cmd.DebugCommand();

	cmd.ParseTarget("toto,tata,titi");
	cmd.DebugTarget();

	cmd.ParseChannel("#channel");
	cmd.DebugChannel();
	return (0);
}
