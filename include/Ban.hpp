#ifndef BAN_HPP
#define BAN_HPP
#include <iostream>
#include "../utils/utils.hpp"

class Ban {
	public:
		Ban(const string &m = string(""),
			const string &f = string(""),
			const string &t = string(""));
		~Ban();
		Ban(const Ban &src);
		Ban	&operator=(const Ban& rhs);

		const string &GetMask() const;
		const string &GetTime() const;
		const string &GetFrom() const;

		void SetMask(const string &mask);
		void SetFrom(const string &from);
		void SetTime(const string &time);

	private:
		string	_mask;
		string	_from;
		string	_time;
};

#endif
