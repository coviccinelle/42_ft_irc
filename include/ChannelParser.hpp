#ifndef CHANNELPARSER_HPP
#define CHANNELPARSER_HPP
#include "../utils/utils.hpp"

class ChannelParser
{
	public:
		ChannelParser(void);
		~ChannelParser(void);
		ChannelParser(ChannelParser const &src);
		ChannelParser	&operator=(ChannelParser const &rhs);
		
		void	Parse(const string &str);
		void	Debug() const;

		string	channel;
		string	prefix;
		string	channelid;
		string	chanstring;
		string	suffix;
	private:
		void					_Channel();
		void					_ChannelPrefix();
		void					_ChannelId();
		void					_ChannelSuffix();
		void					_ChannelString();
};

#endif
