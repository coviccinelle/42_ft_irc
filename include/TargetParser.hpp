#ifndef TARGETPARSE_HPP
#define TARGETPARSE_HPP
#include "../utils/utils.hpp"

class TargetParse
{
	public:
		TargetParse(void);
		~TargetParse(void);
		TargetParse(TargetParse const &src);
		TargetParse	&operator=(TargetParse const &rhs);
		
		void	Debug() const;

		vec_str	targets;
};

#endif
