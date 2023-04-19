#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include "../utils/utils.hpp"
#include "../include/Client.hpp"

class Client;

/* NOTE:
 * 	NAMESPACE:
 *		- Channels names are string beginning with a '&', '#', '+', '!' character of length up to 50 char.
 *			Channel are case insensitive
 *		- Charset '&', '#', '+', '!' are called channel prefix
 *		- Restriction of channel name are (' ', ^G (ASCII 7), ',')
 *		- (':') are used as delimiter for channel mask 
 *		- Exact syntax: https://datatracker.ietf.org/doc/html/rfc2812#section-2.3.1
 *		- Prefix create distinct namespace for channel names
 *
 *	SCOPE:
 *		- A channel is known by the server user can only become member of channel known by the server,
 *			to which the user is connected.
 * 		- Prefix is known by one or more server (for us just one)
 * 		- '&' Prefix for us as no specific effect because we do not handle server to server communication.
 * 		- Channel mask doesn't do effect for us we just have to check if syntax is valid.
 *
 * 	PROPERTIES:
 * 		- Each channel has its own properties which are defined by channel modes.
 * 			Channel modes can be manipulated by the channel members.
 * 			The modes affect the way server manage the channels.
 * 		- Channel with '+' prefix do not support channel modes.
 * 			With the exception of the 't' channel flag which is set.
 *
 * PRIVILEGED CHAN MEMBER:
 * 	- Some channel member are pirviliged.
 * 	- Priviliged channel member can do the following action on the channel:
 * 		INVITE (Invite a client to an invite-only channel (mode +i))
 * 		KICK (Eject a client from the channel)
 * 		MODE (Change the channel's mode, as well as members privileges)
 * 		PRIVMSG (Sending messages to the channel (mode +n, +m, +v)
 * 		TOPIC (Change the channel topic in a mode +t channel)
 *
 * 		CHANNEL OPERATOR:
 * 			- Referred as "chop" or "chanop" on a given channel are considered as 'own' that channel.
 * 				Ownership is shared among chanop.
 * 			- Chanop are identified by the '@' symbol next to their nickname
 * 				whenever it is associated with a channel (replies to the NAMES, WHO, WHOIS commands)
 * 				(probably client side thing dont have to code this)
 * 			- Since channel created with prefix '+' do not support channel modes,
 * 				no member can therefore have the status of chanop.
 *
 * 		CHANNEL CREATOR:
 * 			- A user who create channel with '!' prefix is a channel creator.
 * 				Is also given chanop status upon creation.
 * 			- Channel creator have the ability to toggle certain modes of the chanel,
 * 				which chanop cannot manipulate.
 *
 * 	CHANNEL LIFETIME:
 * 		- We have two groups of channel:
 * 			Standard channels which prefix is either '&', '#' or '+'.
 * 			Safe channels which prefix is '!'
 * */

// Modify if needed or remove
enum defaultReturn {
	ERR_SOMETHING,
	ERR_NOCHANMODES,
	ERR_CHANOPRIVSNEEDED,
	RPL_SOMETHING
};

//  Use bitwise or and bitwise and to retrive specific mode
//  Flag for the member to be added in specific list
enum memberMode {
	MEM_CHANOP = 1 << 13,
	MEM_VOICE = 1 << 14
};

//  Use bitwise or and bitwise and to retrive specific mode
//  All the Channel flags that could be used with MODE command
enum channelMode {
	CHAN_ANONYMOUS = 1, // a
	CHAN_INVITEONLY = 1 << 1, // i
	CHAN_MODERATED = 1 << 2, // m
	CHAN_NOMESSAGE = 1 << 3, // n
	CHAN_QUIET = 1 << 4, // q
	CHAN_PRIVATE = 1 << 5, // p
	CHAN_SECRET = 1 << 6, // s
	CHAN_REOP = 1 << 7, // r
	CHAN_TOPIC = 1 << 8, // t
	CHAN_LIMIT = 1 << 9, // l
	CHAN_KEY = 1 << 10, // k
	CHAN_BAN = 1 << 11, // b
	CHAN_EXCEPTION = 1 << 12 // e
};

