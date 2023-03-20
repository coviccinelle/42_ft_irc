#include "../include/Server.hpp"

Server::Server(int port, const std::string &pass) :
	_portNumber(port),
	_password(pass),
	_nick("")
/*,  _ClientsOnline(0), _MaxClientsOnline(50),  _fds(NULL), _operUsername("admin"), _operPwd("admin42")*/
{
//    _fds = new struct pollfd[MAX_LISTEN]; // http://manpagesfr.free.fr/man/man2/poll.2.html a free 
}


Server::~Server()
{
    //close(_sockfd);
}

//#define MAX_LISTEN	50
//
//#include <ctype.h>
//#define MAX 512
//
//#include <iostream>
//#include <string>
//#include <vector>
//#include <map> 
//#include <cctype>
//#include <cstring>
//
//size_t	is_cmd(std::string s)
//{
//	size_t pos = s.find("\r", 0);
//	if (pos != std::string::npos)
//	{
//		pos = s.find("\n", pos + 1);
//	}
//	return (pos);
//}
//
//void getCmds(std::string &s, std::vector<std::string> &out)
//{
//
//	for (size_t i = 0; i < s.size(); i++)
//	{
//		if (s[i] == '\r')
//			s[i] = '\n';
//	}
//	std::string token;
//	size_t last = 0;
//	size_t next = 0;
//	if ((next = s.find("\n")) != std::string::npos)
//	{
//		token = s.substr(last, next - last);
//		out.push_back(token);
//		last = next + 1;
//	}
//	else
//		return ;
//	token = s.substr(last);
//	out.push_back(token);
//
//}
//
//
///* renvoie un token à partir du delim, ici ' ' */
//void getArgs(std::string const &s, char delimiter,
//            std::vector<std::string> &out)
//{
//	std::string token;
//	size_t last = 0;
//	size_t next = 0;
//	while ((next = s.find(delimiter, last)) != std::string::npos)
//	{
//		token = s.substr(last, next - last);
//		out.push_back(token);
//		last = next + 1;
//	}
//	token = s.substr(last);
//	out.push_back(token);
//}
//
///*appelle la fonction pour faire les args et exec les commandes  */
//void Server::execCmd(std::string cmd, int fd) // TO DO : mettre dans une classe, la classe client ? 
//{
//	std::vector<std::string> arg;
//
//	if (!cmd.empty())
//		getArgs(cmd, ' ', arg);
//	else
//		return ;
//	for (size_t i = 0; i < (arg[0]).size(); i++)
//		arg[0][i] = toupper(arg[0][i]);
//	for (size_t i = 0; i < arg.size(); i++)
//     		std::cout << "exec_CMD arg[" << i << "]" << arg[i] << '\n';
//	if (arg[0] == "PASS")
//	{
//		std::cout << "found pass, call function" << std::endl;
//		Server::cmd_pass(fd, arg);
//	}
//	else if (arg[0] == "NICK") // TO DO : METTRE switch et case plutot
//	{
//		std::cout << "found nick, call function (avec std::vector<std::stin>arg et fd en arguments dans la classe client)" << std::endl;
//		Server::cmd_nick(fd, arg);
//	}
//		// TO DO tester avec fonction d'authentification Nick et pass etc void Server::cmd_pass(int fd, std::vector<std::string> cmd)
//	else if (arg[0] == "USER")
//		std::cout << "found user, call function" << std::endl;
//	else
//	 	std::cout << "no command found, need to erase this BS" << std::endl;
//
//    // std::string s[7] = {"/NICK", "/USER", "/JOIN", "/PART", "/PRIVMSG", "/LIST", "/QUIT"};
//    // int o = 0;
//	// switch (o) {
//	// 	case 1:
//	// 		std::cout << "/NICK -> call NICK function" << std::endl;
//	// 		break;
//	// 	case 2:
//	// 		std::cout << "/USER -> call USER function" << std::endl;
//	// 		break;
//	// 	case 3:
//	// 		std::cout << "/JOIN -> call JOIN function" << std::endl;
//	// 		break;
//	// 	case 4:
//	// 		std::cout << "/PART -> call PART function" << std::endl;
//	// 		break;
//	// 	case 5:
//	// 		std::cout << "/PRIVMSG -> call PRIVMSG function" << std::endl;
//	// 		break;
//	// 	case 6:
//	// 		std::cout << "/NOTICE -> call NOTICE function" << std::endl;
//	// 		break;
//	// 	case 7:
//	// 		std::cout << "/QUIT -> call QUIT function" << std::endl;
//	// 		break;
//	// 	default:
//	// 	{
//	// 		std::cout << "EEH : Command not found" << std::endl;
//	// 		break;
//	// 	}
//	// }
//	
//}
//#include <vector>
//#include <iostream>
//
//void eraseUnnecessarySpaces(std::string &str)
//{
//    for(size_t j = 0; j<=str.size(); j++)
//    {
//        for(size_t i = 0; i <= j; i++)
//        {
//            if(str[i] == ' ' && str[i+1] == ' ')
//            {
//                str.erase(str.begin() + i);
//            }
//            else if(str[0]== ' ')
//            {
//                str.erase(str.begin());
//            }
//            else if(str[i] == '\0' && str[i-1]== ' ')
//            {
//                str.erase(str.end() - 1);
//            }
//        }
//    }
//
//}
//
//
//void eraseEndLines(std::string &str)
//{
//    for(size_t j = 0; j<=str.size(); j++)
//    {
//        for(size_t i = 0; i <= j; i++)
//		{
//			if ((str[i] == '\n') || (str[i] == '\r'))
//				str.erase(str.begin() + i);
//           		 	else if(str[i] == '\0' && str[i-1]== '\r')
//				str.erase(str.end() - 1);
//
//		}
//	}
//}
//
//// recv line and push to cmdwaitingline by fd
//void Server::recvCmd(int fd)
//{
//
//	char		recvline[MAX + 1] = {0};
//	memset(recvline, 0, MAX);
//	recv(fd, recvline, MAX - 1, MSG_DONTWAIT);
//
//
//    std::string s = recvline;
//    std::cout << s;
//
//	eraseUnnecessarySpaces(s);
//    
//	std::vector<std::string> cmds;
//  	getCmds(s, cmds);
//
//	for (size_t i = 0; i < cmds.size(); i++)
//		eraseEndLines(cmds[i]);
//
//	if (!cmds.empty())	
//		for (size_t i = 0; i < cmds.size(); i++)
//     	 		std::cout << "cmd[" << i << "]" << cmds[i] << '\n';
//	else
//	{
//		std::cout << "ERROR: Aucune cmd" << std::endl;
//		return ;
//	}
//
//	std::cout  << "SIZE OF CMD " << cmds.size() << std::endl;
//	for (size_t i = 0; i < cmds.size(); i++)
//	{
//		//execCmd(cmds[i]);
//		std::cout << "Pushing cmd to cmdwaitingline, cmd =" << cmds[i] << std::endl;
//		_CmdsWaitingLine.push_back(std::make_pair(fd, cmds[i]));
//	}
//
//	Server::execCmd(cmds[1], fd);
//		//std::cout <<  _CmdsWaitingLine.second() << std::endl;
//}
//
/*

_clients[fd]->setUsername(username);
std::vector<std::pair(int, string)> _CmdsWaitingLine; // std::pair(int, string) = who/which fd ordered what/cmd //_CmdsWaitingLine.push_back(std::make_pair(fd, cmd));

					char		recvline[MAX + 1] = {0};
					memset(recvline, 0, MAX);
					recv(fd, recvline, MAX - 1, MSG_DONTWAIT);
					_CmdsWaitingLine.push_back(std::make_pair(fd, cmd));

*/

