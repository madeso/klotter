#include "klotter/render/color.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"

#include "klotter/test.util.h"

using namespace klotter;

namespace
{
catchy::FalseString are_equal(const Lrgb& lhs, const Lrgb& rhs)
{
	return testing::Equaler{lhs, rhs}
        .add("r", [](const auto& c) { return c.linear.r;}, testing::machine_epsilon)
        .add("g", [](const auto& c) { return c.linear.g;}, testing::machine_epsilon)
        .add("b", [](const auto& c) { return c.linear.b;}, testing::machine_epsilon)
        .resolve();
}
catchy::FalseString are_equal(const Lch& lhs, const Lch& rhs)
{
	return testing::Equaler{lhs, rhs}
        .add("l", [](const auto& c) { return c.l;}, testing::machine_epsilon)
        .add("c", [](const auto& c) { return c.c;}, testing::machine_epsilon)
        .add("h", [](const auto& c) { return c.h;}, testing::machine_epsilon)
        .resolve();
}
catchy::FalseString are_equal(const Lab& lhs, const Lab& rhs)
{
	return testing::Equaler{lhs, rhs}
        .add("l", [](const auto& c) { return c.l;}, testing::machine_epsilon)
        .add("a", [](const auto& c) { return c.a;}, testing::machine_epsilon)
        .add("b", [](const auto& c) { return c.b;}, testing::machine_epsilon)
        .resolve();
}
}

TEST_CASE("color black/white", "[color]")
{
    // todo(Gustav): expand with better testing data...
    SECTION("gamma was ignored")
    {
        const float ignore_gamma = GENERATE(1.0f, 2.2f);
	    CHECK(are_equal(linear_from_srgb(Color{0.0f, 0.0f, 0.0f}, ignore_gamma), {{0.0f, 0.0f, 0.0f}}));
		CHECK(are_equal(linear_from_srgb(Color{1.0f, 1.0f, 1.0f}, ignore_gamma), {{1.0f, 1.0f, 1.0f}}));
	}

    CHECK(are_equal(oklab_from_linear({{0.0f, 0.0f, 0.0f}}), {0.0f, 0.0f, 0.0f}));
    CHECK(are_equal(linear_from_oklab({0.0f, 0.0f, 0.0f}), {{0.0f, 0.0f, 0.0f}}));
    CHECK(are_equal(oklch_from_oklab({0.0f, 0.0f, 0.0f}), {0.0f, 0.0f, 0.0f}));
	CHECK(are_equal(oklab_from_oklch({0.0f, 0.0f, 0.0f}), {0.0f, 0.0f, 0.0f}));
    
}