typedef std::list< Client* >::iterator	lst_iterator;

class Channel
{
	private:
		Client					*_creator; // Should have only one creator (not possible to set it by user)
									  // even if creator itself want to set an another one.

		std::list< Client* >	_chanop; // Privileged users can do stuff that other cant
		std::list< Client* >	_voice; // Allow users to talk in moderated channel (m flags)
		std::list< Client* >	_user; // User list present in channel
		std::list< Client* >	_invite; // Invite list use when +i flag is set
		std::list< Client* >	_ban; // Ban list use when ban flag is specified.
		std::list< Client* >	_exception; // Exception '+e' list work in conjoncture with ban list allow exception for specific nickname, username, hostname.
 
		string				_key; // Key used to access to channel
		string				_chanstring; // Channel name
		string				_topic; // Channel's topic

		bool				_safe; // Safe channel (created with prefix '!')
		bool				_modeless; // Modeless channel (created with prefix '+') also no chanop

		int					_mode; // bitwise or value that have corresponding mode added to
		int					_size; // 0 is unlimited with flag (+l) add limit to channel user only for regular user

		/* Private Methods */
		int					_validPrefix(const char& chanstring) const;

		// Add or delete member status to specific user.
		int					_addUserStatus(const int& mode, const string& name);
		int					_delUserStatus(const int& mode, const string& name);

		// Send message to every user present in channel.
		void				_sendToAll(const string& msg, const Client& client) const;

		// Flag method use to remove or add in _mode
		void				_delMode(const int& mode);
		void				_addMode(const int& mode);

		// Overload use in public method setChanMode
		void				_setChanMode(const char& modif, const int& mode);
		void				_setChanMode(const char& modif, const int& mode, const string& params);

		// Function to find specific user to specific corresponding list
		lst_iterator		_findChanopIter(const string& name);
		lst_iterator		_findVoiceIter(const string& name);

		Client&				_findUser(const string& name);
		Client&				_findChanop(const string& name);
		Client&				_findVoice(const string& name);

		// Function to compare if client exist in the coresponding list throught nickname
		bool				_compareCreator(const string& name) const;
		bool				_compareVoice(const string& name) const;
		bool				_compareChanop(const string& name) const;
		bool				_compareUser(const string& name) const;

		// Function to compare if client exist in the coresponding list throught client
		bool				_compareCreator(const Client& client) const;
		bool				_compareVoice(const Client& client) const;
		bool				_compareChanop(const Client& client) const;
		bool				_compareUser(const Client& client) const;
		bool				_compareBan(const Client& client) const;
		bool				_compareException(const Client& client) const;

	public:
		/* Coplien */
		Channel(const string &chanstring = "default");
		Channel(string& chanstring, const Client& client); // Create channel object without channel mask
		Channel(const Channel& chan);
		~Channel();
		Channel&	operator=(const Channel& rhs);

		lst_pcli::iterator		findUserIter(const string& name);
		const std::list< Client* > 	&GetUser() const;
		const string 				&GetTopic() const;

		void						SetTopic(const string& params);
		bool						IsOperator(const Client& client) const;

		/* Public Methods */
		void			sendMessage(const string& msg, const Client& clientSend) const;
		int				setMemberStatus(char modif, int mode, const Client& clientSend, const string& params);
		int				setChanMode(char modif, int mode, const Client& clientSend, const string& params);
		int				joinChannel(Client& toAccept);
		void			leaveChannel(Client& toAccept);

		cst_lst_pcli	&GetUsers() const;
		const string	&GetName() const;
		string			GetLstNickname() const;

		friend std::ostream&	operator<<(std::ostream& lhs, const Channel& rhs);
};

#endif
