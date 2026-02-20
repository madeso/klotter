#include "klotter/render/color.h"

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"

#include "klotter/test.util.h"

using namespace klotter;

namespace
{
catchy::FalseString are_equal(const Rgb& lhs, const Rgb& rhs)
{
	const auto rgb255 = 1.5f;
	return testing::Equaler{lhs, rhs}
		.add("r", [](const auto& c) { return c.r * 255.0f; }, rgb255)
		.add("g", [](const auto& c) { return c.g * 255.0f; }, rgb255)
		.add("b", [](const auto& c) { return c.b * 255.0f; }, rgb255)
		.resolve();
}

catchy::FalseString are_equal(const Lin_rgb& lhs, const Lin_rgb& rhs)
{
	return testing::Equaler{lhs, rhs}
        .add("r", [](const auto& c) { return c.linear.r;}, testing::machine_epsilon)
        .add("g", [](const auto& c) { return c.linear.g;}, testing::machine_epsilon)
        .add("b", [](const auto& c) { return c.linear.b;}, testing::machine_epsilon)
        .resolve();
}
catchy::FalseString are_equal(const OkLch& lhs, const OkLch& rhs)
{
	return testing::Equaler{lhs, rhs}
        .add("l", [](const OkLch& c) { return c.l;}, testing::machine_epsilon)
        .add("c", [](const OkLch& c) { return c.c;}, testing::machine_epsilon)
		// todo(Gustav): improve angle compare
        .add("h", [](const OkLch& c) { return c.h.get_wrapped().as_degrees();}, testing::machine_epsilon)
        .resolve();
}
catchy::FalseString are_equal(const OkLab& lhs, const OkLab& rhs)
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
	    CHECK(are_equal(linear_from_srgb(Rgb{0.0f, 0.0f, 0.0f}, ignore_gamma), {{0.0f, 0.0f, 0.0f}}));
		CHECK(are_equal(linear_from_srgb(Rgb{1.0f, 1.0f, 1.0f}, ignore_gamma), {{1.0f, 1.0f, 1.0f}}));
	}
	CHECK(are_equal(srgb_from_linear(Lin_rgb{{0.0f, 0.0f, 0.0f}}), Rgb{0.0f, 0.0f, 0.0f}));
	CHECK(are_equal(srgb_from_linear(Lin_rgb{{1.0f, 1.0f, 1.0f}}), Rgb{1.0f, 1.0f, 1.0f}));

    CHECK(are_equal(oklab_from_linear({{0.0f, 0.0f, 0.0f}}), {0.0f, 0.0f, 0.0f}));
    CHECK(are_equal(linear_from_oklab({0.0f, 0.0f, 0.0f}), {{0.0f, 0.0f, 0.0f}}));
    CHECK(are_equal(oklch_from_oklab({0.0f, 0.0f, 0.0f}), {.l = 0.0f, .c = 0.0f, .h = klotter::no_rotation}));
	CHECK(are_equal(oklab_from_oklch({0.0f, 0.0f, klotter::no_rotation}), {0.0f, 0.0f, 0.0f}));
}

TEST_CASE("color space conversion sky blue", "[color]")
{
	// sky blue from bang-wong palette converted with external tool https://apps.colorjs.io/picker/
	const auto lrgb = Lin_rgb{{0.0930589628f, 0.4564110232f, 0.8148465722f}};
	const auto oklab = OkLab{.l = 0.7345201878f, .a = -0.0653327754f, .b = -0.0975091851f};
	const auto oklch = OkLch{.l = 0.7345201878f, .c = 0.1173729642f, .h = Angle::from_degrees(236.1772573f)};
	
    // todo(Gustav): how accurate is the linear to sRGB function?
	// CHECK(are_equal(linear_from_srgb(Color{86.f / 255.f, 180.f / 255.f, 233.f / 255.f}, 2.2f), lrgb));

	CHECK(are_equal(oklab_from_linear(lrgb), oklab));
	CHECK(are_equal(linear_from_oklab(oklab), lrgb));
	CHECK(are_equal(oklch_from_oklab(oklab), oklch));
	CHECK(are_equal(oklab_from_oklch(oklch), oklab));

	// CHECK(are_equal(srgb_from_linear(linear_from_oklab(oklab_from_oklch({0.1441f, 0.0498f, Angle::from_degrees(264.0f)}))), 
	// Color{0x2/255.f, 0x8/255.0f, 0x1e/255.0f}));
}

