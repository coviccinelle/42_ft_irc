#ifndef SERVER_HPP
#define SERVER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Client.hpp"
#include "../include/Command.hpp"

#define MAX_LISTEN	5

/*
 * Used for switch case evaluation
 */
enum CmdVal {
	UNKNOWN = 0,
	CAP,
	PASS,
	NICK,
	USER,
	PING,
	PRIVMSG
};

/*
 *
 * Manage Client connections and Chanels
 *
 */
class Server
{
	public:
		/* Coplien */
		Server(const std::string &port, const std::string &pas);
		~Server();

		void		AwaitingConnectionQueue();
		void		InitConnectionLoop();
		void 		Logs() const;
		void 		ConnectionLoop();
		void 		SendData(int fd);
		void		AddData(const string &from, const string &message, int n = 0);

	private: 
		void		_AcceptNewConnection();
		void		_ReceiveData(struct pollfd &pfd);
		void		_ParseRecv(const string &buf, Client &client);
		void		_CloseConnection(struct pollfd &pfd);

		void		_ExecCommand(const Command &cmd, Client &client); // Switch case
		CmdVal		_ResolveOption(const string &input); // Return a enum code for switch case eval

		void 		_CapLs(const Command &cmd, Client &client);// Parse CAP LS cmd
		void 		_Pass(const Command &cmd, Client &client); // Parse PASS cmd
		void 		_Nick(const Command &cmd, Client &client); // Parse NICK cmd
		void 		_User(const Command &cmd, Client &client); // Parse USER cmd
		void 		_Ping(const Command &cmd, Client &client); // Parse PING cmd
		void 		_PrivMsg(const Command &cmd, Client &client); // Parse PRIVMSG cmd
																  //
		Client*	_FindNickname(const string &nick, Client *skip = NULL); //check if there's a nickname like this in the list of client's nicknames

		std::string 					_portNumber;
		std::string 					_password;
		int 							_listener;
		std::map< int, Client > 		_clients;
		std::vector< struct pollfd >	_pollfds;
		int								_poll_count;
		std::map< string, CmdVal >		_mapCmd; // mapping between cmd names and integer; used for switch case.
		Parser							_parser;
		string							_data;
};

#endif
