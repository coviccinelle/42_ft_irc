#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../utils/utils.hpp"
#include "../include/system_error.hpp"

# define RPL_WELCOME(nick, user, host) ("001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n")
# define ERR_NEEDMOREPARAMS(command) (std::string("461 ") + command + " :Not enough parameters\r\n")
# define ERR_ALREADYREGISTERED "462 :You may not reregister\r\n"

#define INF_CLI_SIZE 5

enum InfoClient {
	nickname = 0,
	username,
	hostname,
	servername,
	realname
};

enum CmdVal {
	UNKNOWN = 0,
	PASS,
	NICK,
	USER
};

class Client
{
	public:
		Client(const string &pass = "");
		~Client();
		Client(Client const &src);
		Client	&operator=(Client const &rhs);

		int							AcceptClient(int listener);
		void						SplitCmds(const string &str, const string delimiter = "\r\n");
		void 						SendData(const string &msg) const;

		int							GetFd(void) const;
		const string				&GetIp() const;
		cst_vec_vec_str				&GetCmds() const;
		bool						IsConnected() const;
		void						ExecCommand(cst_vec_str &cmd);
		int							ParseRecv(const string &buf);
		CmdVal						ResolveOption(const string &input);

		void						SetConnected(bool b);

	private:
		void						_Pass(cst_vec_str &cmd);
		void						_Nick(cst_vec_str &cmd);
		void						_User(cst_vec_str &cmd);

		int							_fd;
		struct sockaddr_storage		_addr;
		socklen_t					_addrSize;
		string						_ip;
		string						_servPass;

		vec_vec_str					_cmds;
		std::map< string, CmdVal >	_mapCmd;

		bool						_validPass;
		vec_str						_uinfo;
};

void								*GetInAddr(struct sockaddr *sa);
vec_str								Split(const string &str, const string delimiter = " ");

#endif
