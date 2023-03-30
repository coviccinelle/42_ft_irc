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
	while (state >= 0 && state <= 10)
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
				else if (*_it == SPACE) { return (space); }
				else if (isalpha(*_it)) state = 1;
				else if (isdigit(*_it)) state = 2;
				else if (isspecial(_it)) state = 3;
				else state = 6;
				break ;
			case 1:
				// letter
				if (isalpha(*_it)) { state = 1;}
				else if (_it == end || is_fstate(_it, letter)) { --_it; return (letter); }
				else state = 6;
				break ;
			case 2:
				// digit
				if (isdigit(*_it)) { state = 2; }
				else if (_it == end || is_fstate(_it, digit)) { --_it; return (digit); }
				else state =  6;
				break ;
			case 3:
				// special
				if (isspecial(_it)) { state = 3; }
				else if (_it == end || is_fstate(_it, special)) { --_it; return (special); }
				else state = 6;
				break ;
			case 4:
				break ;
			case 5:
				break ;
			case 6:
				// nosp
				if (_it == end || is_fstate(_it, nosp)) { --_it; return (nosp); }
				else if (*_it == '!') return (nosp);
				else state = 6;
				break ;
			case 7:
				break ;
			case 8:
				break ;
			case 9:
				break ;
			case 10:
				break ;
		}
	}
	return (eoi);
}

void	Parser::_Nickname()
{
	std::cout << "I'm Nickname" << std::endl;
}

//prefix     =  servername / ( nickname [ [ "!" user ] "@" host ] )
void Parser::_Prefix()
{
	std::cout << "I'm prefix" << std::endl;
	_Wrapper();
	if (_current != letter && _current != special)
		throw ;
	_Nickname();
	//Nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
	_Wrapper();
	if (_current == excl_mark || _current == at)
	{
		if (_current == excl_mark)
		{
			_Wrapper();
			std::cout << "_User() required here" << std::endl;
			//_User();  // user       =  1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )
					  // ; any octet except NUL, CR, LF, " " and "@"
			_Wrapper();
		}
		if (_current == at)
		{
			std::cout << "host required here!" << std::endl;
			//host = hostname /hostaddr
		}
	}
	else if (_current != eoi)
		throw ;
}

void	Parser::_Command()
{
	std::cout << "I'm command" << std::endl;
}

void	Parser::_Param()
{
	std::cout << "I'm Param" << std::endl;
}

void Parser::_Message()
{
	_Wrapper();
	if (_current == colon)
	{
		_Prefix();
		_Wrapper();
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
