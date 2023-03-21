#include "../include/Server.hpp"

Server::Server(const std::string &port, const std::string &pass) :
	_portNumber(port),
	_password(pass),
	_nick(""),
	_sockfd(0),
	_connectedClients(new std::vector< Client >)
{
}


Server::~Server()
{
    close(_sockfd);
	delete _connectedClients;
}

/*
 *
 * man socket(2), setsockopt(2), bind(2), listen(2)
 *
 * Only waiting for ipv4 connection now
 *
*/

void	Server::AwaitingConnectionQueue()
{
	struct addrinfo hints;
	struct addrinfo *res;
	int	sockopt = 1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, _portNumber.c_str(), &hints, &res);

	if ((_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		throw system_error("socket");
	if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) == -1)
		throw system_error("fcntl");
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &sockopt, sizeof(sockopt)) == -1)
		throw system_error("setsockopt");
	if (bind(_sockfd, res->ai_addr, res->ai_addrlen) == -1)
		throw system_error("bind");
	freeaddrinfo(res);
	if (listen(_sockfd, MAX_LISTEN) == -1)
		throw system_error("listen");
}

void Server::AcceptClientConnection()
{
	int 					new_fd;
	struct sockaddr_storage	addr;
	socklen_t				addrSize;

	new_fd = 0;
	addrSize = 0;
	memset(&addr, 0, sizeof(addr));
	while (1)
	{
		new_fd = accept(_sockfd, (struct sockaddr *)&addr, &addrSize);
		if (new_fd != -1)
		{
			std::cout << "New client accepted" << std::endl;
			if (!fork())
			{
				close(_sockfd);
				while (1)
				{
					if (send(new_fd, "001 thi-phng :Welcome to the Internet Relay Network!\r\n", 55, 0) == -1)
						std::cout << "message not sent" << std::endl;
					std::cout << "I'm sending" << std::endl;
					sleep(1);
				}
				std::cout << "Hugh you don't love me :(" << std::endl;
				close(new_fd);
				exit(0);
			}
			break; 
		}
	//	close(new_fd);
	}
}
