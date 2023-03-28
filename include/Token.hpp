#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "../utils/utils.hpp"
#include "../include/irc_error.hpp"

class Token
{
	public:
		Token(void);
		~Token(void);
		Token(Token const &src);
		Token	&operator=(Token const &rhs);
	private:
};

#endif
