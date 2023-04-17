#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <cctype>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <bitset>

#include <fcntl.h> // pour les flags F_SETFL et O_NONBLOCK
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <poll.h>
#include <arpa/inet.h>

#define SERVER_NAME string("irc")
#define USER_MODE string("iwos")
#define MODE_SIZE 4

class Client;
class Channel;

typedef std::string 						string;

typedef std::vector< string >				vec_str;
typedef const std::vector< string >			cst_vec_str;

typedef std::vector< vec_str >				vec_vec_str;
typedef const std::vector< vec_str >		cst_vec_vec_str;

typedef std::vector< struct pollfd >		vec_pfd;
typedef const std::vector< struct pollfd >	cst_vec_pfd;

typedef std::map< int, Client >				map_int_cli;
typedef const std::map< int, Client >		cst_map_int_cli;

typedef std::vector< struct pollfd >		vec_pfd;
typedef const std::vector< struct pollfd >	cst_vec_pfd;

typedef std::list< Channel >				lst_chan;
typedef const std::list< Channel >			cst_lst_chan;

typedef std::list< Client* >				lst_pcli;
typedef const std::list< Client* >			cst_lst_pcli;


string	ltrim(const string &s);
string	rtrim(const string &s);
string 	trim(const string &s);

#endif
