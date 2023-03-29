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

Token	Parser::_GetToken() const
{
	int		state = 0;
	string	s;
	string::const_iterator end = _input.end();
	while (_it != end)
	{
		switch(state)
		{
			case 0:
				if (_it == end) return (eoi);
				else if (isalpha(*_it)) state = 1;
				else if (isdit(*_it)) state = 2;
				else if (*_it == ':') return (colon);
				else if (*_it == SPACE) return (space);
				else if (isspace(*_it)) state = 3;
				else if (*_it == '!') return (exclamation_mark);
				else if (*_it == '*') return (at);
				++_it;
				break ;
			case 1:
				break ;
			case 2:
				break ;
			case 5:
				break ;
			case 6:
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
