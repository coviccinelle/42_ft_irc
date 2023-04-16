#include "../include/ChannelParser.hpp"

ChannelParser::ChannelParser(void) :
	Parser(),
	_channels()
{
	return ;
}

ChannelParser::~ChannelParser(void)
{
	return ;
}

ChannelParser::ChannelParser(ChannelParser const &src) :
	Parser(src),
	_channels()
{
	return ;
}

ChannelParser &ChannelParser::operator=(ChannelParser const &rhs)
{
	if (&rhs == this)
		return (*this);

	_channels = rhs._channels;

	return (*this);
}

void ChannelParser::Debug() const
{
//	std::cout << "===========[ DEBUG ChannelParser ]===========" << std::endl;
//	std::cout << "Channel :[" << GetChannels()[chan] << "]" << std::endl;
//	std::cout << "Prefix :[" << GetChannels()[chanprefix] << "]" << std::endl;
//	std::cout << "Channelid :[" << GetChannels()[chanid] << "]" << std::endl;
//	std::cout << "Chanstring :[" << GetChannels()[chanstring] << "]" << std::endl;
//	std::cout << "Suffix :[" << GetChannels()[chansuffix] << "]" << std::endl;
//	std::cout << "===========================" << std::endl;
}

void	ChannelParser::Parse(const string &str)
{
	_channels.push_back(vec_str(INF_CHAN_SIZE));
	_input = str;
	_it = --_input.begin();
	_Channel();
}

void	ChannelParser::ParseChannel(const string &str)
{
	Parse(str);
}

void	ChannelParser::DebugChannel() const
{
	Debug();
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
	SetId(string(start, _it--));
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
	SetPrefix(string(start, _it + 1));
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
	SetSuffix(string(start, _it));
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
			SetChanstring(string(start, _it));
			return _ChannelSuffix();
		}
	}
	SetChanstring(string(start, _it));
}

void	ChannelParser::_Channel()
{
	string::iterator	start = _it + 1;
	_ChannelPrefix();
	_ChannelString();
	SetChannel(string(start, _it));
}

void	ChannelParser::SetChannel(const string &s)
{
	_channels.back()[chan] = s;
}

void	ChannelParser::SetPrefix(const string &s)
{
	_channels.back()[chanprefix] = s;
}

void	ChannelParser::SetId(const string &s)
{
	_channels.back()[chanid] = s;
}

void	ChannelParser::SetChanstring(const string &s)
{
	_channels.back()[chanstring] = s;
}

void	ChannelParser::SetSuffix(const string &s)
{
	_channels.back()[chansuffix] = s;
}

cst_vec_vec_str	&ChannelParser::GetChannels() const
{
	return (_channels);
}
