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
		throw system_error("💀 fatal: socket binding failed");
	freeaddrinfo(res);
	if (listen(_listener, MAX_LISTEN) == -1)
		throw system_error("listen failed");
}

void Server::_AcceptNewConnection()
{
	Client			client(_password);
	int				new_fd;

	new_fd = client.AcceptClient(_listener);
	if (new_fd == -1)
		std::cerr << "⚠️  warning: accept failed" << std::endl;
	else
	{
		struct pollfd	new_pfd;

		//new_pfd.revents = 0;
		new_pfd.fd = new_fd;
		new_pfd.events = POLLIN;

		_pollfds.push_back(new_pfd);
		_clients.insert(std::make_pair(new_fd, client));
		std::cout << "ℹ️  irc server:\033[0;32m connection etablished\033[0;37m from " << client.GetIp() << " on socket " << client.GetFd() << std::endl;
	}
}

void	Server::_CloseConnection(struct pollfd &pfd)
{
	std::cout << "ℹ️  irc server:\033[0;31m connection close \033[0;37mfrom " << _clients[pfd.fd].GetIp() << " on socket " << pfd.fd << std::endl;
	_clients.erase(pfd.fd);
	close(pfd.fd);
	_pollfds.erase(vec_pfd::iterator(&pfd));
}

void	Server::_ReceiveData(struct pollfd &pfd)
{
	//pfd.revents = 0;
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
			Client &client(_clients[pfd.fd]);
			
			if (client.ParseRecv(buf) == -1)
				return ;
		}
	}
}

void Server::InitConnectionLoop()
{
	_pollfds[0].fd = _listener;
	_pollfds[0].events = POLLIN;
	_pollfds[0].revents = 0;
}

void Server::Logs() const
{
	std::cout << "=======================" << std::endl;
	std::cout << "Connection Logs:" << std::endl;
	for (std::map< int, Client >::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::cout << "	clients info:" << std::endl;
		std::cout << "		IP: " << it->second.GetIp() << std::endl;
		std::cout << "		Socket: " << it->second.GetFd() << std::endl;
		if (it->second.GetUinfo()[nickname].empty() == false)
			std::cout << "		Nick: " << it->second.GetUinfo()[nickname] << std::endl;
		if (it->second.GetUinfo()[username].empty() == false)
			std::cout << "		User: " << it->second.GetUinfo()[username] << std::endl;
		if (it->second.GetUinfo()[hostname].empty() == false)
			std::cout << "		Host: " << it->second.GetUinfo()[hostname] << std::endl;
		if (it->second.GetUinfo()[servername].empty() == false)
			std::cout << "		Server: " << it->second.GetUinfo()[servername] << std::endl;
		if (it->second.GetUinfo()[realname].empty() == false)
			std::cout << "		Realname: " << it->second.GetUinfo()[realname] << std::endl;
	}
	std::cout << "=======================" << std::endl;
}

void Server::ConnectionLoop()
{
	std::cout << "port [" << _portNumber << "] password [" << _password << "]" << std::endl;
	while (1)
	{
		if ((_poll_count = poll(_pollfds.data(), _pollfds.size(), -1)) == -1)
			throw system_error("poll failed");
		system("clear");
		std::cout << "------------[ IRC ]------------" << std::endl;
		std::cout << "port [" << _portNumber << "] password [" << _password << "]" << std::endl;
		for (size_t i = 0; i < _pollfds.size(); ++i)
		{
			if (_pollfds[i].revents & POLLIN)
				_ReceiveData(_pollfds[i]);
		}
		Logs();
	}
}
