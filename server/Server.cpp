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
		_pollfds.push_back(client.GetPfd());
		_clients.insert(std::make_pair(client.GetPfd().fd, client));
		std::cout << "ℹ️  irc server: connection from " << client.GetIp() << " on socket " << client.GetPfd().fd << std::endl;
	}
}

void	Server::_ParseRecv(char *buffer, const struct pollfd &pfd)
{
	_clients[pfd.fd].SplitCmds(string(buffer));
	for (std::vector<string>::const_iterator it = _clients[pfd.fd].GetCmds().begin(); it != _clients[pfd.fd].GetCmds().end(); ++it)
		std::cout << "cmd : " << *it << std::endl;
	std::cout << "Parsing Recv done ✅ --- Ready to registrer" << std::endl;
}

void	Server::_CloseConnection(struct pollfd &pfd)
{
	std::cout << "ℹ️  irc server:\033[0;31m connection close \033[0;37mfrom " << _clients[pfd.fd].GetIp() << " on socket " << pfd.fd << std::endl;
	_clients.erase(pfd.fd);
	close(pfd.fd);
	_pollfds.erase(std::vector< struct pollfd >::iterator(&pfd));
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
			_ParseRecv(buf, pfd);
	}
}

int Server::_Sender(int fd, char *buf)
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
		if ((ret = _Sender(pfd.fd, buf)) == -1)
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

void Server::Logs() const
{
	std::cout << "=======================" << std::endl;
	std::cout << "Connection Logs:" << std::endl;
	for (std::map< int, Client >::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::cout << "	clients info:" << std::endl;
		std::cout << "		IP: " << it->second.GetIp() << std::endl;
		std::cout << "		Socket: " << it->second.GetPfd().fd << std::endl;
	}
	std::cout << "=======================" << std::endl;
}

void Server::ConnectionLoop()
{
	while (1)
	{
		if ((_poll_count = poll(_pollfds.data(), _pollfds.size(), -1)) == -1)
			throw system_error("poll failed");
		system("clear");
		std::cout << "----------[ IRC ]---------" << std::endl;
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
		Logs();
	}
}
