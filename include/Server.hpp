#ifndef SERVER_HPP
#define SERVER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Client.hpp"
#include "../include/Command.hpp"

#define MAX_LISTEN	5

/*
 *
 * Manage Client connections and Chanels
 *
 */
class Server
{
	public:
		/* Coplien */
		Server(const std::string &port = "5555", const std::string &pass = "admin");
		~Server();

		void		AwaitingConnectionQueue();
		void		InitConnectionLoop();
		void 		Logs() const;
		void 		ConnectionLoop();
		void 		SendData(int fd, const string &from, const string &s) const;

	private: 
		void		_AcceptNewConnection();
		void		_ReceiveData(struct pollfd &pfd);
		void		_ParseRecv(const string &buf, Client &client);
		void		_CloseConnection(struct pollfd &pfd);

		void 		_CapLs(Command &cmd, Client &client);// Parse CAP LS cmd
		void 		_Pass(Command &cmd, Client &client); // Parse PASS cmd
		void 		_Nick(Command &cmd, Client &client); // Parse NICK cmd
		void 		_User(Command &cmd, Client &client); // Parse USER cmd
		void 		_Ping(Command &cmd, Client &client); // Parse PING cmd

		std::string 					_portNumber;
		std::string 					_password;
		int 							_listener;
		std::map< int, Client > 		_clients;
		std::vector< struct pollfd >	_pollfds;
		int								_poll_count;
		std::map< string, CmdVal >		_mapCmd; // mapping between cmd names and integer; used for switch case.
};

#endif
