#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include  <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <fcntl.h> // pour les flags F_SETFL et O_NONBLOCK


#include <unistd.h>
//#include "SystemError.hpp"

#include <poll.h>
#include <map>

#include <vector>
//#include "Client.hpp"
//#include "Server.hpp"

class Client;
class Server
{
	public:
		Server(int port, const std::string &pass);
		~Server();

		//std::string getPassword(){return _password;};
		//std::string getUsername(){return _username;};
		//void		setup();
		//int			getPortNumber(){return _portNumber;};
		//int			parsing(std::string recvline, int fd);
		//int 		check_password(std::string recvline, int fd);
		//void 		setPassword(std::string password){_password = password;};
		//void 		setUsername(std::string username){_username = username;};
		//void 		setPortNumber(int portnumber){_portNumber = portnumber;};
		//void		AwaitingConnectionLoop();
		//void    	AcceptNewClient();
		//void    	addClientToPoll(int newClientfd);
		//void    	removeClientFromPoll(int i);
		//void 		FdMultithreading(void);
		//void 		execCmd(std::string cmd, int fd);
		//std::string	_message;
		////waitingLine();

		//std::string	recv_all(int fd);
		////commandes
		//void cmd_nick(int fd, std::vector<std::string> cmd);
		//void cmd_pass(int fd, std::vector<std::string> cmd);
		//void cmd_user(int fd, std::vector<std::string> cmd);
		//void cmd_oper(int fd, std::vector<std::string> cmd);
		//void recvCmd(int fd);

	private:
		Server();
		//typedef std::pair<int, std::string> WhoOrderedWhat;

		 //std::vector<WhoOrderedWhat> _CmdsWaitingLine; // std::pair(int, string) = who/which fd ordered what/cmd //_CmdsWaitingLine.push_back(std::make_pair(fd, cmd));

		//std::map<int, std::string>	_CmdsWaitingLine;
		int 		_portNumber;
		std::string _password;
		std::string _nick;
//		const char *_portNumberChar;
//		int 		_sockfd;
//		int 		_ClientsOnline;
//		int 		_MaxClientsOnline;
//		struct pollfd						*_fds;
//		std::map<int, Client> clients;

//		std::map<int, Client *>				_clients;
//
//		std::string _operUsername;
//		std::string _operPwd;
//		typedef std::vector<std::make_pair(int, cmd)> waitingcmds;

};

#endif
