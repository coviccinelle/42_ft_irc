#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <poll.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <cstring>

#include "../utils/utils.hpp"

class Client
{
	public:
		Client();
		~Client();
		Client(Client const &src);
		Client	&operator=(Client const &rhs);

		const struct pollfd		&getPfd(void) const;
		int						AcceptClient(int listener);
		const string			&getIp() const;

	private:
		struct pollfd			_pfd;
		struct sockaddr_storage	_addr;
		socklen_t				_addrSize;
		string					_ip;
};

void			*getInAddr(struct sockaddr *sa);

#endif
