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
			std::cerr << "⚠️  warning: server socket creation failed" << std::endl;
			continue;
		}
		if (fcntl(_listener, F_SETFL, O_NONBLOCK) == -1)
			throw system_error("fcntl");
		if (setsockopt(_listener, SOL_SOCKET, SO_REUSEPORT, &sockopt, sizeof(sockopt)) == -1)
			throw system_error("setsockopt");
		if (bind(_listener, res->ai_addr, res->ai_addrlen) == -1)
		{
			close(_listener);
			std::cerr << "⚠️   warning: server bind failed" << std::endl;
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
	Client client;

	if (client.AcceptClient(_listener) == -1)
			std::cerr << "⚠️  warning: accept failed" << std::endl;
	else
	{
		_pollfds.push_back(client.getPfd());
		_clients.insert(std::make_pair(client.getPfd().fd, client));
		std::cout << "ℹ️  irc server: connection from " << client.getIp() << " on socket " << client.getPfd().fd << std::endl;
	}
}

void	Server::_parseRecv(char *buffer)
{
	string buf(buffer);
	std::cout << buf;
	size_t i = -1;
	if (buf.find("CAP LS") != i && buf.find("PASS") != i && buf.find("NICK") != i && buf.find("USER") != i)
	{
		std::cout << "buf = \n[" << buf << "]\n" << "Parsing Recv done ✅ --- Ready to registrer" << std::endl;
		sleep(8);
	}
	else
	{
		std::cout << "not enough data, should wait more, for now : " << buf << std::endl;
		sleep(8);
	}
}

void	Server::_CloseConnection(struct pollfd &pfd)
{
	std::cout << "ℹ️  irc server:\033[0;31m connection close \033[0;37mfrom" << _clients[pfd.fd].getIp() << " on socket " << pfd.fd << std::endl;
	close(pfd.fd);
	_pollfds.erase(std::vector< struct pollfd >::iterator(&pfd));
	_clients.erase(pfd.fd);
}

void	Server::_ReceiveData(struct pollfd &pfd)
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
		if (ret == 0)
			_CloseConnection(pfd);
		else if (ret < 0)
			std::cerr << "⚠️  warning : recv err" << std::endl;
		else
		{
			std::cout << "Hello recv buf = \n" << buf << std::endl;
			_parseRecv(buf);
		}
	}
}

int Server::_sender(int fd, char *buf)
{
	int ret;
	memset(&buf, 0, sizeof(buf));
	std::cout << "Sending buf = " << buf << std::endl;
	ret = send(fd, buf, sizeof buf, 0);
	return (ret);
}


void Server::_SendData(struct pollfd &pfd)
{
	pfd.revents = 0;
	pfd.events = POLLOUT;
	if (pfd.fd != _listener)
	{
		int ret;
		char buf[512] = "Coucou from server\n";
		if ((ret = _sender(pfd.fd, buf)) == -1)
			std::cerr << "⚠️ warning : send err" << std::endl;
	}
	else
		std::cerr << "⚠️  warning: euhhh it's the POLLOUT listener fd " << std::endl;

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
		std::cout << "Connected Logs:" << std::endl;
		for (std::map< int, Client >::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			std::cout << "clients info:" << std::endl;
			std::cout << "	IP: " << it->second.getIp() << std::endl;
			std::cout << "	Socket: " << it->second.getPfd().fd << std::endl;
		}
		if ((_poll_count = poll(_pollfds.data(), _pollfds.size(), -1)) == -1)
			throw system_error("poll failed");
		for (size_t i = 0; i < _pollfds.size(); ++i)
		{
			if (_pollfds[i].revents & POLLIN)
				_ReceiveData(_pollfds[i]);
			else
			{
				std::cout << "ready for pollout" << std::endl;
//				_SendData(_pollfds[i]);
			}
		}
		std::cout << "listening" << std::endl;
	}
}