//
//
//
///*
////L'argument sockfd est une socket qui a été créée avec la fonction socket(2), attachée à une adresse
//// avec bind(2), et attend des connextions après un appel listen(2)
//Une socket SOCK_STREAM doit être dans un état connecté avant que des données puisse y être lues ou écrites. 
//Les protocoles de communication qui implémentent les sockets SOCK_STREAM garantissent qu'aucune donnée n'est perdue ou dupliquée. Si un bloc de données, pour lequel le correspondant a suffisamment de place dans son tampon, n'est pas transmis correctement dans un délai raisonnable, la connexion est considérée comme inutilisable. PF_INET protocole internet ipv4
//bind() It’s a socket identification like a telephone number to contact
//*/
//void	Server::AwaitingConnectionLoop()
//{
//	struct protoent	*p = getprotobyname("tcp");
//	int	opt = 1;
//#ifdef __APPLE__
//	if ((_sockfd = socket(AF_INET, SOCK_STREAM, p->p_proto)) == -1)
//		throw system_error("socket");
//	fcntl(_sockfd, F_SETFL, O_NONBLOCK);
//	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
//		throw system_error("setsockopt");
//#else
//	if ((_sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, p->p_proto)) == -1)
//		throw system_error("socket");
//	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) == -1)
//		throw system_error("setsockopt");
//#endif
//	sockaddr_in	sin;
//	sin.sin_family = AF_INET;
//	sin.sin_addr.s_addr = INADDR_ANY;
//	sin.sin_port = htons(_portNumber);
//	if (bind(_sockfd, (sockaddr *)&sin, sizeof(sin)) == -1)
//		throw system_error("bind");
//	if (listen(_sockfd, MAX_LISTEN) == -1)
//		throw system_error("listen");
//	addClientToPoll(_sockfd);
//		
//}
//
//
//void	Server::addClientToPoll(int newClientfd)
//{
//	if (_ClientsOnline == _MaxClientsOnline)
//	{
//		_MaxClientsOnline *= 2;
//		_fds = (struct pollfd *)realloc(_fds, _MaxClientsOnline);
//	}
//	_fds[_ClientsOnline].fd = newClientfd;
//	_fds[_ClientsOnline].events = POLLIN;
//	_clients.insert(std::pair<int, Client *>(newClientfd, new Client(newClientfd)));
//	_ClientsOnline++;
//};
//
//void	Server::removeClientFromPoll(int i)
//{
//	close(_fds[i].fd);
//	_fds[i] = _fds[_ClientsOnline - 1];
//	_clients.erase(_fds[i].fd);
//	_ClientsOnline--;
//};
//
///*
//Accept() extrait la première connexion de la file des connexions en attente de la socket sockfd à l'écoute, crée une nouvelle socket connectée, et renvoie un nouveau descripteur de fichier qui fait référence à cette socket
//S'il n'y a pas de connexion en attente dans la file, et si la socket n'est pas marquée comme non bloquante, accept() se met en attente d'une connexion. Si la socket est non bloquante, et qu'aucune connexion n'est présente dans la file, accept() échoue avec l'erreur EAGAIN.
//*/
//void	Server::AcceptNewClient(void)
//{
//	struct sockaddr_storage	remotaddr;
//	socklen_t				addrlen;
//	int newClientfd;
//
//	addrlen = sizeof(remotaddr);
//	std::cout << "0.1. Waiting for new client" << std::endl;
//	newClientfd = accept(_sockfd, (struct sockaddr*)&remotaddr, &addrlen);
//	std::cout << "0.2. New client accepted" << std::endl;
//	if (newClientfd == -1)
//	{	
//		std::cerr << "Accept() error: " << strerror(errno) << std::endl;
//		return ;
//	}	
//	addClientToPoll(newClientfd);
//	std::cout <<  "0.2. Accepted new client number : " << newClientfd << std::endl;
//};
//
//
//// # define MAX 512
//
//// /*comprendre pourquoi connection lost maintenant */
//
//int Server::check_password(std::string recvline, int fd)
//{
//	(void)fd;
//	std::string cap_ls = recvline.substr(0, recvline.find("\r\n"));
//	std::cout << "\033[0;32mCAP_LS is => " << cap_ls << std::endl;
//	if (cap_ls == "CAP LS")
//		std::cout << "\033[0;32mCAP LS is present" << std::endl;
//
//	std::string password = recvline.substr(recvline.find("\n") + 1, recvline.length());
//	std::cout << "\033[0;32m 1.0 The rest From Password is => " << password << std::endl;
//	if (password.find("PASS ") != std::string::npos)
//	{
//	//Server::cmd_pass(fd, cmd);
//		password = password.substr(5, password.substr(password.find("PASS ") + 5, password.length()).find("\r\n"));
//		std::cout << "\033[1;33mPassword is => [" << password << "]"<< std::endl;
//		std::cout << "\033[1;33mPassword server is => [" << _password << "]"<< std::endl;
//		if (_password == password)
//		{
//			std::cout << "\033[0;32mPassword is correct" << std::endl;
//			std::string username = recvline.substr(recvline.find("NICK ") + 5, recvline.substr(recvline.find("NICK ") + 5, recvline.length()).find("\r\n"));
//			std::cout << "\033[1;33mUsername is => [" << username << "]"<< std::endl;
//			send(fd, "001 thi-phng :Welcome to the Internet Relay Network!\r\n", 55, 0);
//			_clients[fd]->setUsername(username);
//			std::cout << "\033[0;32m 1.1. Username set to " << _clients[fd]->getUsername() << std::endl;
//			//_clients[fd]->setConnected(true);
//		 	std::cout << "Client " << fd << " connected" << std::endl;
//			return (1);
//		}
//		else
//		{
//			std::cout << "\033[0;31mPassword is incorrect" << std::endl;
//			removeClientFromPoll(fd);
//			std::cout << "Client " << fd << " disconnected" << std::endl;
//			return (0);
//		}
//	}
//	else
//	{
//		removeClientFromPoll(fd);
//		std::cout << "\033[0;31mPassword doesn't exist. Wait for the next ping for more infomations" << std::endl;
//		return (-1);
//	}
//	return (0);
//}
//
//#include <algorithm> 
//
///*
//Pour être prévenu de l'arrivée d'une connexion sur une socket, on peut utiliser select(2) ou poll(2). Un événement « lecture » sera délivré lorsqu'une tentative de connexion de socket aura lieu, et on pourra alors appeler accept() pour la valider. 
//A positive value indicates the total number of file descriptors that have been selected (that is, file descriptors for which the revents member is non-zero). A value of 0 indicates that the call timed out and no file descriptors have been selected. Upon failure, poll() shall return -1 and set errno to indicate the error.
//*/
//void Server::FdMultithreading(void)
//{
//	while (true)
//	{
//		int poll_count = poll(_fds, _ClientsOnline, -1); 
//		if (poll_count == -1)
//			 throw system_error("poll");
//		if (poll_count == 0)
//			std::cout << "ZERO: call timed out " << std::endl;
//		for (int i = 0; i < _ClientsOnline; i++)
//		{
//			if (_fds[i].revents & POLLIN)
//			{
//
//				if (_fds[i].fd == _sockfd)
//					AcceptNewClient();
//				else
//				{
//					// recvCmd(_fds[i].fd);
//					_clients[_fds[i].fd]->recv_all(_fds[i].fd, *this);
//
//				}	
//			}		
//		}
//	}
//};
