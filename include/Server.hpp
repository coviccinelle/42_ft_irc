#ifndef SERVER_HPP
#define SERVER_HPP

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
		void 		Logs() const;
		void 		ConnectionLoop();

	private:
		Server();
		void	_AcceptNewConnection();
		void	_ReceiveData(struct pollfd &pfd);
		void	_ParseRecv(const string &buf, Client &client);
		void	_CloseConnection(struct pollfd &pfd);


		std::string 					_portNumber;
		std::string 					_password;
		int 							_listener;
		std::map< int, Client > 		_clients;
		std::vector< struct pollfd >	_pollfds;
		int								_poll_count;
};

#endif
