#include <exception>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <string>

typedef std::string string;

class system_error: public std::exception
{
	public:
		system_error();
		~system_error() throw();
		system_error &operator=(system_error const &rhs);
		system_error(system_error const &src);

		system_error(const string &error);

		virtual const char* what() const throw();
	private:
		string _str;
};
