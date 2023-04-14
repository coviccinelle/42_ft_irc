#include "../include/Channel.hpp"

Channel::Channel(void) : 
	_creator(NULL),
	_chanop(),
	_voice(),
	_user(),
	_invite(),
	_ban(),
	_exception(),
	_key(),
	_chanstring("toto"),
	_safe(false),
	_modeless(false),
	_mode(0),
	_size(0)
{
}

Channel::~Channel()
{
}

Channel::Channel(const Channel &src)
{
	_creator = src._creator;
	_chanop = src._chanop;
	_voice = src._voice;
	_user = src._user;
	_invite = src._invite;
	_ban = src._ban;
	_exception = src._exception;
	_key = src._key;
	_chanstring = src._chanstring;
	_safe = src._safe;
	_modeless = src._modeless;
	_mode = src._mode;
	_size = src._size;
}

Channel&	Channel::operator=(const Channel& rhs)
{
	if (this == &rhs)
		return (*this);

	_creator = rhs._creator;
	_chanop = rhs._chanop;
	_voice = rhs._voice;
	_user = rhs._user;
	_invite = rhs._invite;
	_ban = rhs._ban;
	_exception = rhs._exception;
	_key = rhs._key;
	_chanstring = rhs._chanstring;
	_safe = rhs._safe;
	_modeless = rhs._modeless;
	_mode = rhs._mode;
	_size = rhs._size;

	return (*this);
}

bool	Channel::_compareChanop(const Client& client) const
{
	for (std::list< Client* >::const_iterator it = _chanop.begin(); it != _chanop.end(); it++)
		if ((*it)->GetUinfo()[nickname] == client.GetUinfo()[nickname])
			return (true);
	return (false);
}

bool	Channel::_compareVoice(const Client& client) const
{
	for (std::list< Client* >::const_iterator it = _voice.begin(); it != _voice.end(); it++)
		if ((*it)->GetUinfo()[nickname] == client.GetUinfo()[nickname])
			return (true);
	return (false);
}

bool	Channel::_compareUser(const Client& client) const
{
	for (std::list< Client* >::const_iterator it = _user.begin(); it != _user.end(); it++)
		if ((*it)->GetUinfo()[nickname] == client.GetUinfo()[nickname])
			return (true);
	return (false);
}

// @brief:
//  Delete the flag to the _mode variable
//
// @params:
//  - mode: should be max 3 flag added via bitwise OR
void	Channel::_delMode(const int& mode)
{
	if (mode & CHAN_ANONYMOUS)
		_mode &= ~CHAN_ANONYMOUS;
	if (mode & CHAN_INVITEONLY)
		_mode &= ~CHAN_INVITEONLY;
	if (mode & CHAN_MODERATED)
		_mode &= ~CHAN_MODERATED;
	if (mode & CHAN_NOMESSAGE)
		_mode &= ~CHAN_NOMESSAGE;
	if (mode & CHAN_QUIET)
		_mode &= ~CHAN_QUIET;
	if (mode & CHAN_PRIVATE)
		_mode &= ~CHAN_PRIVATE;
	if (mode & CHAN_SECRET)
		_mode &= ~CHAN_SECRET;
	if (mode & CHAN_REOP)
		_mode &= ~CHAN_REOP;
	if (mode & CHAN_TOPIC)
		_mode &= ~CHAN_TOPIC;
	if (mode & CHAN_LIMIT)
		_mode &= ~CHAN_LIMIT;
	if (mode & CHAN_KEY)
		_mode &= ~CHAN_KEY;
	if (mode & CHAN_BAN)
		_mode &= ~CHAN_BAN;
	if (mode & CHAN_EXCEPTION)
		_mode &= ~CHAN_EXCEPTION;
}

// @brief:
//  Add the flag to the _mode variable
//
// @params:
//  - mode: should be max 3 new flag added via bitwise OR
void	Channel::_addMode(const int& mode)
{
	if (mode & CHAN_ANONYMOUS)
		_mode |= CHAN_ANONYMOUS;
	if (mode & CHAN_INVITEONLY)
		_mode |= CHAN_INVITEONLY;
	if (mode & CHAN_MODERATED)
		_mode |= CHAN_MODERATED;
	if (mode & CHAN_NOMESSAGE)
		_mode |= CHAN_NOMESSAGE;
	if (mode & CHAN_QUIET)
		_mode |= CHAN_QUIET;
	if (mode & CHAN_PRIVATE)
		_mode |= CHAN_PRIVATE;
	if (mode & CHAN_SECRET)
		_mode |= CHAN_SECRET;
	if (mode & CHAN_REOP)
		_mode |= CHAN_REOP;
	if (mode & CHAN_TOPIC)
		_mode |= CHAN_TOPIC;
	if (mode & CHAN_LIMIT)
		_mode |= CHAN_LIMIT;
	if (mode & CHAN_KEY)
		_mode |= CHAN_KEY;
	if (mode & CHAN_BAN)
		_mode |= CHAN_BAN;
	if (mode & CHAN_EXCEPTION)
		_mode |= CHAN_EXCEPTION;
}


