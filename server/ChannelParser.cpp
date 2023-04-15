#include "../include/ChannelParser.hpp"

ChannelParser::ChannelParser(void) :
	channel(""),
	prefix(""),
	channelid(""),
	chanstring(""),
	suffix("")
{
	return ;
}

ChannelParser::~ChannelParser(void)
{
	return ;
}

ChannelParser::ChannelParser(ChannelParser const &src)
{
	channel = src.channel;
	prefix = src.prefix;
	channelid = src.channelid;
	chanstring = src.chanstring;
	suffix = src.suffix;

	return ;
}

ChannelParser &ChannelParser::operator=(ChannelParser const &rhs)
{
	if (&rhs == this)
		return (*this);

	channel = rhs.channel;
	prefix = rhs.prefix;
	channelid = rhs.channelid;
	chanstring = rhs.chanstring;
	suffix = rhs.suffix;

	return (*this);
}

void ChannelParser::Debug() const
{
	std::cout << "===========[ DEBUG ChannelParser ]===========" << std::endl;
	std::cout << "Channel :[" << channel << "]" << std::endl;
	std::cout << "Prefix :[" << prefix << "]" << std::endl;
	std::cout << "Channelid :[" << channelid << "]" << std::endl;
	std::cout << "Chanstring :[" << chanstring << "]" << std::endl;
	std::cout << "Suffix :[" << suffix << "]" << std::endl;
	std::cout << "===========================" << std::endl;
}

void	ChannelParser::Parse(const string &str)
{
	_input = str;
	_it = --_input.begin();
	_Channel();
}

void	ChannelParser::_ChannelId()
{
	string::iterator	start = ++_it;
	for (int i = 0; i < 5; i++)
	{
		if (_it == _input.end() &&
			isdigit(*_it) == false &&
			isupper(*_it) == false)
			throw irc_error("parsing failed: _ChannelId: channelid expected", ERR_CHANNELID);
		++_it;
	}
	_chan->channelid = string(start, _it--);
}

void	ChannelParser::_ChannelPrefix()
{
	string::iterator	start = _it + 1;
	_Wrapper();
	if (_current != sha &&
		_current != plus &&
		_current != excl_mark &&
		_current != amp)
		throw irc_error("parsing failed: _ChannelPrefix: sha or plus or excl_mark or amp expected", ERR_CHANNELPREFIX);
	_chan->prefix = string(start, _it + 1);
	if (_current == excl_mark)
		_ChannelId();
}

void	ChannelParser::_ChannelSuffix()
{
	string::iterator	start = _it + 1;
	_Wrapper();
	if (_current == eoi)
		throw irc_error("parsing failed: _ChannelSuffix: chanstring expected", ERR_CHANNELSUFFIX);
	while (_current != eoi)
	{
		_Wrapper();
		if (_current == colon)
			throw irc_error("parsing failed: _ChannelSuffix: chanstring expected", ERR_CHANNELSUFFIX);
	}
	_chan->suffix = string(start, _it);
}

void ChannelParser::_ChannelString()
{
	if (_current == eoi)
		throw irc_error("parsing failed: _Channel: chanstring expected", ERR_CHANNEL);
	string::iterator	start = _it + 1;
	while (_current != eoi)
	{
		_Wrapper();
		if (_current == colon)
		{
			_chan->chanstring = string(start, _it);
			return _ChannelSuffix();
		}
	}
	_chan->chanstring = string(start, _it);
}

void	ChannelParser::_Channel()
{
	string::iterator	start = _it + 1;
	_ChannelPrefix();
	_ChannelString();
	_chan->channel = string(start, _it);
}
