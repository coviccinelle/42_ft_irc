#include "../include/Client.hpp"

Client::Client() :
	_addrSize(sizeof(_addr))
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

int Client::acceptClient(int listener)
{
	return (_pfd.fd = accept(listener, (struct sockaddr *)&_addr, &_addrSize));
}

struct pollfd	Client::getPfd(void) const
{
	return (_pfd);
}

void	*getInAddr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


const string Client::getStringIpAddress() const
{
	char ip[INET6_ADDRSTRLEN];

	memset(&ip, 0, INET6_ADDRSTRLEN);
	inet_ntop(_addr.ss_family, getInAddr((struct sockaddr *)&_addr), ip, INET6_ADDRSTRLEN);
	return (string(ip));
}
