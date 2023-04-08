#ifndef SERVER_HPP
#define SERVER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Client.hpp"
#include "../include/Command.hpp"

#define MAX_LISTEN	5
#define SERVER_NAME string("irc")

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
	PRIVMSG,
	MODE,
	NOTICE,
	OPER
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
		Server(const string &port, const string &pass, const string &operPass = "oper");
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

		void 		_CapLs(const Command &cmd, Client &client);
		void 		_Oper(const Command &cmd, Client &client);
		void 		_Pass(const Command &cmd, Client &client);
		void 		_Nick(const Command &cmd, Client &client);
		void 		_User(const Command &cmd, Client &client);
		void 		_Pong(const Command &cmd, Client &client);
		void 		_PrivMsg(const Command &cmd, Client &client);
		void		_Mode(const Command &cmd, Client &client);
		void		_Notice(const Command &cmd, Client &client);
		Client*		_FindNickname(const string &nick, Client *skip = NULL); //check if there's a nickname like this in the list of client's nicknames
		Client* 	_FindUsername(const string &name, Client *skip = NULL);

		std::string 					_portNumber;
		std::string 					_password;
		std::string 					_operPass;
		int 							_listener;
		std::map< int, Client > 		_clients;
		std::vector< struct pollfd >	_pollfds;
		int								_poll_count;
		std::map< string, CmdVal >		_mapCmd; // mapping between cmd names and integer; used for switch case.
		Parser							_parser;
		string							_data;
};

#endif
