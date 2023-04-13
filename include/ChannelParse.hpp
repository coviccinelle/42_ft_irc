#ifndef CHANNELPARSE_HPP
#define CHANNELPARSE_HPP
#include "../utils/utils.hpp"

class ChannelParse
{
	public:
		ChannelParse(void);
		~ChannelParse(void);
		ChannelParse(ChannelParse const &src);
		ChannelParse	&operator=(ChannelParse const &rhs);
		
		void	Debug() const;

		string	channel;
		string	prefix;
		string	channelid;
		string	chanstring;
		string	suffix;
};

#endif
