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
		//std::cout << "[" << *_it << "]" << std::endl;
		//sleep(1);
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
	_Wrapper();
	std::cout << "I'm Nickname" << std::endl;
	if (_current != letter && _current != special)
		throw ;
	while (_current == letter || _current == digit || _current == special || _current == dash)
		_Wrapper();
}

//host = hostname /hostaddr
void	Parser::_Host()
{
		_Wrapper();
}

void	Parser::_User()
{
		_Wrapper();
}

//prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
void Parser::_Prefix()
{
	std::cout << "I'm prefix" << std::endl;
	_Nickname();
	if (_current == excl_mark)
		_User();
	if (_current == at)
		_Host();
}

void	Parser::_Command()
{
	std::cout << "I'm command" << std::endl;
}

void	Parser::_Param()
{
	std::cout << "I'm Param" << std::endl;
	if (_current != space)
		throw ;
	_Wrapper();
	if (_current != nospcl)
		throw ;
}

void Parser::_Message()
{
	_Wrapper();
	if (_current == colon)
	{
		_Prefix();
		if (_current != space)
			throw ;
	}
	else if (_current == letter || _current == digit)
	{
		_Command();
		_Wrapper();
		_Param();
		_Wrapper();
		if (_current != eoi)
			throw;
	}
}

void Parser::_Wrapper()
{
	_current = _GetToken();
	_tokens.push_back(_current);
	/*
	if (_current == space)
		std::cout << "space" << std::endl;
	else if (_current == nosp)
		std::cout << "nosp" << std::endl;
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
	else if (_current == percent))
		std::cout << "percent" << std::endl;
	else if (_current == amp))
		std::cout << "amp" << std::endl;
	else if (_current == plus))
		std::cout << "plus" << std::endl;
	else
		std::cout << "error" << std::endl;
		*/
}

const std::vector< Token >	&Parser::Parse(const string &str)
{
	_input = str;
	_it = --_input.begin();

	_Wrapper();
	while (_current != eoi)
	{
		_Wrapper();
	}
	return (_tokens);
}
