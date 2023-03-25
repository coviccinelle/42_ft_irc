#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../utils/utils.hpp"
#include "../include/system_error.hpp"

enum CmdVal {
	UNKNOWN = 0,
	PASS
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
		int							_fd;
		struct sockaddr_storage		_addr;
		socklen_t					_addrSize;
		string						_ip;
		vec_vec_str					_cmds;
		bool						_validPass;
		string						_servPass;
		std::map< string, CmdVal >	_mapCmd;
};

void								*GetInAddr(struct sockaddr *sa);
vec_str								Split(const string &str, const string delimiter = " ");

#endif