// TO CHANGE:
// 	Better error handling here nothing is handle.
//
// @params:
// - modif: char '-' or '+'
// - mode: flag 'enum channelMode'
void	Channel::_setChanMode(const char& modif, const int& mode)
{
	if (modif == '-')
		_delMode(mode);
	else if (modif == '+')
		_addMode(mode);
}

// TO CHANGE:
// 	Better error handling here nothing is handle.
// 	Better way to handle parameters and flags at the same time.
//
// @params:
// - modif: char either '-' or '+'
// - mode: bitwise OR that containts channelMode enum to know which mode to add or delete
// - params: parameter for 'k' or 'l'
void	Channel::_setChanMode(const char& modif, const int& mode, const string& params)
{
	if (modif == '+' && mode & CHAN_LIMIT)
		_size = std::atoi(params.c_str());
	if (modif == '+' && mode & CHAN_KEY)
		_key = params;
	if (modif == '-' && mode & CHAN_KEY)
		_key = "";

	if (modif == '-')
		_delMode(mode);
	else if (modif == '+')
		_addMode(mode);

}

// TO CHANGE:
// 	I need to change the way message are send,
// 	use of right format is needed (nickname!username@hostname)
//
// @params:
//  - msg: message to send to other client present in channel.
//  - client: client that is currently sending the message to channel.
//void	Channel::_sendToAll(const string& msg, const Client& client) const
//
//	for (std::list<Client>::iterator it = _user.begin(); it != _user.end(); it++)
//		if (client.GetUinfo()[nickname] != it->GetUinfo()[nickname])
//			it->SendData(msg);
//	for (std::list<Client>::iterator it = _chanop.begin(); it != _chanop.end(); it++)
//		if (client.GetUinfo()[nickname] != it->GetUinfo()[nickname])
//			it->SendData(msg);
//	for (std::list<Client>::iterator it = _voice.begin(); it != _voice.end(); it++)
//		if (client.GetUinfo()[nickname] != it->GetUinfo()[nickname])
//			it->SendData(msg);
//	if (_creator != NULL && client.GetUinfo()[nickname] != _creator)
//		_creator.SendData(msg);
//}

//// @params:
//// - prefix: char prefix of the chanstring
//int	Channel::_validPrefix(const char& prefix) const
//{
//	if (prefix == '&'
//		|| prefix == '#'
//		|| prefix == '+'
//		|| prefix == '!')
//		return (1);
//	return (0);
//}

// TO CHANGE:
// 	Proper error handling
//
// @params:
// - mode: use flag 'enum memberMode' to delete specific member status to user
// - name: name of the suposed user to be deleted to specific member status
//
// @info:
// 	Return value doens't exist those are placeholder for the moment
//int	Channel::_delUserStatus(const int& mode, const string& name)
//{
//	if (name.empty())
//		return (ERR_SOMETHING);
//
//	if (mode & MEM_VOICE)
//	{
//		Client&	ret = _findVoice(name);
//		if (&ret == &(*(_voice.end())))
//			return (ERR_SOMETHING);
//		_voice.remove(ret);
//	}
//	if (mode & MEM_CHANOP)
//	{
//		Client&	ret = _findChanop(name);
//		if (&ret == &(*(_chanop.end())))
//			return (ERR_SOMETHING);
//		_chanop.remove(ret);
//	}
//	return (RPL_SOMETHING);
//}

