#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Parser.hpp"
#include "../include/Errors.hpp"

// Size of InfoClient enum below
#define INF_CLI_SIZE 6

// List of infos on a client
enum InfoClient {
	password = 0,
	nickname,
	username,
	hostname,
	servername,
	realname
};

/* 
 * 
 * This class hold all information about a connected client.
 *
 * Client objected are created at connection time by the server
 * and destroyed when connection is lost.
 * 
 */
class Client
{
	public:
		/* Coplien */
		Client(); // Forbidden Client creation without Server clients list reference or no server pass
		Client(const string &pass, const std::map< int, Client > &clients);
		~Client();
		Client(Client const &src);
		Client	&operator=(Client const &rhs);

		/* Public Methods */
		int								AcceptClient(int listener); // Return socket fd for socker communication

		void							ParseRecv(const string &buf); // Parse the cmd received by the server

		/* Getters */
		int								GetFd(void) const;
		const string					&GetIp() const;
		cst_vec_str						&GetUinfo() const;
		const std::list< Command >		&GetCmds() const;
		bool							IsRegistd() const;
		void							PopCmd();

		void							SetUinfo(const vec_str &uinfo);
		void							SetRegistd();

	private:
		/* Private Methods */
		void							_ParseBuf(const string &buf);
		void							ValidNickname(const string &nick);

		/* Connection Info */
		int								_fd; // Connection socket
		struct sockaddr_storage			_addr;
		socklen_t						_addrSize;
		string							_ip;
		string							_servPass; // Server Password (should be a string ref)

		/* Commands */
		string							_buf;
		std::list< Command >			_cmds; // Commands that need to be process

		/* Client info */
		bool							_registd;
		vec_str							_uinfo; // nickname, username, hostname ... See InfoClient above for all available field.

		/* Utils */
		const std::map< int, Client >	*_clients;
		Parser							_parser;
};

// Non-Member function
void								*GetInAddr(struct sockaddr *sa); // Return sin_addr (IpV4) or sin6_addr (IpV6)
vec_str								Split(const string &str, const string delimiter = " "); // Split string using delimiter

#endif
