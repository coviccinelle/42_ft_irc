#include "../include/TargetParser.hpp"

TargetParser::TargetParser() :
	Parser(),
	_targets()
{
	return ;
}

TargetParser::~TargetParser()
{
	return ;
}

TargetParser::TargetParser(TargetParser const &src) :
	Parser(),
	_targets()
{
	return ;
}

TargetParser &TargetParser::operator=(TargetParser const &rhs)
{
	if (&rhs == this)
		return (*this);

	return (*this);
}

void TargetParser::Debug() const
{
	std::cout << "===========[ DEBUG TargetParser ]===========" << std::endl;
	for (vec_str::const_iterator it = GetTargets().begin(); it != GetTargets().end(); ++it)
	{
		std::cout << *it;
		if (it + 1 != GetTargets().end())
			std::cout << " ";
	}
	std::cout << std::endl;
	std::cout << "===========================" << std::endl;
}

void	TargetParser::Parse(const string &str)
{
	_input = str;
	_it = --_input.begin();
	_Target();
}

void	TargetParser::ParseTarget(const string &str)
{
	Parse(str);
}

void	TargetParser::DebugTarget() const
{
	Debug();
}

void	TargetParser::_Target()
{
	std::string::iterator start = _it + 1;
	std::string::iterator start2 = _it + 1;
	_Wrapper();
	while (1)
	{
		if (_current == colon)
			throw irc_error("parsing failed: _Target: colon found", ERR_MIDDLE);
		if (_current == comma)
		{
			AddTarget(string(start, _it));
			start = _it + 1;
		}
		else if (_current == space || _current == eoi)
		{
			AddTarget(string(start, _it));
			return ;
		}
		_Wrapper();
	}
}

cst_vec_str	&TargetParser::GetTargets() const
{
	return (_targets);
}

void	TargetParser::AddTarget(const string &s)
{
	_targets.push_back(s);
}
