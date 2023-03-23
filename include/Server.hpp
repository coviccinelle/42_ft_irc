#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h> // pour les flags F_SETFL et O_NONBLOCK
#include <signal.h>
#include <poll.h>

#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include <vector>

#include "../utils/utils.hpp"
#include "../include/system_error.hpp"
#include "../include/Client.hpp"

#define MAX_LISTEN	5

class Client;
class Server
{
	public:
		Server(const std::string &port, const std::string &pass);
		~Server();

		void		AwaitingConnectionQueue();
		void		InitConnectionLoop();
		void 		ConnectionLoop();

	private:
		Server();
		void _AcceptNewConnection();
		void _ReceiveData(struct pollfd &pfd);
		void _SendData(struct pollfd &pfd);
		void _parseRecv(char *buf);
		int	 _sender(int fd, char *buf);


		std::string 					_portNumber;
		std::string 					_password;
		int 							_listener;
		std::vector< Client > 			_clients;
		std::vector< struct pollfd > 	_pollfds;
		int								_poll_count;
};

#endif
