#pragma once

#include "klotter/angle.h"

namespace klotter
{

/// Represents color in gamma (non-linear) space (aka sRGB).
/// @see Lrgb
struct Color
{
	float r;
	float g;
	float b;

	constexpr Color(float red, float green, float blue)
		: r(red)
		, g(green)
		, b(blue)
	{}
};


// A color is sRGB space
struct Hsl
{
	Angle h;
	float s;
	float l;
};

/// Represents a linear sRGB color.
/// @see Color
struct Lrgb
{
	glm::vec3 linear;
};

/// Represents a color in the OKlch color space.
struct Lch
{
	float l;
	float c;
	Angle h;
};

/// Represents a color in the OKlab color space.
struct Lab
{
	float l;
	float a;
	float b;
};

// linear-space gamma-space
float linear_from_srgb(float value, float gamma);
Lrgb linear_from_srgb(const Color& value, float gamma);

float srgb_from_linear(float value);
Color srgb_from_linear(const Lrgb& value);

Color srgb_from_hsl(const Hsl& hsl);

// oklab & oklch
Lab oklab_from_linear(const Lrgb& c);
Lrgb linear_from_oklab(const Lab& c);
Lch oklch_from_oklab(const Lab& c);
Lab oklab_from_oklch(const Lch& c);

}

namespace klotter::colors
{

constexpr klotter::Color rgb(int r, int g, int b)
{
	return {static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f};
}

constexpr auto white = klotter::Color{1.0f, 1.0f, 1.0f};
constexpr auto black = klotter::Color{0.0f, 0.0f, 0.0f};

// https://www.nature.com/articles/nmeth.1618
constexpr klotter::Color orange = rgb(230, 159, 0);
constexpr klotter::Color blue_sky = rgb(86, 180, 233);
constexpr klotter::Color green_bluish = rgb(0, 158, 115);
constexpr klotter::Color yellow = rgb(240, 228, 66);
constexpr klotter::Color blue = rgb(0, 114, 178);
constexpr klotter::Color red_vermillion = rgb(213, 94, 0);
constexpr klotter::Color purple_redish = rgb(204, 121, 167);

}  //  namespace klotter::colors
