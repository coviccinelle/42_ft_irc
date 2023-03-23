#include "../include/Client.hpp"

Client::Client() :
	_addrSize(sizeof(_addr)),
	_ip("")
{
	_pfd.events = POLLIN;
	return ;
}

Client::~Client(void)
{
	std::cout << "Client destroyed" << std::endl;
	return ;
}

Client::Client(Client const &src)
{
	_pfd = src._pfd;

	return ;
}

Client &Client::operator=(Client const &rhs)
{
	_pfd = rhs._pfd;
	return (*this);
}

int Client::AcceptClient(int listener)
{
	char ip[INET6_ADDRSTRLEN];
	_pfd.fd = accept(listener, (struct sockaddr *)&_addr, &_addrSize);

	if (_pfd.fd != -1)
		inet_ntop(_addr.ss_family, getInAddr((struct sockaddr *)&_addr), ip, INET6_ADDRSTRLEN);
	std::cout << "here" << ip << std::endl;
	_ip = ip;
	std::cout << "here" << _ip << std::endl;
	return (_pfd.fd);
}

const struct pollfd	&Client::getPfd() const
{
	return (_pfd);
}

const string	&Client::getIp() const
{
	std::cout << "HERE" << _ip << std::endl;
	return (_ip);
}

void	*getInAddr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
