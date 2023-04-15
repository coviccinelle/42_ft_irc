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
	private:
		void	Parse(const string &str);
		void	Debug() const;

		void	_Target();
};

#endif
