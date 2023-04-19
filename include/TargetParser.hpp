#ifndef TARGETPARSER_HPP
#define TARGETPARSER_HPP
#include "../utils/utils.hpp"
#include "../include/Parser.hpp"

class TargetParser : public Parser
{
	public:
		TargetParser();
		virtual ~TargetParser();
		TargetParser(TargetParser const &src);
		TargetParser	&operator=(TargetParser const &rhs);
		
		void	ParseTarget(const string &str);
		void	DebugTarget() const;

		cst_vec_str	&GetTargets() const;
		void		ClearTargets();

	private:
		void	Parse(const string &str);
		void	Debug() const;

		void	_Target();
		void	AddTarget(const string &s);

		vec_str	_targets;
};

#endif
