#include <gtest/gtest.h>
#include "../include/Parser.hpp"
/*
 * Test reference : https://google.github.io/googletest/reference/testing.html
 * Test assertion : https://google.github.io/googletest/reference/assertions.html
 */

namespace {

	TEST(_GetToken, TestName) {
		Parser p;

		p.Parse("PASS toto");
	}


}  // namespace
