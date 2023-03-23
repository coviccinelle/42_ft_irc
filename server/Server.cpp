#include "../include/Server.hpp"

Server::Server(const std::string &port, const std::string &pass) :
	_portNumber(port),
	_password(pass),
	_listener(0),
	_pollfds(1),
	_poll_count(0)
{
}


Server::~Server()
{
    close(_listener);
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
		if ((_listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		{
			std::cerr << "warning: server socket creation failed" << std::endl;
			continue;
		}
		if (fcntl(_listener, F_SETFL, O_NONBLOCK) == -1)
			throw system_error("fcntl");
		if (setsockopt(_listener, SOL_SOCKET, SO_REUSEPORT, &sockopt, sizeof(sockopt)) == -1)
			throw system_error("setsockopt");
		if (bind(_listener, res->ai_addr, res->ai_addrlen) == -1)
		{
			close(_listener);
			std::cerr << "warning: server bind failed" << std::endl;
			continue;
		}
		break;
	}
	if (p == NULL)
		throw system_error("fatal: socket binding failed");
	freeaddrinfo(res);
	if (listen(_listener, MAX_LISTEN) == -1)
		throw system_error("listen failed");
}

void Server::_AcceptNewConnection()
{
	socklen_t				addrSize;
	struct pollfd 			new_pfd;
	struct sockaddr_storage	addr;

	addrSize = 0;
	memset(&addr, 0, sizeof(addr));
	memset(&new_pfd, 0, sizeof(new_pfd));
	if ((new_pfd.fd = accept(_listener, (struct sockaddr *)&addr, &addrSize)) == -1)
			std::cerr << "warning: accept failed" << std::endl;
	else
	{
		new_pfd.events = POLLIN;
		_pollfds.push_back(new_pfd);
		std::cout << "connection from "<< "ip (TODO) " << "accepted ! fd: " << new_pfd.fd << std::endl;
	}
}

void Server::_ReceiveData(struct pollfd &pfd)
{
	pfd.revents = 0;
	if (pfd.fd == _listener)
		_AcceptNewConnection();
	else
	{
		int ret;
		char buf[512];
		memset(&buf, 0, sizeof(buf));
		ret = recv(pfd.fd, buf, sizeof buf, 0); 
		std::cout << "Hello recv buf = " << buf << std::endl;
		if (ret == 0)
		{
			std::cout << "Connection closed" << std::endl;
			close(pfd.fd);
			_pollfds.erase(std::vector< struct pollfd >::iterator(&pfd));
		}
		else if (ret < 0)
			std::cerr << "warning : recv err" << std::endl;
	}
}

void Server::InitConnectionLoop()
{
	_pollfds[0].fd = _listener;
	_pollfds[0].events = POLLIN;
}

void Server::ConnectionLoop()
{
	while (1)
	{
		if ((_poll_count = poll(_pollfds.data(), _pollfds.size(), -1)) == -1)
			throw system_error("poll failed");
		for (size_t i = 0; i < _pollfds.size(); ++i)
		{
			if (_pollfds[i].revents & POLLIN || _pollfds[i].revents & POLLRDHUP)
				_ReceiveData(_pollfds[i]);
			else
				std::cout << "ready for pollout" << std::endl;
		}
		std::cout << "listening" << std::endl;
	}
}
