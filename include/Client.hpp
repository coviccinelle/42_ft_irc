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

#include <vector>

#include "../utils/utils.hpp"

class Client
{
	public:
		Client();
		~Client();
		Client(Client const &src);
		Client	&operator=(Client const &rhs);

		int							AcceptClient(int listener);
		void						SplitCmds(const string &str, const string delimiter = "\n");

		const struct pollfd			&GetPfd(void) const;
		const string				&GetIp() const;
		const std::vector<string>	&GetCmds() const;

	private:
		struct pollfd			_pfd;
		struct sockaddr_storage	_addr;
		socklen_t				_addrSize;
		string					_ip;
		std::vector< string >	_cmds;
};

void			*GetInAddr(struct sockaddr *sa);

#endif
