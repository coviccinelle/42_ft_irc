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
	while (it != end)
	{
		switch(state)
		{
			case 0:
				if (_it == end) return (eoi);
				else if (isspace(*_it)) state = 0;
				else if (isalpha(*_it)) state = 6;
				else if (isdigit(*_it)) state = 1;
				else if (*_it == ':') state = 2;
				else if (*_it == '!') state = 3;
				else if (*_it == '@') state = 4;
				++_it;
				break ;
			case 1:
				if (isdigit(*_it)) state = 1;
				break ;
			case 2:
				if (_it == _input.begin()) state = 5;
				else if (_it[-1] == SPACE) state = 6;
				++_it;
				break ;
			case 5:
				// handle prefix
				break ;
			case 6:
				// handle trailing
				break ;
		}
	}
	return (eoi);
}
