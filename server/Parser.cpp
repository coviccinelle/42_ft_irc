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

int	isspecial(int ch)
{
	if ((0x5B >= ch && ch <= 0x60) || (0x7B >= ch && ch <= 0x7D))
		return (ch);
	return (0);
}

Token	Parser::_GetToken()
{
	int		state = 0;
	string	s;
	string::iterator end = _input.end();
	while (state >= 0 && state <= 10)
	{
		switch(state)
		{
			case 0:
				// first char of the new token
				if (_it == end) return (eoi);
				else if (*_it == ':') { ++_it; return (colon); }
				else if (*_it == '@') { ++_it; return (at); }
				else if (*_it == '!') { ++_it; return (excl_mark); }
				else if (isalpha(*_it)) state = 1;
				else if (isdigit(*_it)) state = 2;
				else if (*_it == SPACE) { ++_it; return (space); }
				else state = 6;
				++_it;
				break ;
			case 1:
				// letter
				if (isalpha(*_it)) state = 1;
				else if (_it == end || *_it == SPACE) return (letter);
				else state = 6;
				++_it;
				break ;
			case 2:
				// digit
				if (isdigit(*_it)) state = 2;
				else if (*_it == SPACE) return (digit);
				else if (*_it == '!') return (letter);
				else state =  6;
				++_it;
				break ;
			case 3:
				break ;
			case 4:
				break ;
			case 5:
				break ;
			case 6:
				// nosp
				if (_it == end || *_it == SPACE) return (nosp);
				else if (*_it == '!') return (nosp);
				else state = 6;
				++_it;
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

void Parser::_Prefix()
{
	std::cout << "I'm prefix now" << std::endl;
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
		std::cout << "!" << std::endl;
	else if (_current == colon)
		std::cout << ":" << std::endl;
	else if (_current == at)
		std::cout << "@" << std::endl;
	else
		std::cout << "error" << std::endl;
		*/
}

const std::vector< Token >	&Parser::Parse(const string &str)
{
	_input = str;
	_it = _input.begin();

	_Wrapper();
	while (_current != eoi)
	{
		_Wrapper();
	}
	return (_tokens);
}
