#ifndef SERVER_HPP
#define SERVER_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include "../include/Command.hpp"

#define MAX_LISTEN	5
#define SERVER_NAME string("irc")
#define SERVER_VERSION string("0.0.1")
#define SERVER_DATE string("Mon Mar 2023 at 15:11:00 UTC")

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
	WHO,
	UNKNOWN
};

/*
 * Manage Client connections and Channels
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
		void							SendChannel(lst_pchan::const_iterator chan, const string &message, const string &from, const Client *skip = NULL);
		void							SendChannel(lst_chan::const_iterator chan, const string &message, const string &from, const Client *skip = NULL);
		void							AddData(const string &message, const string &from = SERVER_NAME);

	private: 
		void							_AcceptNewConnection();
		void							_ReceiveData(struct pollfd &pfd);
		void							_CloseConnection(struct pollfd &pfd);
		void							_CloseConnection(Client &client);
		vec_pfd::iterator				_GetPfdFromFd(int fd);

		// *** UTILS *** //
		void							_ExecCommand(Command cmd, Client &client); // Switch case
		CmdVal							_ResolveOption(const string &input); // Return a enum code for switch case eval
		Client							*_FindNickname(const string &nick, Client *skip = NULL); //check if there's a nickname like this in the list of client's nicknames
		Client							*_FindUsername(const string &name, Client *skip = NULL);
		lst_chan::iterator				_FindChannel(const string &name);
		void							_NoticeServ(const string str, Client &client, int q = 0);
		cst_vec_str						&_WrapTargets(Command &cmd, size_t pos);
		cst_vec_vec_str					&_WrapChannels(Command &cmd, size_t pos);

		// *** SERVER COMMANDS *** //
		void 							_CapLs(Command &cmd, Client &client);
		void 							_Oper(Command &cmd, Client &client);
		void 							_Pass(Command &cmd, Client &client);
		void 							_Nick(Command &cmd, Client &client);
		void 							_User(Command &cmd, Client &client);
		void 							_Pong(Command &cmd, Client &client);
		void 							_PrivMsg(Command &cmd, Client &client);
		void							_ModeServer(Command &cmd, Client &client, const string &target);
		void							_ModeClient(Command &cmd, Client &client, const string &target);
		void							_Mode(Command &cmd, Client &client);
		void							_Notice(Command &cmd, Client &client);
		void							_Kill(Command &cmd, Client &client);
		void							_Quit(Command &cmd, Client &client);
		
		// *** CHANNEL COMMANDS *** //
		lst_chan::iterator				_chanExist(const string chanName);
		void							_Join(Command &cmd, Client &client);
		void							_Part(Command &cmd, Client &client);
		void							_Topic(Command &cmd, Client &client);
		void							_Names(Command &cmd, Client &client);
		void							_List(Command &cmd, Client &client);
		void							_Invite(Command &cmd, Client &client);
		void							_Kick(Command &cmd, Client &client);
		void							_Who(Command &cmd, Client &client);

		std::string 					_portNumber;
		std::string 					_password;
		std::string 					_operPass;

		int 							_listener;

		map_int_cli						_clients;
		vec_pfd							_pollfds;
		int								_poll_count;
		std::map< string, CmdVal >		_mapCmd; // mapping between cmd names and integer; used for switch case.
		std::vector< Fn >				_funcTable;
		string							_data;
		lst_chan						_channels;
};

#endif
