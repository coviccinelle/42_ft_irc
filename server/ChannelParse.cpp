#include "../include/ChannelParse.hpp"

ChannelParse::ChannelParse(void) :
	channel(""),
	prefix(""),
	channelid(""),
	chanstring(""),
	suffix("")
{
	return ;
}

ChannelParse::~ChannelParse(void)
{
	return ;
}

ChannelParse::ChannelParse(ChannelParse const &src)
{
	channel = src.channel;
	prefix = src.prefix;
	channelid = src.channelid;
	chanstring = src.chanstring;
	suffix = src.suffix;

	return ;
}

ChannelParse &ChannelParse::operator=(ChannelParse const &rhs)
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

void ChannelParse::Debug() const
{
	std::cout << "===========[ DEBUG ChannelParse ]===========" << std::endl;
	std::cout << "Channel :[" << channel << "]" << std::endl;
	std::cout << "Prefix :[" << prefix << "]" << std::endl;
	std::cout << "Channelid :[" << channelid << "]" << std::endl;
	std::cout << "Chanstring :[" << chanstring << "]" << std::endl;
	std::cout << "Suffix :[" << suffix << "]" << std::endl;
}