TEST_CASE("rgb -> hsl", "[color]")
{
	using namespace klotter::colors;

	// https://ethanschoonover.com/solarized/
	CHECK(are_equal(srgb_from_hsl(hsl(192, 100.0f, 10.6f)), Rgb{  0.0f / 255.0f,  43.0f/255.0f,  54.0f/255.0f})); // base03
	CHECK(are_equal(srgb_from_hsl(hsl(192, 80.8f,  14.3f)), Rgb{  7.0f / 255.0f,  54.0f/255.0f,  66.0f/255.0f})); // base02
	CHECK(are_equal(srgb_from_hsl(hsl(194, 14.1f,  40.2f)), Rgb{ 88.0f / 255.0f, 110.0f/255.0f, 117.0f/255.0f})); // base01
	CHECK(are_equal(srgb_from_hsl(hsl(196, 12.9f,  45.5f)), Rgb{101.0f / 255.0f, 123.0f/255.0f, 131.0f/255.0f})); // base00
	CHECK(are_equal(srgb_from_hsl(hsl(186, 8.3f,  55.1f)), Rgb{131.0f / 255.0f, 148.0f/255.0f, 150.0f/255.0f})); // base0
	CHECK(are_equal(srgb_from_hsl(hsl(180, 6.9f,  60.4f)), Rgb{147.0f / 255.0f, 161.0f/255.0f, 161.0f/255.0f})); // base1
	CHECK(are_equal(srgb_from_hsl(hsl(46, 42.4f,  88.4f)), Rgb{238.0f / 255.0f, 232.0f/255.0f, 213.0f/255.0f})); // base2
	CHECK(are_equal(srgb_from_hsl(hsl(44, 86.7f,  94.1f)), Rgb{253.0f / 255.0f, 246.0f/255.0f, 227.0f/255.0f})); // base3
	CHECK(are_equal(srgb_from_hsl(hsl(45, 100.0f, 35.5f)), Rgb{181.0f / 255.0f, 137.0f/255.0f,   0.0f/255.0f})); // yellow
	CHECK(are_equal(srgb_from_hsl(hsl(18, 80.4f,  44.1f)), Rgb{203.0f / 255.0f,  75.0f/255.0f,  22.0f/255.0f})); // orange
	CHECK(are_equal(srgb_from_hsl(hsl(1, 71.2f,  52.4f)), Rgb{220.0f / 255.0f,  50.0f/255.0f,  47.0f/255.0f})); // red
	CHECK(are_equal(srgb_from_hsl(hsl(331, 64.1f,  52.0f)), Rgb{211.0f / 255.0f,  54.0f/255.0f, 130.0f/255.0f})); // magenta
	CHECK(are_equal(srgb_from_hsl(hsl(237, 42.7f,  59.6f)), Rgb{108.0f / 255.0f, 113.0f/255.0f, 196.0f/255.0f})); // violet
	CHECK(are_equal(srgb_from_hsl(hsl(205, 69.4f,  48.6f)), Rgb{ 38.0f / 255.0f, 139.0f/255.0f, 210.0f/255.0f})); // blue
	CHECK(are_equal(srgb_from_hsl(hsl(175, 58.6f,  39.8f)), Rgb{ 42.0f / 255.0f, 161.0f/255.0f, 152.0f/255.0f})); // cyan
	CHECK(are_equal(srgb_from_hsl(hsl(68, 100.0f, 30.0f)), Rgb{133.0f / 255.0f, 153.0f / 255.0f, 0.0f / 255.0f})); // green

  	CHECK(are_equal(srgb_from_hsl(hsl(0,0,0)           ), rgb(0,0,0)       )); // Black  
  	CHECK(are_equal(srgb_from_hsl(hsl(0,0,100)         ), rgb(255,255,255) )); // White  
  	CHECK(are_equal(srgb_from_hsl(hsl(0,100,50)        ), rgb(255,0,0)     )); // Red    
  	CHECK(are_equal(srgb_from_hsl(hsl(120,100,50)      ), rgb(0,255,0)     )); // Lime   
  	CHECK(are_equal(srgb_from_hsl(hsl(240,100,50)      ), rgb(0,0,255)     )); // Blue   
  	CHECK(are_equal(srgb_from_hsl(hsl(60,100,50)       ), rgb(255,255,0)   )); // Yellow 
  	CHECK(are_equal(srgb_from_hsl(hsl(180,100,50)      ), rgb(0,255,255)   )); // Cyan   
  	CHECK(are_equal(srgb_from_hsl(hsl(300,100,50)      ), rgb(255,0,255)   )); // Magenta
  	CHECK(are_equal(srgb_from_hsl(hsl(0,0,75)          ), rgb(191,191,191) )); // Silver 
  	CHECK(are_equal(srgb_from_hsl(hsl(0,0,50)          ), rgb(128,128,128) )); // Gray   
  	CHECK(are_equal(srgb_from_hsl(hsl(0,100,25)        ), rgb(128,0,0)     )); // Maroon 
  	CHECK(are_equal(srgb_from_hsl(hsl(60,100,25)       ), rgb(128,128,0)   )); // Olive  
  	CHECK(are_equal(srgb_from_hsl(hsl(120,100,25)      ), rgb(0,128,0)     )); // Green  
  	CHECK(are_equal(srgb_from_hsl(hsl(300,100,25)      ), rgb(128,0,128)   )); // Purple 
  	CHECK(are_equal(srgb_from_hsl(hsl(180,100,25)      ), rgb(0,128,128)   )); // Teal   
  	CHECK(are_equal(srgb_from_hsl(hsl(240,100,25)      ), rgb(0,0,128)     )); // Navy   
}
