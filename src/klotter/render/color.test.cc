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
        .add("l", [](const Lch& c) { return c.l;}, testing::machine_epsilon)
        .add("c", [](const Lch& c) { return c.c;}, testing::machine_epsilon)
		// todo(Gustav): improve angle compare
        .add("h", [](const Lch& c) { return c.h.get_wrapped().as_degrees();}, testing::machine_epsilon)
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

TEST_CASE("color space conversion black/white", "[color]")
{
    SECTION("gamma was ignored")
    {
        const float ignore_gamma = GENERATE(1.0f, 2.2f);
	    CHECK(are_equal(linear_from_srgb(Color{0.0f, 0.0f, 0.0f}, ignore_gamma), {{0.0f, 0.0f, 0.0f}}));
		CHECK(are_equal(linear_from_srgb(Color{1.0f, 1.0f, 1.0f}, ignore_gamma), {{1.0f, 1.0f, 1.0f}}));
	}

    CHECK(are_equal(oklab_from_linear({{0.0f, 0.0f, 0.0f}}), {0.0f, 0.0f, 0.0f}));
    CHECK(are_equal(linear_from_oklab({0.0f, 0.0f, 0.0f}), {{0.0f, 0.0f, 0.0f}}));
    CHECK(are_equal(oklch_from_oklab({0.0f, 0.0f, 0.0f}), {.l = 0.0f, .c = 0.0f, .h = klotter::no_rotation}));
	CHECK(are_equal(oklab_from_oklch({0.0f, 0.0f, klotter::no_rotation}), {0.0f, 0.0f, 0.0f}));
}

TEST_CASE("color space conversion sky blue", "[color]")
{
	// sky blue from bang-wong palette converted with external tool https://apps.colorjs.io/picker/
	const auto lrgb = Lrgb{{0.0930589628f, 0.4564110232f, 0.8148465722f}};
	const auto oklab = Lab{.l = 0.7345201878f, .a = -0.0653327754f, .b = -0.0975091851f};
	const auto oklch = Lch{.l = 0.7345201878f, .c = 0.1173729642f, .h = Angle::from_degrees(236.1772573f)};
	
    // todo(Gustav): how accurate is the linear to sRGB function?
	// CHECK(are_equal(linear_from_srgb(Color{86.f / 255.f, 180.f / 255.f, 233.f / 255.f}, 2.2f), lrgb));

	CHECK(are_equal(oklab_from_linear(lrgb), oklab));
	CHECK(are_equal(linear_from_oklab(oklab), lrgb));
	CHECK(are_equal(oklch_from_oklab(oklab), oklch));
	CHECK(are_equal(oklab_from_oklch(oklch), oklab));
}
