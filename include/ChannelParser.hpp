#ifndef CHANNELPARSER_HPP
#define CHANNELPARSER_HPP
#include "../utils/utils.hpp"
#include "../include/Parser.hpp"

#define INF_CHAN_SIZE 5
enum InfoChannel {
	chan = 0,
	chanprefix,
	chanid,
	chanstring,
	chansuffix
};

class ChannelParser : public Parser
{
	public:
		ChannelParser(void);
		virtual ~ChannelParser(void);
		ChannelParser(ChannelParser const &src);
		ChannelParser	&operator=(ChannelParser const &rhs);
		
		void	ParseChannel(const string &str);
		void	DebugChannel() const;

		cst_vec_str	&GetChannels() const;

	private:
		void	Parse(const string &str);
		void	Debug() const;

		void	SetChannel(const string &s);
		void	SetPrefix(const string &s);
		void	SetId(const string &s);
		void	SetChanstring(const string &s);
		void	SetSuffix(const string &s);

		void	_Channel();
		void	_ChannelPrefix();
		void	_ChannelId();
		void	_ChannelSuffix();
		void	_ChannelString();

		vec_str	_channels;
};

#endif
