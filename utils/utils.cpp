#include "utils.hpp"

string ltrim(const string &s)
{
	size_t	start = s.find_first_not_of(" \t\n\r");
	return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string &s)
{
	size_t	end = s.find_last_not_of(" \t\n\r");
	return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string &s)
{
	return rtrim(ltrim(s));
}
