#include "../include/Client.hpp"

Client::Client() :
	_addrSize(sizeof(_addr)),
	_ip("")
{
	memset(&_pfd, 0, sizeof(_pfd));
	memset(&_addr, 0, sizeof(_addr));
	_pfd.events = POLLIN;
	return ;
}

Client::~Client(void)
{
	return ;
}

Client::Client(Client const &src)
{
	memcpy(&_pfd, &src._pfd, sizeof(src._pfd));
	memcpy(&_addr, &src._addr, sizeof(src._addr));
	memcpy(&_addrSize, &src._addrSize, sizeof(src._addrSize));
	_ip = src._ip;
	return ;
}

Client &Client::operator=(Client const &rhs)
{
	if (this == &rhs)
		return (*this);
	memcpy(&_pfd, &rhs._pfd, sizeof(rhs._pfd));
	memcpy(&_addr, &rhs._addr, sizeof(rhs._addr));
	memcpy(&_addrSize, &rhs._addrSize, sizeof(rhs._addrSize));
	_ip = rhs._ip;

	return (*this);
}

int Client::AcceptClient(int listener)
{
	char ip[INET6_ADDRSTRLEN];
	_pfd.fd = accept(listener, (struct sockaddr *)&_addr, &_addrSize);
	if (_pfd.fd != -1)
	{
		inet_ntop(_addr.ss_family, getInAddr((struct sockaddr *)&_addr), ip, INET6_ADDRSTRLEN);
		_ip = ip;
	}
	else
		std::cerr << "⚠️  warning : accept failed" << std::endl;

	return (_pfd.fd);
}

const struct pollfd	&Client::getPfd() const
{
	return (_pfd);
}

const string	&Client::getIp() const
{
	return (_ip);
}

void	*getInAddr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
