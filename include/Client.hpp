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
		void						SplitCmds(const string &str, const string delimiter = "\r\n");

		int							GetFd(void) const;
		const string				&GetIp() const;
		cst_vec_vec_str				&GetCmds() const;
		bool						IsConnected() const;
		int							ParseRecv(const string &buf, const string &pass);

		void						SetConnected(bool b);

	private:
		int							_fd;
		struct sockaddr_storage		_addr;
		socklen_t					_addrSize;
		string						_ip;
		vec_vec_str					_cmds;
		bool						_validpass;
};

void								*GetInAddr(struct sockaddr *sa);
vec_str								Split(const string &str, const string delimiter = " ");

#endif
