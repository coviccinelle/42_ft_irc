#include  "include/Server.hpp"
#include "include/Channel.hpp"

const string& parsePort(const string &port)
{
	for (string::const_iterator it = port.begin(); it != port.end(); ++it)
		if (isdigit(*it) == 0)
			throw std::invalid_argument("Invalid Port Number: only positive digits accepted");

	int	res = std::atoi(port.c_str());

	// The process need to run as root if port are in range 1-1023
	if (res > 65535 || res < 1024) 
		throw std::invalid_argument("Invalid Port Number : portNumber must be between 1024 and 65535");

	return (port);
}

const string	parseArgs(int ac, char **av)
{
	if (ac != 3)
		throw std::invalid_argument("Invalid Args: How to use: ./ircserv portNumber password.");
	return (parsePort(string(av[1])));
}

int main(int ac, char **av)
{
	std::system("clear");
	std::cout << "------------[ IRC ]------------" << std::endl;
	try
	{
		string portNumber(parseArgs(ac, av));
		Server server(portNumber, string(av[2]));
		server.AwaitingConnectionQueue();
		server.InitConnectionLoop();
		server.ConnectionLoop();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	std::cout << "Server is closed. Byeee 👋👋👋" << std::endl;
	return (0);
}
