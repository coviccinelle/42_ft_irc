#include "../include/Channel.hpp"

// Tools
string _GetTime()
{
	time_t secs = std::time(0);
	std::stringstream ss;
	ss << secs;
	return (ss.str());
}

Channel::Channel(const string &chanstring) : 
	_creator(NULL),
	_user(),
	_chanstring(chanstring),
	_topic("Default Topic"),
	_mode(),
	_ctime(_GetTime()),
	_topicStat(""),
	_banList(),
	_inviteList()
{
	_mode.set(CHAN_TOPIC, false);
	_mode.set(CHAN_MODERATE, false);
	_mode.set(CHAN_INVITE, false);
	_mode.set(CHAN_ANONYMOUS, false);
}

Channel::~Channel()
{
}

Channel::Channel(const Channel &src) :
	_creator(src._creator),
	_user(src._user),
	_chanstring(src._chanstring),
	_topic(src._topic),
	_mode(src._mode),
	_ctime(src._ctime),
	_topicStat(src._topicStat),
	_banList(src._banList),
	_inviteList(src._inviteList)
{
}

Channel&	Channel::operator=(const Channel& rhs)
{
	if (this == &rhs)
		return (*this);

	_creator = rhs._creator;
	_user = rhs._user;
	_chanstring = rhs._chanstring;
	_topic = rhs._topic;
	_mode = rhs._mode;
	_ctime = rhs._ctime;
	_topicStat = rhs._topicStat;
	_banList = rhs._banList;
	_inviteList = rhs._inviteList;

	return (*this);
}

void	Channel::AddToBanList(const string &from, const string &toBan)
{
	_banList.push_back(Ban(toBan, from, _GetTime()));
}

void	Channel::AddToInviteList(const string &from, const string &toInvite)
{
	_inviteList.push_back(Ban(toInvite, from, _GetTime()));
}

void	Channel::RemoveFromBanList(const string &deBan)
{
	_banList.remove_if(BanFinder(deBan));
}

cst_lst_ban	Channel::GetBanList() const
{
	return (_banList);
}

void	Channel::SetMemberMode(Client &client, const char c, bool status)
{
	_user[&client].set(MEMBER_MODE.find(c), status);
}

void	Channel::SetChanMode(const char c, bool status)
{
	_mode.set(CHAN_MODE.find(c), status);
}

void	Channel::SetTopic(const string& name)
{
	_topic = name;
}

bool	Channel::IsOperator(Client& client)
{
	bool status = false;

	if (_user.find(&client) != _user.end())
		status = _user.find(&client)->second[MEM_CHANOP];
	if (status == false)
		status = client.isOperator();
	return (status);
}

bool	Channel::IsVoiced(Client& client)
{
	return (_user.find(&client)->second[MEM_VOICE]);
}

bool	Channel::IsInvited(Client& client)
{
	bool status = false;
	if (IsInvite() == false)
		status = true;
	for (lst_ban::const_iterator it = _inviteList.begin(); it != _inviteList.end(); ++it)
	{
		if (it->GetMask().find(client.GetUinfo()[nickname]) != std::string::npos)
			status = true;
	}
	return (status);
}

bool	Channel::IsInvite() const
{
	return (_mode[CHAN_INVITE]);
}

bool	Channel::IsModerated() const
{
	return (_mode[CHAN_MODERATE]);
}

bool Channel::IsAnon() const
{
	return (_mode[CHAN_ANONYMOUS]);
}

bool	Channel::IsOpTopicOnly() const
{
	return (_mode[CHAN_TOPIC]);
}

bool	Channel::IsBanned(Client& client)
{
	bool isBan = false;
	for (lst_ban::const_iterator it = _banList.begin(); it != _banList.end(); ++it)
	{
		if (it->GetMask().find(client.GetUinfo()[nickname]) != std::string::npos)
			isBan = true;
	}
	return (isBan);
}

string Channel::GetOrigin(const Client &client) const
{
	string ret;
	if (IsAnon())
		ret = "anonymous!anonymous@anonymous";
	else 
		ret = client.GetPrefix();
	return (ret);
}

string Channel::GetNickname(const Client &client) const
{
	string ret;
	if (IsAnon())
		ret = "anonymous";
	else 
		ret = client.GetUinfo()[nickname];
	return (ret);
}

const string &Channel::GetTopic() const
{
	return (_topic);
}

int	Channel::joinChannel(Client& toAccept)
{
	if (_creator == NULL)
	{
		_topicStat = toAccept.GetPrefix() + " " + _GetTime();
		_creator = &toAccept;
		SetMemberMode(toAccept, 'o', true);
	}
	else if (&toAccept == _creator)
		SetMemberMode(toAccept, 'o', true);
	_user.insert(std::make_pair(&toAccept, 0));
	toAccept.RegisterChannel(*this);

	return (0);
}

void	Channel::leaveChannel(Client& toRemove)
{
	toRemove.DeregisterChannel(*this);
	_user.erase(&toRemove);

	return ;
}

const string	&Channel::GetName() const
{
	return (_chanstring);
}

cst_map_pcli	&Channel::GetUsers() const
{
	return (_user);
}

string	Channel::GetLstNickname() const
{
	string res("");
	for (map_pcli::const_iterator it = _user.begin(); it != _user.end(); ++it)
	{
		if (it->second[MEM_CHANOP])
			res += "@";
		res += GetNickname(*(it->first));
		if (it != _user.end())
			res += " ";
	}
	return (res);
}

map_pcli::iterator	Channel::findUserIter(const string &toFind)
{
	for (map_pcli::iterator it = _user.begin(); it != _user.end(); ++it)
	{
		if (it->first->GetUinfo()[nickname] == toFind)
			return (it);
	}
	return (_user.end());
}

map_pcli::const_iterator	Channel::findUserIter(const string &toFind) const
{
	for (map_pcli::const_iterator it = _user.begin(); it != _user.end(); ++it)
	{
		if (it->first->GetUinfo()[nickname] == toFind)
			return (it);
	}
	return (_user.end());
}

string	Channel::GetStrChanMode() const
{
	string res;

	for (int i = 0; i < CHAN_MODE_SIZE; ++i)
	{
		if (_mode[i])
			res += CHAN_MODE[i];
	}
	return (res);
}

string	Channel::GetStrUserMode(const Client &client) const 
{
	string res;

	for (int i = 0; i < MEMBER_MODE_SIZE; ++i)
	{
		if (_user.find(const_cast< Client * >(&client))->second[i])
			res += MEMBER_MODE[i];
	}
	return (res);
}

const string	&Channel::GetCtime() const
{
	return (_ctime);
}

const string	&Channel::GetTopicStat() const
{
	return (_topicStat);
}

Client	*Channel::GetCreator() const
{
	return (_creator);
}
