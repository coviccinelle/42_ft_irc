#ifndef CHANNEL_HPP
# define CHANNEL_HPP
#include <ctime>
#include <iostream>
#include <string>
#include <sstream>
#include "../utils/utils.hpp"
#include "../include/Client.hpp"
#include "../include/Ban.hpp"

#define MEMBER_MODE string("obv")
#define CHAN_MODE string("tma")

class Client;

#define CHAN_MODE_SIZE 4
enum chanmode {
	CHAN_TOPIC = 0, // t
	CHAN_MODERATE, // m
	CHAN_INVITE, // i
	CHAN_ANONYMOUS // a
};

#define MEMBER_MODE_SIZE 3
enum memberMode {
	MEM_CHANOP = 0, // o
	MEM_BAN, // b
	MEM_VOICE // v
};

typedef std::map< Client*, std::bitset< MEMBER_MODE_SIZE > >		map_pcli;	
typedef const std::map< Client*, std::bitset< MEMBER_MODE_SIZE > >	cst_map_pcli;

typedef std::bitset< CHAN_MODE_SIZE >								chan_mode;
typedef const std::bitset< CHAN_MODE_SIZE >							cst_chan_mode;

typedef std::list< Ban >											lst_ban;
typedef const std::list< Ban >										cst_lst_ban;

class Channel
{
	public:
		/* Coplien */
		Channel(const string &chanstring = "default");
		~Channel();
		Channel(const Channel& chan);
		Channel	&operator=(const Channel& rhs);

		const string 				&GetTopic() const;

		void						SetTopic(const string& params);

		bool						IsBanned(Client& client);
		bool						IsOperator(Client& client);
		bool						IsVoiced(Client& client);
		bool						IsOpTopicOnly() const;
		bool						IsModerated() const;
		bool						IsInvite() const;
		bool						IsAnon() const;

		string						GetOrigin(Client &client);

		/* Public Methods */
		void						SetChanMode(const char c, bool status);
		void						SetMemberMode(Client &client, const char c, bool status);

		int							joinChannel(Client& toAccept);
		void						leaveChannel(Client& toRemove);
		map_pcli::const_iterator	findUserIter(const string &toFind) const;
		map_pcli::iterator			findUserIter(const string &toFind);

		cst_map_pcli				&GetUsers() const;
		const string				&GetName() const;
		string						GetLstNickname() const;
		string						GetStrChanMode() const;
		string						GetStrUserMode(const Client &client) const;
		const string				&GetCtime() const;
		const string				&GetTopicStat() const;
		void						AddToBanList(const string &from, const string &toBan);
		void						AddToInviteList(const string &from, const string &toBan);
		void						RemoveFromBanList(const string &deBan);
		cst_lst_ban					GetBanList() const;
		Client						*GetCreator() const;

	private:
		class BanFinder {
			public:
				BanFinder(const std::string& deBan) : _deBan(deBan) {}

				bool operator()(const Ban& b) const {
					return b.GetMask().find(_deBan) != std::string::npos;
				}

			private:
				const std::string& _deBan;
		};

		string						_GetTime() const;
		Client						*_creator;
		map_pcli					_user;
		string						_chanstring;
		string						_topic;
		chan_mode					_mode;
		string						_ctime;
		string						_topicStat;
		lst_ban						_banList;
		lst_ban						_inviteList;
};

#endif
