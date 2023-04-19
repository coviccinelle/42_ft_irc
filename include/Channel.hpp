#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include "../utils/utils.hpp"
#include "../include/Client.hpp"

#define MEMBER_MODE string("oO")
#define CHAN_MODE string("b")

class Client;

#define CHAN_MODE_SIZE 1
enum chanmode {
	CHAN_BAN = 0, // b
};

#define MEMBER_MODE_SIZE 2
enum memberMode {
	MEM_CHANOP = 0, // o
	MEM_CHANCREATOR = 1 // O
};

typedef std::map< Client*, std::bitset< MEMBER_MODE_SIZE > >		map_pcli;	
typedef const std::map< Client*, std::bitset< MEMBER_MODE_SIZE > >	cst_map_pcli;
typedef std::bitset< CHAN_MODE_SIZE >								chan_mode;
typedef const std::bitset< CHAN_MODE_SIZE >							cst_chan_mode;

class Channel
{
	public:
		/* Coplien */
		Channel(const string &chanstring = "default");
		~Channel();
		Channel(const Channel& chan);
		Channel	&operator=(const Channel& rhs);

		/* Public Methods */
		void						SetChanMode(const char c, bool status);
		void						SetMemberMode(Client &client, const char c, bool status);

		int							joinChannel(Client& toAccept);
		void						leaveChannel(Client& toRemove);
		map_pcli::const_iterator	findUserIter(const string &toFind) const;

		cst_map_pcli				&GetUsers() const;
		const string				&GetName() const;
		string						GetLstNickname() const;
	private:
		Client						*_creator;
		map_pcli					_user;
		string						_chanstring;
		chan_mode					_mode;
};

#endif
