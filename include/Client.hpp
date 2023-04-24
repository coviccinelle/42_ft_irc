#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"
#include "../include/Command.hpp"
#include "../include/Channel.hpp"
#include "../include/Errors.hpp"

// Size of InfoClient enum below
#define INF_CLI_SIZE 6

class Channel;

#define USER_MODE string("iwos")
#define MODE_SIZE 4

// List of infos on a client
enum InfoClient {
	password = 0,
	nickname,
	username,
	hostname,
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
		~Client();
		Client(Client const &src);
		Client	&operator=(Client const &rhs);

		/* Public Methods */
		int								AcceptClient(int listener); // Return socket fd for socket communication
		void							ParseRecv(const string &buf); // Parse the cmd received by the server

		/* Getters */
		int								GetFd(void) const;
		const string					&GetIp() const;
		cst_vec_str						&GetUinfo() const;
		string							GetPrefix() const;
		const std::list< Command >		&GetCmds() const;
		const std::bitset< MODE_SIZE >	&GetMode() const;
		string							GetStrMode() const;
		bool							IsRegistd() const;
		void							PopCmd();

		void							SetUinfo(const vec_str &uinfo);
		void							SetRegistd();
		void							SetStrMode(const string &mode);
		void							SetMode(const char c, bool status);

		void							RegisterChannel(Channel &chan);
		void							DeregisterChannel(Channel &chan);
		const std::list< Channel* >		&GetChannels() const;

		// User Mode info
		bool							isInvisible() const; // +i
		bool							isWallops() const; // +w
		bool							isOperator() const; // +o
		bool							isServNotice() const; // +s

	private:
		/* Private Methods */
		void							_ParseBuf(const string &buf);

		/* Connection Info */
		int								_fd; // Connection socket
		struct sockaddr_storage			_addr;
		socklen_t						_addrSize;
		string							_ip;

		/* Commands */
		string							_buf;
		std::list< Command >			_cmds; // Commands that need to be process
		std::list< Channel* >			_channels;

		/* Client info */
		bool							_registd;
		vec_str							_uinfo; // nickname, username, hostname ... See InfoClient above for all available field.
		std::bitset< MODE_SIZE >		_mode;
};

// Non-Member function
void								*GetInAddr(struct sockaddr *sa); // Return sin_addr (IpV4) or sin6_addr (IpV6)
vec_str								Split(const string &str, const string delimiter = " "); // Split string using delimiter

#endif
