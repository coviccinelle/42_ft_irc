#include "../include/Parser.hpp"

Parser::Parser(void)
{
	std::cout << "Parser default constructor called." << std::endl;

	return ;
}

Parser::~Parser(void)
{
	std::cout << "Parser destructor called" << std::endl;

	return ;
}

Parser::Parser(Parser const &src)
{
	std::cout << "Parser copy constructor called" << std::endl;
	*this = src;

	return ;
}

Parser &Parser::operator=(Parser const &rhs)
{
	std::cout << "Parser copy assignment operator called" << std::endl;
	if (&rhs == this)
		return (*this);

	return (*this);
}

void	Parser::Parse(const string &str)
{
	_input = str;
	_it = _input.begin();
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
	while (_it != end)
	{
		switch(state)
		{
			case 0:
				if (_it == end) return (eoi);
				else if (*_it == ':') return (colon);
				else if (*_it == SPACE) return (space);
				else if (*_it == '!') return (exclamation_mark);
				else if (*_it == '@') return (at);
				else if (isalpha(*_it)) state = 1;
				else if (isdigit(*_it)) state = 2;
				else if (isspecial(*_it)) state = 3;
				else state = 5;
				++_it;
				break ;
			case 1:
				// letter
				if (isalpha(*_it)) state = 1;
				else if (*_it == SPACE)
				{
					--_it;
					return (letter);
				}
				else if (isdigit(*_it))
				{
					
				}
				break ;
			case 2:
				// digit
				if (isdigit(*_it)) state = 2;
				else if (*_it == SPACE)
				{
					--_it;
					return (digit);
				}
				else state =  6;
				break ;
			case 3:
				if (isalpha(*_it) || isdigit(*_it) || isspecial(*_it) || *_it == '-')
					s += *_it;
				else if (*_it == SPACE)
				{
					--_it;
					return (special);
				}
				else state = 6;
				break ;
			case 4:
				break ;
			case 5:
				if (_it != end) state = 3;
				else
				{
					--_it;
					return (nospcl);
				}
				break ;
			case 6:
				// nospcl
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

void	wrapper()
{
	std::cout << "Bullshit" << std::endl;
	_current = _GetToken();
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
	wrapper();
	if (_current == colon)
	{
		_Prefix();
		wrapper();
		if (_current != space)
			throw ;
	}
	else if (_current == letter || _current == digit)
	{
		_Command();
		wrapper();
		_Param();
		wrapper();
		if (_current != eoi)
			throw;
	}
}



// testing purpose only
int main(void)
{
	return (0);
}
