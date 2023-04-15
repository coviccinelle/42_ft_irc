#include "../include/TargetParser.hpp"

TargetParser::TargetParser(void) :
	targets()
{
	return ;
}

TargetParser::~TargetParser(void)
{
	return ;
}

TargetParser::TargetParser(TargetParser const &src)
{
	targets = src.targets;
	return ;
}

TargetParser &TargetParser::operator=(TargetParser const &rhs)
{
	if (&rhs == this)
		return (*this);

	targets = rhs.targets;
	return (*this);
}

void TargetParser::Debug() const
{
	std::cout << "===========[ DEBUG TargetParser ]===========" << std::endl;
	for (vec_str::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		std::cout << *it;
		if (it + 1 != targets.end())
			std::cout << " ";
	}
	std::cout << std::endl;
	std::cout << "===========================" << std::endl;
}

void	TargetParser::_Target(const string &str)
{

}

void	TargetParser::_Target()
{
	_input = str;
	_it = --_input.begin();

	std::string::iterator start = _it + 1;
	std::string::iterator start2 = _it + 1;
	_Wrapper();
	while (1)
	{
		if (_current == colon)
			throw irc_error("parsing failed: _Target: colon found", ERR_MIDDLE);
		if (_current == comma)
		{
			_cmd->target.push_back(string(start, _it));
			start = _it + 1;
		}
		else if (_current == space || _current == eoi)
		{
			_cmd->target.push_back(string(start, _it));
			_cmd->middle.push_back(string(start2, _it));
			return ;
		}
		_Wrapper();
	}
}
