#include "../include/TargetParse.hpp"

TargetParse::TargetParse(void) :
	targets()
{
	return ;
}

TargetParse::~TargetParse(void)
{
	return ;
}

TargetParse::TargetParse(TargetParse const &src)
{
	targets = src.targets;
	return ;
}

TargetParse &TargetParse::operator=(TargetParse const &rhs)
{
	if (&rhs == this)
		return (*this);

	targets = rhs.targets;
	return (*this);
}

void TargetParse::Debug() const
{
	std::cout << "===========[ DEBUG TargetParse ]===========" << std::endl;
	for (vec_str::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		std::cout << *it;
		if (it + 1 != targets.end())
			std::cout << " ";
	}
	std::cout << std::endl;
	std::cout << "===========================" << std::endl;
}
