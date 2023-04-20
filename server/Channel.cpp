#include "../include/Channel.hpp"

Channel::Channel(const string &chanstring) : 
	_creator(NULL),
	_user(),
	_chanstring(chanstring),
	_topic("Default Topic"),
	_ctime(""),
	_topicStat("")
{
	_ctime = _GetTime();
	std::cout << "New channel created on " << _ctime << std::endl;
}

Channel::~Channel()
{
}

Channel::Channel(const Channel &src) :
	_creator(src._creator),
	_user(src._user),
	_chanstring(src._chanstring),
	_topic(src._topic),
	_ctime(src._ctime),
	_topicStat(src._topicStat)
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
	_ctime = rhs._ctime;
	_topicStat = rhs._topicStat;

	return (*this);
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

bool	Channel::IsOperator(const Client& client) const
{
//	return (_user.find(&client)[MEMBER_MODE.find('o')]);
	(void)client;
	return (0);
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
		SetChanMode('t', true);
	}
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
		res = res + it->first->GetUinfo()[nickname];
		if (it != _user.end())
			res += " ";
	}
	return (res);
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

string Channel::_GetTime() const
{
	time_t secs = std::time(0);
	std::stringstream ss;
	ss << secs;
	return (ss.str());
}
