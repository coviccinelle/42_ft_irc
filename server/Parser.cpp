#include "../include/Parser.hpp"

Parser::Parser(void) : 
	_it(),
	_current(error),
	_tokens(),
	_input(""),
	_targets()
{
	return ;
}

Parser::~Parser(void)
{
	return ;
}

Parser::Parser(Parser const &src) :
	_it(src._it),
	_current(src._current),
	_tokens(src._tokens),
	_input(src._input),
	_targets(src._targets)
{

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
	_targets = rhs._targets;

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

void Parser::_Wrapper()
{
	_current = _GetToken();
	_tokens.push_back(_current);
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

bool	Parser::isValidUserMode(const string &str)
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
