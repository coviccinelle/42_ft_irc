#include "../include/CommandParser.hpp"

CommandParser::CommandParser() :
	Parser()
{
	return ;
}

CommandParser::~CommandParser(void)
{
	return ;
}

CommandParser::CommandParser(CommandParser const &src) :
	Parser()
{
	return ;
}

CommandParser &CommandParser::operator=(CommandParser const &rhs)
{
	if (&rhs == this)
		return (*this);

	return (*this);
}

void CommandParser::Debug() const
{
	std::cout << "===========[ DEBUG ]===========" << std::endl;
	std::cout << "Message :[" << GetCinfo()[message] << "]" << std::endl;
	std::cout << "Params :[" << GetCinfo()[params] << "]" << std::endl;
	std::cout << "Prefix :[" << GetCinfo()[prefix] << "]" << std::endl;
	std::cout << "User :[" << GetCinfo()[user] << "]" << std::endl;
	std::cout << "Host :[" << GetCinfo()[host] << "]" << std::endl;
	std::cout << "Nickname :[" << GetCinfo()[nick] << "]" << std::endl;
	std::cout << "Command :[" << GetCinfo()[command] << "]" << std::endl;
	std::cout << "Middle : " << std::endl;
	for (std::vector<string>::const_iterator it = GetMiddle().begin(); it != GetMiddle().end(); ++it)
		std::cout << " [" << *it << "]" << std::endl;
	std::cout << "Trailing : " << GetCinfo()[trailing] << std::endl;
	std::cout << "===============================" << std::endl;
}

void	CommandParser::Parse(const string &str)
{
	_input = str;
	_it = --_input.begin();
	_Message();

	return ;
}

void	CommandParser::ParseCommand(const string &str)
{
	Parse(str);
}

void	CommandParser::DebugCommand() const
{
	Debug();
}

void CommandParser::_Message()
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
	SetCommand(string(start, _it + 1));
	_Param();
	SetMessage(string(start, _it));
}

void	CommandParser::_Command()
{
	if (_current != letter && _current != digit)
		throw irc_error("parsing failed: _Command: letter or digit expected", ERR_COMMAND);
}

void	CommandParser::_Middle()
{
	string::iterator	start = _it + 1;
	_Wrapper();
	if (_current == space)
		throw irc_error("parsing failed: _Middle: space found", ERR_MIDDLE);
	if (_current == colon)
	{
		_Trailing();
		SetTrailing(string(start + 1, _it));
		return ;
	}
	while (_current != space && _current != eoi)
		_Wrapper();
	AddMiddle(string(start, _it));
}

void	CommandParser::_Nickname()
{
	std::string::iterator	start = _it + 1;
	_Wrapper();
	if (_current != letter && _current != special)
		throw irc_error("parsing failed: _Nickname: letter or special expected", ERR_NICK);
	while (_current == letter || _current == digit || _current == special || _current == dash)
		_Wrapper();
	SetNickname(string(start, _it));
}

void	CommandParser::_Host()
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
			SetHost(string(start, _it));
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
	SetHost(string(start, _it));
}

void	CommandParser::_User()
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
			SetUser(string(start, _it));
			return ;
		}
	}
	if (_current == eoi)
		throw irc_error("parsing failed: _User: eoi found", ERR_USER);
	SetUser(string(start, _it));
}

void CommandParser::_Prefix()
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
	SetPrefix(string(start, _it));
}


void	CommandParser::_Trailing()
{
	while (_current != eoi)
		_Wrapper();
}

void	CommandParser::_Param()
{
	_Wrapper();
	if (_current == eoi)
		return ;
	if (_current != space)
		throw irc_error("parsing failed: _Param: space expected", ERR_PARAM);
	std::string::iterator start = _it + 1;
	while (_current != eoi)
	{
		if (_current != space)
			throw irc_error("parsing failed: _Param: space expected", ERR_PARAM);
		_Middle();
	}
	SetParams(string(start, _it));
}

void	CommandParser::SetMessage(const string &s)
{
	_cinfo[message] = s;
}

void	CommandParser::SetPrefix(const string &s)
{
	_cinfo[prefix] = s;
}

void	CommandParser::SetParams(const string &s)
{
	_cinfo[params] = s;
}

void	CommandParser::SetUser(const string &s)
{
	_cinfo[user] = s;
}

void	CommandParser::SetHost(const string &s)
{
	_cinfo[host] = s;
}

void	CommandParser::SetNickname(const string &s)
{
	_cinfo[nick] = s;
}

void	CommandParser::SetCommand(const string &s)
{
	_cinfo[command] = s;
}

void	CommandParser::SetTrailing(const string &s)
{
	_cinfo[trailing] = s;
}
