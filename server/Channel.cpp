#include "../include/Channel.hpp"

Channel::Channel(const string &chanstring) : 
	_creator(NULL),
	_user(),
	_chanstring(chanstring)
{
}

Channel::~Channel()
{
}

Channel::Channel(const Channel &src) :
	_creator(src._creator),
	_user(src._user),
	_chanstring(src._chanstring)
{
}

Channel&	Channel::operator=(const Channel& rhs)
{
	if (this == &rhs)
		return (*this);

	_creator = rhs._creator;
	_user = rhs._user;
	_chanstring = rhs._chanstring;

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
	(void)client;
	//return (_mode[USER_MODE.find('o')]);
	std::cout << "under construction" << std::endl;
	return (false);
}

const string &Channel::GetTopic() const
{
	return (_topic);
}

int	Channel::joinChannel(Client& toAccept)
{
	if (_creator == NULL)
	{
		_creator = &toAccept;
		SetMemberMode(toAccept, 'O', true);
		SetMemberMode(toAccept, 'o', true);
		SetChanMode('b', true);
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
