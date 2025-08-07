
#include "klotter/render/ui.h"

#include "klotter/str.h"

#include "catchy/falsestring.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

#include <string>

using namespace klotter;

static std::string string_from(const ImVec2& self)
{
	return Str{} << "(" << self.x << ", " << self.y << ")";
}

static catchy::FalseString vec2_match(const ImVec2& left, const ImVec2& right)
{
	using W = Catch::Matchers::WithinAbsMatcher;
	constexpr double m = 0.0001;
	const auto same = W(left.x, m).match(right.x) && W(left.y, m).match(right.y);
	return same ? catchy::FalseString::True()
				: catchy::FalseString::False(Str{} << string_from(left) << " != " << string_from(right));
}

TEST_CASE("ui-region-works", "[ui]")
{
	// todo(Gustav): expand tests
	CHECK(vec2_match(calculate_region({0, 0}, {0, 0}, {0, 0}, {10, 10}, 10), {0, 0}));
}
