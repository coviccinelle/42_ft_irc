#include "../include/Ban.hpp"

Ban::Ban(const string &m, const string &f, const string &t) :
	_mask(m),
	_from(f),
	_time(t)
{

}

Ban::~Ban()
{
}

Ban::Ban(const Ban &src) :
	_mask(src._mask),
	_from(src._from),
	_time(src._time)
{
	return ;
}

Ban	&Ban::operator=(const Ban& rhs)
{
	if (&rhs == this)
		return (*this);
	_mask = rhs._mask;
	_from = rhs._from;
	_time = rhs._time;

	return (*this);
}

const string &Ban::GetMask() const
{
	return (_mask);
}

const string &Ban::GetTime() const
{
	return (_time);
}

const string &Ban::GetFrom() const
{
	return (_from);
}

void Ban::SetMask(const string &mask)
{
	_mask = mask;
}

void Ban::SetFrom(const string &from)
{
	_from =  from;
}

void Ban::SetTime(const string &time)
{
	_time = time;
}
