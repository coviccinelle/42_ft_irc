#include "../include/Server.hpp"

Server::Server(const std::string &port, const std::string &pass) :
	_portNumber(port),
	_password(pass),
	_sockfd(0),
	_pollfds(1),
	_poll_count(0)
{
}


Server::~Server()
{
    close(_sockfd);
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
	struct addrinfo *p;
	int	sockopt = 1;
	int ret_value;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((ret_value = getaddrinfo(NULL, _portNumber.c_str(), &hints, &res)) != 0)
		throw system_error("getaddrinfo error");

	for (p = res; p != NULL; p = p->ai_next)
	{
		if ((_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		{
			std::cerr << "warning: server socket creation failed" << std::endl;
			continue;
		}
		if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) == -1)
			throw system_error("fcntl");
		if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &sockopt, sizeof(sockopt)) == -1)
			throw system_error("setsockopt");
		if (bind(_sockfd, res->ai_addr, res->ai_addrlen) == -1)
		{
			close(_sockfd);
			std::cerr << "warning: server bind failed" << std::endl;
			continue;
		}
		break;
	}
	if (p == NULL)
		throw system_error("fatal: socket binding failed");
	freeaddrinfo(res);
	if (listen(_sockfd, MAX_LISTEN) == -1)
		throw system_error("listen failed");
}

void Server::ConnectionLoop()
{
	int 					new_fd;
	struct sockaddr_storage	addr;
	socklen_t				addrSize;

	_pollfds[0].fd = _sockfd;
	_pollfds[0].events = POLLIN;

	new_fd = 0;
	addrSize = 0;
	memset(&addr, 0, sizeof(addr));
	while (1)
	{
		if ((_poll_count = poll(_pollfds.data(), _pollfds.size(), -1)) == -1)
			throw system_error("poll failed");
		for (std::vector< struct pollfd >::iterator pfd = _pollfds.begin(); pfd != _pollfds.end(); ++pfd)
		{
			if (pfd->revents & POLLIN)
			{
				if (pfd->fd == _sockfd)
				{
					if ((new_fd = accept(_sockfd, (struct sockaddr *)&addr, &addrSize)) == -1)
						throw system_error("accept failed");
					std::cout << "connection accepted ! fd: " << new_fd << std::endl;
				}
			}
		}
		std::cout << "listening" << std::endl;
		sleep(1);
	}
}
