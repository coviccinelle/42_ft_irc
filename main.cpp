#include  "include/Server.hpp"
#include "utils/utils.hpp"

const string& parsePort(const string &port)
{
	for (string::const_iterator it = port.begin(); it != port.end(); ++it)
		if (isdigit(*it) == 0)
			throw std::invalid_argument("Invalid Port Number: only positive digits accepted");

	if (std::atoi(port.c_str()) > 65535)
		throw std::invalid_argument("Invalid Port Number : portNumber must be between 1 and 65535");

	return (port);
}

const string&	parseArgs(int ac, char **av)
{
	if (ac != 3)
		throw std::invalid_argument("Invalid Args: How to use: ./ircserv portNumber password.");
	return (parsePort(string(av[1])));
}

int main(int ac, char **av)
{
	try
	{
		std::cout << "Step 1: Server is starting : Parsing arguments" << std::endl;
		string portNumber(parseArgs(ac, av));
		Server server(portNumber, string(av[2]));

		std::cout << "Step 2: Server is setting up : Awaitting Connection Loop ..." << std::endl;
		server.AwaitingConnectionQueue();
		server.ConnectionLoop();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	/*
	try
	{
		std::cout << "Step 3: Server is setting up : FdMultithreading ..." << std::endl;
		server.FdMultithreading();
	}
	catch (std::exception &e)
	{
		std::cout << "FdMultithreading Error: " << std::endl;
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	*/
	std::cout << "Server is closed. Byeee 👋👋👋" << std::endl;
	return (0);
}