//// TO CHANGE:
//// 	Proper error handling
////
//// @params:
//// - mode: use flag 'enum memberMode' to add specific member status to user
//// - name: name of the suposed user to be added to specific member status
////
//// @info:
//// 	Return value doens't exist those are placeholder for the moment
//int	Channel::_addUserStatus(const int& mode, const string& name)
//{
//	Client&	ret = name.empty() == false ? _findUser(name) : *(_user.end());
//
//	if (&ret == &(*(_user.end())))
//		return (ERR_SOMETHING);
//
//	if (mode & MEM_VOICE && !_compareVoice(name))
//	{
//		_voice.push_back(ret);
//		_user.remove(ret); // Remove from _user list, so we have the specific number of user status
//
//		/* Check if flag +l is set to channel if so change update the _size,
//		 * which is the current size of _user list. */
//		if (_mode & CHAN_LIMIT)
//			_size = _user.size();
//	}
//	if (mode & MEM_CHANOP && !_compareChanop(name))
//	{
//		_chanop.push_back(ret);
//		_user.remove(ret);
//		if (_mode & CHAN_LIMIT)
//			_size = _user.size();
//	}
//	return (RPL_SOMETHING);
//}
//
///* Public Methods */
//
//// TO CHANGE:
//// 	Need to add anonymous send
////
//// @params:
//// - msg: message to send to channel
//// - clientSend: client that is sending the message
//void	Channel::sendMessage(const string& msg, const Client& clientSend) const
//{
//	// Moderated channel, only _voice, _chanop, _creator can talk in those channel
//	// _user receive the message but cannot send.
//	if ((_mode & CHAN_MODERATED) && (_compareVoice(clientSend)
//			|| _compareChanop(clientSend)
//			|| _compareCreator(clientSend)))
//	{
//		_sendToAll(msg, clientSend);
//		return;
//	}
//	if (!(_mode & CHAN_MODERATED))
//		_sendToAll(msg, clientSend);
//}
//
//// TO CHANGE:
//// 	Error handling might change in the future
////
//// @params:
//// - modif: char '-' or '+' that is passed before the mode
//// - mode: check enum should be CHAN_*
//// - clientSend: client who send the command
//// - params="": default to empty, if params is needed for the specific mode.
////
//// @info:
//// 	The 'enum channelMode' is the only one that should be used for @params.mode.
//// 	Return value are placeholder for the moment.
//int	Channel::setChanMode(char modif, int mode, const Client& clientSend, const string& params="")
//{
//	string	msg_err;
//
//	// check if '+' prefix as been given when creating this channel
//	if (_modeless)
//	{
//		msg_err = _chanstring + ":Channel doesn't support modes"
//
//		clientSend.SendData(msg_err);
//		return (ERR_NOCHANMODES);
//	}
//	if (!_compareChanop(clientSend) && !_compareCreator(clientSend))
//	{
//		msg_err = _chanstring + ":You're not channel operator"
//
//		clientSend.SendData(msg_err);
//		return (ERR_CHANOPRIVSNEEDED);
//	}
//
//	if (params.empty())
//		_setChanMode(modif, mode);
//	else
//		_setChanMode(modif, mode, params);
//}
//
//// TO CHANGE:
//// 	Error handling might change in the future
////
//// @params:
//// - modif: char '-' or '+' that is passed before the mode
//// - mode: check enum should be MEM_*
//// - clientSend: client who send the command
//// - params="": default to empty, if params is needed for the specific mode.
////
//// @info:
//// 	The 'enum memberMode' is the only one that should be used for @params.mode.
//// 	Return value are placeholder for the moment.
//int	Channel::setMemberStatus(char modif, int mode, const Client& clientSend, const string& params="")
//{
//	string	msg_err;
//
//	// check if '+' prefix as been given when creating this channel
//	if (_modeless)
//	{
//		msg_err = _chanstring + ":Channel doesn't support modes"
//
//		clientSend.SendData(msg_err);
//		return (ERR_NOCHANMODES);
//	}
//	if (!_compareChanop(clientSend) && !_compareCreator(clientSend))
//	{
//		msg_err = _chanstring + ":You're not channel operator"
//
//		clientSend.SendData(msg_err);
//		return (ERR_CHANOPRIVSNEEDED);
//	}
//	if (modif == '+')
//		_addUserStatus(mode, params); // return a value for if failure or not
//	else if (modif == '-')
//		_delUserStatus(mode, params); // same as above
//
//	return (RPL_SOMETHING);
//}

// TO CHANGE:
// 	Better handle of ban list and exception list for joining channel
//
//	@params:
//	- toAccept: Client that want to join the channel
int	Channel::joinChannel(Client& toAccept)
{
	// Need to change the behavior of both _compare function to work for wildcards, username and hostname.
	// ATM just check nickname.
//	if (_compareBan(toAccept) && !_compareException(toAccept))
//		return (-1);

	_user.push_back(&toAccept);
	toAccept.RegisterChannel(*this);
	return (0);
}

lst_iterator	Channel::_findUserIter(const string& name)
{
	for (lst_iterator it = _user.begin(); it != _user.end(); ++it)
	{
		if ((*it)->GetUinfo()[nickname] == name)
			return (it);
	}
	return (_user.end());
}

//	TO CHANGE:
//	Response and error handling.
//
//	@params:
//	- toAccept: Client that want to leave the channel
int	Channel::leaveChannel(Client& toAccept)
{
	lst_iterator it = _findUserIter(toAccept.GetUinfo()[nickname]);
	if (it == _user.end())
		return (-1);

	toAccept.DeregisterChannel(*this);
	_user.remove(*it);
	return (RPL_SOMETHING);
}


const string	&Channel::GetName() const
{
	return (_chanstring);
}
