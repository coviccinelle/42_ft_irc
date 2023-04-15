#ifndef CHANNELPARSER_HPP
#define CHANNELPARSER_HPP
#include "../utils/utils.hpp"

class ChannelParser
{
	public:
		ChannelParser(void);
		virtual ~ChannelParser(void);
		ChannelParser(ChannelParser const &src);
		ChannelParser	&operator=(ChannelParser const &rhs);
		
		string	channel;
		string	prefix;
		string	channelid;
		string	chanstring;
		string	suffix;
	private:
		void	Parse(const string &str);
		void	Debug() const;

		void	_Channel();
		void	_ChannelPrefix();
		void	_ChannelId();
		void	_ChannelSuffix();
		void	_ChannelString();
};

#endif
