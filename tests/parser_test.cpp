#include <gtest/gtest.h>
#include "../include/Parser.hpp"

/*
 * Test reference : https://google.github.io/googletest/reference/testing.html
 * Test assertion : https://google.github.io/googletest/reference/assertions.html
 */

namespace {

	typedef std::vector< Token >		vec_tok;

	TEST(ParserClass, ParseBasicTest) {
		Parser p;
		vec_tok res = p.Parse("PASS toto");

		vec_tok wit;
		wit.push_back(letter);
		wit.push_back(space);
		wit.push_back(letter);
		wit.push_back(eoi);

		EXPECT_EQ(wit, res);
	}

	TEST(ParserClass, PrefixBasicTest) {
		Parser p;
		vec_tok res = p.Parse(":Tot{o-F42 PASS toto");

		vec_tok wit;
		wit.push_back(colon);
		wit.push_back(letter);
		wit.push_back(special);
		wit.push_back(letter);
		wit.push_back(dash);
		wit.push_back(letter);
		wit.push_back(digit);
		wit.push_back(space);
		wit.push_back(letter);
		wit.push_back(space);
		wit.push_back(letter);
		wit.push_back(eoi);

		EXPECT_EQ(wit, res);
	}

	TEST(ParserClass, PrefixAdvancedTest) {
		Parser p;
		vec_tok res = p.Parse(":Tot{o-F42!SuperUser@host-42school.web-school.com PASS toto");

		vec_tok wit;
		wit.push_back(colon);

		wit.push_back(letter);
		wit.push_back(special);
		wit.push_back(letter);
		wit.push_back(dash);
		wit.push_back(letter);
		wit.push_back(digit);
		wit.push_back(excl_mark);
		wit.push_back(letter);
		wit.push_back(at);
		wit.push_back(letter);
		wit.push_back(dash);
		wit.push_back(digit);
		wit.push_back(letter);
		wit.push_back(dot);
		wit.push_back(letter);
		wit.push_back(dash);
		wit.push_back(letter);
		wit.push_back(dot);
		wit.push_back(letter);
		wit.push_back(space);
		wit.push_back(letter);
		wit.push_back(space);
		wit.push_back(letter);
		wit.push_back(eoi);

		EXPECT_EQ(wit, res);
	}

}  // namespace
