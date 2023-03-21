#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <unistd.h>

#include "../utils/utils.hpp"

class Client
{
	public:
		Client(int fd, const struct sockaddr_storage &addr, const socklen_t &size);
		~Client(void);
		Client(Client const &src);
		Client	&operator=(Client const &rhs);
		int	getSockfd(void) const;
	private:
		Client(void);
		int			 			_sockfd;
		struct sockaddr_storage	_addr;
		socklen_t				_addrSize;
//		std::string 	_nick;

};

#endif
