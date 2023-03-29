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

	TEST(ParserClass, ParseServername) {
		Parser p;
		vec_tok res = p.Parse(":42MyServerName-TheBest42IRC PASS toto");

		vec_tok wit;
		wit.push_back(digit);
		wit.push_back(letter);
		wit.push_back(dash);
		wit.push_back(letter);
		wit.push_back(digit);
		wit.push_back(letter);
		wit.push_back(space);
		wit.push_back(letter);
		wit.push_back(space);
		wit.push_back(letter);
		wit.push_back(eoi);

		EXPECT_EQ(wit, res);
	}

}  // namespace
