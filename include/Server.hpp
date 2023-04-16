#ifndef SERVER_HPP
#define SERVER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Command.hpp"

#define MAX_LISTEN	5
#define SERVER_NAME string("irc")

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

enum CmdVal {
	CAP = 0,
	PASS,
	NICK,
	USER,
	PING,
	PRIVMSG,
	MODE,
	NOTICE,
	OPER,
	JOIN,
	QUIT,
	KILL,
	PART,
	TOPIC,
	NAMES,
	LIST,
	INVITE,
	KICK,
	UNKNOWN
};

/*
 * Manage Client connections and Chanels
 */
class Server
{
	public:
		typedef  void (Server::*Fn)(Command &cmd, Client &client);
		/* Coplien */
		Server(const string &port, const string &pass, const string &operPass = "oper");
		~Server();

		void							AwaitingConnectionQueue();
		void							InitConnectionLoop();
		void 							Logs() const;
		void 							ConnectionLoop();
		void 							SendData(int fd);
		void							AddData(const string &from, const string &message, int n = 0);

	private: 
		void							_AcceptNewConnection();
		void							_ReceiveData(struct pollfd &pfd);
		void							_CloseConnection(struct pollfd &pfd);
		void							_CloseConnection(Client &client);
		vec_pfd::iterator				_GetPfdFromFd(int fd);

		// *** UTILS *** //
		void							_ExecCommand(Command cmd, Client &client); // Switch case
		CmdVal							_ResolveOption(const string &input); // Return a enum code for switch case eval
		Client*							_FindNickname(const string &nick, Client *skip = NULL); //check if there's a nickname like this in the list of client's nicknames
		Client* 						_FindUsername(const string &name, Client *skip = NULL);
		void							_NoticeServ(const string str, Client &client, int q = 0);
		cst_vec_str						&_WrapTargets(Command &cmd, size_t pos);

		// *** SERVER COMMANDS *** //
		void 							_CapLs(Command &cmd, Client &client);
		void 							_Oper(Command &cmd, Client &client);
		void 							_Pass(Command &cmd, Client &client);
		void 							_Nick(Command &cmd, Client &client);
		void 							_User(Command &cmd, Client &client);
		void 							_Pong(Command &cmd, Client &client);
		void 							_PrivMsg(Command &cmd, Client &client);
		void							_Mode(Command &cmd, Client &client);
		void							_Notice(Command &cmd, Client &client);
		void							_Kill(Command &cmd, Client &client);
		void							_Quit(Command &cmd, Client &client);
		
		// *** CHANNEL COMMANDS *** //
		void							_Join(Command &cmd, Client &client);
		void							_Part(Command &cmd, Client &client);
		void							_Topic(Command &cmd, Client &client);
		void							_Names(Command &cmd, Client &client);
		void							_List(Command &cmd, Client &client);
		void							_Invite(Command &cmd, Client &client);
		void							_Kick(Command &cmd, Client &client);

		std::string 					_portNumber;
		std::string 					_password;
		std::string 					_operPass;

		int 							_listener;

		std::map< int, Client > 		_clients;
		std::vector< struct pollfd >	_pollfds;
		int								_poll_count;
		std::map< string, CmdVal >		_mapCmd; // mapping between cmd names and integer; used for switch case.
		std::vector< Fn >				_funcTable;
		string							_data;
		std::list< Channel >			_channels;
};

#endif
