#include "../include/Parser.hpp"

Parser::Parser(void)
{

	return ;
}

Parser::~Parser(void)
{

	return ;
}

Parser::Parser(Parser const &src)
{
	*this = src;

	return ;
}

Parser &Parser::operator=(Parser const &rhs)
{
	if (&rhs == this)
		return (*this);

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
	else if (isalpha(*it) && skip != letter) { return (true); }
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
//		std::cout << "[" << *_it << "]" << std::endl;
//		sleep(1);
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
	std::cout << "I'm Nickname" << std::endl;
	_Wrapper();
	if (_current != letter && _current != special)
		throw irc_error("parsing failed: _Nickname: letter or special expected", ERR_NICK);
	while (_current == letter || _current == digit || _current == special || _current == dash)
		_Wrapper();
}

//host = hostname /hostaddr
void	Parser::_Host()
{
	std::cout << "I'm host" << std::endl;
	_Wrapper();
	if (_current != digit && _current != letter)
		throw irc_error("parsing failed: _Host: letter or digit expected", ERR_HOST);
	while (_current != eoi)
	{
		_Wrapper();
		if (_current == space)
			return ;
		if (_current != digit &&
			_current != letter &&
			_current != dot &&
			_current != dash &&
			_current != colon)
			throw irc_error("parsing failed: _Host: digit or letter or dot or dash or colon expected", ERR_HOST);
	}
	if (_current == eoi)
		throw irc_error("parsing failed: _Host: eoi found", ERR_HOST);
}

void	Parser::_User()
{
	std::cout << "I'm user" << std::endl;
	std::string::iterator	start = _it + 1;
	_Wrapper();
	if (_current == eoi || _current == space || _current == at)
		throw irc_error("parsing failed: _User: eoi or space or at found", ERR_USER);
	while (_current != eoi)
	{
		_Wrapper();
		if (_current == space || _current == at)
		{
			_cmd.user = string(start, _it);
			return ;
		}
	}
	if (_current == eoi)
		throw irc_error("parsing failed: _User: eoi found", ERR_USER);
	_cmd.user = string(start, _it);
}

//prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
void Parser::_Prefix()
{
	string::iterator	start = _it + 1;
	std::cout << "I'm prefix" << std::endl;
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
	_cmd.prefix = string(start, _it);
}

void	Parser::_Command()
{
	std::cout << "I'm command" << std::endl;
	if (_current != letter && _current != digit)
		throw irc_error("parsing failed: _Command: letter or digit expected", ERR_COMMAND);
}

void	Parser::_Middle()
{
	_Wrapper();
	if (_current == colon || _current == space)
		throw irc_error("parsing failed: _Middle: colon or space found", ERR_MIDDLE);
	if (_current == eoi)
		return ;
	std::cout << "checking for valid targets" << std::endl;
}

void	Parser::_Param()
{
	std::cout << "I'm Param" << std::endl;
	_Wrapper();
	while (_current != eoi)
	{
		if (_current != space)
			throw irc_error("parsing failed: _Param: space expected", ERR_PARAM);
		_Middle();
		_Wrapper();
	}
}

void Parser::_Message()
{
	string::iterator	start = _it;
	_Wrapper();
	if (_current == colon)
	{
		_Prefix();
		if (_current != space)
			throw irc_error("parsing failed: _Message: space expected", ERR_MESSAGE);
		_Wrapper();
	}
	_Command();
	_Param();
	std::cout << "Done parsing Message ! OK" << std::endl;
	(void)start;
	_cmd.message = string(start, _it);
}

void Parser::_Wrapper()
{
	_current = _GetToken();
	_tokens.push_back(_current);
	/*
	if (_current == space)
		std::cout << "space" << std::endl;
	else if (_current == letter)
		std::cout << "letter" << std::endl;
	else if (_current == digit)
		std::cout << "digit" << std::endl;
	else if (_current == eoi)
		std::cout << "eoi" << std::endl;
	else if (_current == excl_mark)
		std::cout << "excl_mark" << std::endl;
	else if (_current == colon)
		std::cout << "colon" << std::endl;
	else if (_current == at)
		std::cout << "at" << std::endl;
	else if (_current == dash)
		std::cout << "dash" << std::endl;
	else if (_current == special)
		std::cout << "special" << std::endl;
	else if (_current == dot)
		std::cout << "dot" << std::endl;
	else if (_current == comma)
		std::cout << "comma" << std::endl;
	else if (_current == sha)
		std::cout << "sha" << std::endl;
	else if (_current == percent)
		std::cout << "percent" << std::endl;
	else if (_current == amp)
		std::cout << "amp" << std::endl;
	else if (_current == plus)
		std::cout << "plus" << std::endl;
	else
		std::cout << "error" << std::endl;
	*/
}

const std::vector< Token >	&Parser::Parse(const string &str)
{
	_input = str;
	_it = --_input.begin();

	_Message();
	std::cout << "Message :[" << _cmd.message << "]" << std::endl;
	std::cout << "Prefix :[" << _cmd.prefix << "]" << std::endl;
	std::cout << "User :[" << _cmd.user << "]" << std::endl;
	return (_tokens);
}
