#include "klotter/cpp.h"

#include "catch2/catch_test_macros.hpp"

using namespace klotter;

TEST_CASE("check_equal", "[cpp]")
{
    CHECK(all_equal(std::array{5, 5, 5}));
	CHECK(all_equal(std::array{2, 2, 2}));
    CHECK_FALSE(all_equal(std::array{1, 2, 3}));
    CHECK_FALSE(all_equal(std::array{3, 2, 1}));
}
