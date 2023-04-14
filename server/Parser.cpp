#include "../include/Parser.hpp"

Parser::Parser(void) : 
	_it(),
	_current(error),
	_tokens(),
	_cmd(new Command()),
	_chan(new ChannelParse())
{
	return ;
}

Parser::~Parser(void)
{
	delete _cmd;
	delete _chan;
	return ;
}

Parser::Parser(Parser const &src)
{
	_it = src._it;
	_current = src._current;
	_tokens = src._tokens;
	_input = src._input;
	_cmd = new Command(*src._cmd);
	_chan = new ChannelParse(*src._chan);

	return ;
}

Parser &Parser::operator=(Parser const &rhs)
{
	if (&rhs == this)
		return (*this);
	_it = rhs._it;
	_current = rhs._current;
	_tokens = rhs._tokens;
	_input = rhs._input;

	delete _cmd;
	delete _chan;
	_cmd = new Command(*rhs._cmd);
	_chan = new ChannelParse(*rhs._chan);

	return (*this);
}

bool	isspecial(string::const_iterator it)
{
	if ((*it >= '[' && *it <= '`') || (*it >= '{' && *it <= '~'))
		return (true);
	return (false);
}

bool is_fstate(string::const_iterator it, int skip)
{
	if (*it == ':' && skip != colon) return (true);
	else if (*it == '@' && skip != at) return (true);
	else if (*it == '!' && skip != excl_mark) return (true); 
	else if (*it == '-' && skip != dash) return (true);
	else if (*it == '.' && skip != dot) return (true);
	else if (*it == ',' && skip != comma) return (true);
	else if (*it == '#' && skip != sha) return (true);
	else if (*it == '%' && skip != percent) return (true);
	else if (*it == '&' && skip != amp) return (true);
	else if (*it == '+' && skip != plus) return (true);
	else if (*it == SPACE && skip != space) return (true);
	else if (isalpha(*it) && skip != letter) return (true);
	else if (isdigit(*it) && skip != digit)	return (true);
	else if (isspecial(it) && skip != special) return (true);
	return (false);
}

Token	Parser::_GetToken()
{
	int		state = 0;
	string	s;
	string::iterator end = _input.end();
	while (state >= 0 && state <= 4)
	{
		++_it;
		switch(state)
		{
			case 0:
				// first char of the new token
				if (_it == end) return (eoi);
				else if (*_it == ':') { return (colon); }
				else if (*_it == '@') { return (at); }
				else if (*_it == '!') { return (excl_mark); }
				else if (*_it == '-') { return (dash); }
				else if (*_it == '.') { return (dot); }
				else if (*_it == ',') { return (comma); }
				else if (*_it == '#') { return (sha); }
				else if (*_it == '%') { return (percent); }
				else if (*_it == '&') { return (amp); }
				else if (*_it == '+') { return (plus); }
				else if (*_it == SPACE) { return (space); }
				else if (isalpha(*_it)) state = 1;
				else if (isdigit(*_it)) state = 2;
				else if (isspecial(_it)) state = 3;
				else state = 4;
				break ;
			case 1:
				// letter
				if (isalpha(*_it)) { state = 1;}
				else if (_it == end || is_fstate(_it, letter)) { --_it; return (letter); }
				else state = 4;
				break ;
			case 2:
				// digit
				if (isdigit(*_it)) { state = 2; }
				else if (_it == end || is_fstate(_it, digit)) { --_it; return (digit); }
				else state =  4;
				break ;
			case 3:
				// special
				if (isspecial(_it)) { state = 3; }
				else if (_it == end || is_fstate(_it, special)) { --_it; return (special); }
				else state = 4;
				break ;
			case 4:
				// nospcl
				if (_it == end || is_fstate(_it, nospcl)) { --_it; return (nospcl); }
				else state = 4;
				break ;
		}
	}
	return (eoi);
}

void	Parser::_Nickname()
{
	std::string::iterator	start = _it + 1;
	_Wrapper();
	if (_current != letter && _current != special)
		throw irc_error("parsing failed: _Nickname: letter or special expected", ERR_NICK);
	while (_current == letter || _current == digit || _current == special || _current == dash)
		_Wrapper();
	_cmd->nickname = string(start, _it);
}

void	Parser::_Host()
{
	std::string::iterator	start = _it + 1;
	_Wrapper();
	if (_current != digit && _current != letter)
		throw irc_error("parsing failed: _Host: letter or digit expected", ERR_HOST);
	while (_current != eoi)
	{
		_Wrapper();
		if (_current == space)
		{
			_cmd->host = string(start, _it);
			return ;
		}
		if (_current != digit &&
			_current != letter &&
			_current != dot &&
			_current != dash &&
			_current != colon)
			throw irc_error("parsing failed: _Host: digit or letter or dot or dash or colon expected", ERR_HOST);
	}
	if (_current == eoi)
		throw irc_error("parsing failed: _Host: eoi found", ERR_HOST);
	_cmd->host = string(start, _it);
}

void	Parser::_User()
{
	std::string::iterator	start = _it + 1;
	_Wrapper();
	if (_current == eoi || _current == space || _current == at)
		throw irc_error("parsing failed: _User: eoi or space or at found", ERR_USER);
	while (_current != eoi)
	{
		_Wrapper();
		if (_current == space || _current == at)
		{
			_cmd->user = string(start, _it);
			return ;
		}
	}
	if (_current == eoi)
		throw irc_error("parsing failed: _User: eoi found", ERR_USER);
	_cmd->user = string(start, _it);
}

