#pragma once

#include "klotter/angle.h"

// todo(Gustav): split into 2 headers, one color (rgb + linear) and one extra

namespace klotter
{

/// Represents color in gamma (non-linear) space (aka sRGB).
/// @see Lrgb
struct Rgb
{
	float r;
	float g;
	float b;

	constexpr Rgb(float red, float green, float blue)
		: r(red)
		, g(green)
		, b(blue)
	{}
};

/// Represents a linear sRGB color.
/// @see Color
struct Lin_rgb
{
	glm::vec3 linear;
};

/// Represents a color in the OKhsv color space
struct OkHsv
{
	Angle hue;
	float saturation;
	float value;
};

/// Represents a color in the OKhsl color space
struct OkHsl
{
	Angle hue;
	float saturation;
	float lightness;
};

/// Represents a color in the sRGB color space.
struct Hsl
{
	Angle hue;
	float saturation;
	float lightness;
};

/// Represents a color in the OKlch color space.
struct OkLch
{
	float l;
	float c;
	Angle h;
};

/// Represents a color in the OKlab color space.
struct OkLab
{
	float l;
	float a;
	float b;
};

// linear-space gamma-space
// todo(Gustav): should the functions that take gamma still be used?
float linear_from_srgb(float value, float gamma);
Lin_rgb linear_from_srgb(const Rgb& value, float gamma);

float linear_from_srgb(float value);
Lin_rgb linear_from_srgb(const Rgb& value);

float srgb_from_linear(float value);
Rgb srgb_from_linear(const Lin_rgb& value);


// oklab & oklch
// from: https://bottosson.github.io/posts/oklab/
OkLab oklab_from_linear(const Lin_rgb& c);
Lin_rgb linear_from_oklab(const OkLab& c);
OkLch oklch_from_oklab(const OkLab& c);
OkLab oklab_from_oklch(const OkLch& c);


// gammut clipping functions from https://bottosson.github.io/posts/gamutclipping/
Lin_rgb gamut_clip_preserve_chroma(const Lin_rgb& rgb);
Lin_rgb gamut_clip_project_to_0_5(const Lin_rgb& rgb);
Lin_rgb gamut_clip_project_to_l_cusp(const Lin_rgb& rgb);
Lin_rgb gamut_clip_adaptive_L0_0_5(const Lin_rgb& rgb, float alpha = 0.05f);
Lin_rgb gamut_clip_adaptive_l0_l_cusp(const Lin_rgb& rgb, float alpha = 0.05f);




Rgb srgb_from_okhsv(const OkHsv& hsv);
OkHsv okhsv_from_srgb(const Rgb& rgb);

Rgb srgb_from_okhsl(const OkHsl& hsl);
OkHsl okhsl_from_srgb(const Rgb& rgb);

// srgb from hsl (not ok hsl)
Rgb srgb_from_hsl(const Hsl& hsl);

float keep_within01(float f);
Lin_rgb keep_within(Lin_rgb c);

}

namespace klotter::hues
{
// https://tympanus.net/codrops/css_reference/hsl/
constexpr klotter::Angle red = Angle::from_degrees(0.0f);
constexpr klotter::Angle orange = Angle::from_degrees(30.0f);
constexpr klotter::Angle yellow = Angle::from_degrees(60.0f);
constexpr klotter::Angle green = Angle::from_degrees(120.0f);
constexpr klotter::Angle cyan = Angle::from_degrees(180.0f);
constexpr klotter::Angle blue = Angle::from_degrees(240.0f);
constexpr klotter::Angle magenta = Angle::from_degrees(300.0f);
}

namespace klotter::colors
{

/// helper function to create a rgb color with code similar to css causing some editors to display a color box
constexpr klotter::Rgb rgb(int r, int g, int b)
{
	return {static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f};
}

/// helper function to create a hsl color with code similar to css causing some editors to display a color box
constexpr Hsl hsl(int deg, float s, float l)
{
	return {.hue = Angle::from_degrees(static_cast<float>(deg)), .saturation = s / 100.0f, .lightness = l / 100.0f};
}

constexpr auto white = klotter::Rgb{1.0f, 1.0f, 1.0f};
constexpr auto black = klotter::Rgb{0.0f, 0.0f, 0.0f};

// https://www.nature.com/articles/nmeth.1618
constexpr klotter::Rgb orange = rgb(230, 159, 0);
constexpr klotter::Rgb blue_sky = rgb(86, 180, 233);
constexpr klotter::Rgb green_bluish = rgb(0, 158, 115);
constexpr klotter::Rgb yellow = rgb(240, 228, 66);
constexpr klotter::Rgb blue = rgb(0, 114, 178);
constexpr klotter::Rgb red_vermillion = rgb(213, 94, 0);
constexpr klotter::Rgb purple_redish = rgb(204, 121, 167);

}  //  namespace klotter::colors
