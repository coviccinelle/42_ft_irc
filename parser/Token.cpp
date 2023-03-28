#include "../include/Token.hpp"

Token::Token(void)
{
	std::cout << "Token default constructor called." << std::endl;

	return ;
}

Token::~Token(void)
{
	std::cout << "Token destructor called" << std::endl;

	return ;
}

Token::Token(Token const &src)
{
	std::cout << "Token copy constructor called" << std::endl;
	*this = src;

	return ;
}

Token &Token::operator=(Token const &rhs)
{
	std::cout << "Token copy assignment operator called" << std::endl;

	return (*this);
}
