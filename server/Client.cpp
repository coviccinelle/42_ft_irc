#include "../include/Client.hpp"

Client::Client(int fd, const struct sockaddr_storage &addr, const socklen_t &size) :
	_sockfd(fd),
	_addr(addr),
	_addrSize(size)
{
	std::cout << "construct" << std::endl;
	return ;
}

Client::~Client(void)
{
	std::cout << "Client destroyed" << std::endl;
	close(_sockfd);
	return ;
}

Client::Client(Client const &src)
{
	_sockfd = src._sockfd;

	return ;
}

Client &Client::operator=(Client const &rhs)
{
	_sockfd = rhs._sockfd;
	return (*this);
}

int	Client::getSockfd(void) const
{
	return (_sockfd);
}
