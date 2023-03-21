#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h> // pour les flags F_SETFL et O_NONBLOCK
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
		void 		AcceptClientConnection();

	private:
		Server();


		std::string 			_portNumber;
		std::string 			_password;
		std::string 			_nick;
		int 					_sockfd;
		std::vector< Client > 	*_connectedClients;
};
