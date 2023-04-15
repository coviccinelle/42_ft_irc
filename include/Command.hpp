#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <iostream>

#define INF_CMD_SIZE 8

enum InfoCommand {
	message = 0,
	prefix,
	params,
	user,
	host,
	nick,
	command,
	trailing
};

class Command
{
	public:
		Command(void);
		~Command(void);
		Command(Command const &src);
		Command	&operator=(Command const &rhs);

		cst_vec_str	&GetCinfo() const;
		cst_vec_str	&GetMiddle() const;

		void	SetMessage(const string &s);
		void	SetPrefix(const string &s);
		void	SetParams(const string &s);
		void	SetUser(const string &s);
		void	SetHost(const string &s);
		void	SetNickname(const string &s);
		void	SetCommand(const string &s);
		void	SetTrailing(const string &s);
		void	AddMiddle(const string &s);
	private:
		vec_str	_cinfo;
		vec_str	_middle;
};

#endif