void Parser::_Prefix()
{
	string::iterator	start = _it + 1;
	_Nickname();
	if (_current == excl_mark)
	{
		_User();
		if (_current != at)
			throw irc_error("parsing failed: _Prefix: at expected", ERR_PREFIX);
		_Host();
	}
	else if (_current == at)
		_Host();
	_cmd->prefix = string(start, _it);
}

void	Parser::_Command()
{
	if (_current != letter && _current != digit)
		throw irc_error("parsing failed: _Command: letter or digit expected", ERR_COMMAND);
}

void	Parser::_Middle()
{
	string::iterator	start = _it + 1;
	_Wrapper();
	if (_current == space)
		throw irc_error("parsing failed: _Middle: space found", ERR_MIDDLE);
	if (_current == colon)
	{
		_Trailing();
		_cmd->trailing = string(start + 1, _it);
		return ;
	}
	while (_current != space && _current != eoi)
		_Wrapper();
	_cmd->middle.push_back(string(start, _it));
}

void	Parser::_Target()
{
	std::string::iterator start = _it + 1;
	std::string::iterator start2 = _it + 1;
	_Wrapper();
	if (_current == colon)
	{
		_Trailing();
		_cmd->trailing = string(start + 1, _it);
		return ;
	}
	while (1)
	{
		if (_current == colon)
			throw irc_error("parsing failed: _Target: colon found", ERR_MIDDLE);
		if (_current == comma)
		{
			_cmd->target.push_back(string(start, _it));
			start = _it + 1;
		}
		else if (_current == space || _current == eoi)
		{
			_cmd->target.push_back(string(start, _it));
			_cmd->middle.push_back(string(start2, _it));
			return ;
		}
		_Wrapper();
	}
}

void	Parser::_Trailing()
{
	while (_current != eoi)
		_Wrapper();
}

void	Parser::_Param()
{
	_Wrapper();
	if (_current == eoi)
		return ;
	if (_current != space)
		throw irc_error("parsing failed: _Param: space expected", ERR_PARAM);
	std::string::iterator start = _it + 1;
	if (_current != eoi &&
		_cmd->command != "JOIN" &&
		_cmd->command != "PART")
		_Target();
	while (_current != eoi)
	{
		if (_current != space)
			throw irc_error("parsing failed: _Param: space expected", ERR_PARAM);
		_Middle();
	}
	_cmd->params = string(start, _it);
}

void Parser::_Message()
{
	string::iterator	start = _input.begin();
	_Wrapper();
	if (_current == colon)
	{
		_Prefix();
		if (_current != space)
			throw irc_error("parsing failed: _Message: space expected", ERR_MESSAGE);
		start = _it + 1;
		_Wrapper();
	}
	_Command();
	_cmd->command = string(start, _it + 1);
	_Param();
	_cmd->message = string(start, _it);
}

void Parser::_Wrapper()
{
	_current = _GetToken();
	_tokens.push_back(_current);
}

void	Parser::_InitCmd()
{
	delete _cmd;
	_cmd = new Command();
}

void	Parser::_InitChan()
{
	delete _chan;
	_chan = new ChannelParse();
}

const std::vector< Token >	&Parser::Tokens() const
{
	return (_tokens);
}

void	Parser::Parse(const string &str)
{
	std::cout << "str here " << str << std::endl;
	_InitCmd();
	_input = str;
	_it = --_input.begin();
	_Message();

	return ;
}

const Command	&Parser::GetCommand() const
{
	return (*_cmd);
}

const ChannelParse 	&Parser::GetChan() const
{
	return (*_chan);
}

bool	Parser::isValidNick(const string &str)
{
	if (str.size() < 1 || str.size() > 9)
		return (false);
	_input = str;
	_it = --_input.begin();

	_Wrapper();
	if (_current != letter && _current != special)
		return (false);
	while (_current != eoi)
	{
		if (_current != letter &&
			_current != digit &&
			_current != special &&
			_current != dash)
			return (false);
		_Wrapper();
	}
	return (true);
}

bool	Parser::ParseUserMode(const string &str)
{
	string::const_iterator it = str.begin();
	if (*it != '+' && *it != '-')
		return (false);
	while (it != str.end())
	{
		if (*it != '+' &&
			*it != '-' &&
			USER_MODE.find(*it) == std::string::npos)
			return (false);
		++it;
	}
	return (true);
}

void	Parser::_ChannelId()
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
	_chan->channelid = string(start, _it);
}

void	Parser::_ChannelPrefix()
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

void	Parser::_ChannelSuffix()
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

void Parser::_ChannelString()
{
	if (_current == eoi)
		throw irc_error("parsing failed: _Channel: chanstring expected", ERR_CHANNEL);
	string::iterator	start = _it;
	while (_current != eoi)
	{
		_Wrapper();
		if (_current == colon)
		{
			_chan->chanstring = string(start, _it - 1);
			return _ChannelSuffix();
		}
	}
	_chan->chanstring = string(start, _it);
}

void	Parser::_Channel()
{
	string::iterator	start = _it + 1;
	_ChannelPrefix();
	_ChannelString();
	_chan->channel = string(start, _it);
}

void	Parser::ParseJoin(const string &str)
{
	_InitChan();
	_input = str;
	_it = --_input.begin();
	_Channel();
}
