#pragma once

#include "klotter/angle.h"

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


// A color is sRGB space
struct Hsl
{
	Angle h;
	float s;
	float l;
};

/// Represents a linear sRGB color.
/// @see Color
struct Lin_rgb
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
Lin_rgb linear_from_srgb(const Rgb& value, float gamma);

float srgb_from_linear(float value);
Rgb srgb_from_linear(const Lin_rgb& value);

Rgb srgb_from_hsl(const Hsl& hsl);

// oklab & oklch
// from: https://bottosson.github.io/posts/oklab/

Lab oklab_from_linear(const Lin_rgb& c);
Lin_rgb linear_from_oklab(const Lab& c);
Lch oklch_from_oklab(const Lab& c);
Lab oklab_from_oklch(const Lch& c);


// gammut clipping functions from https://bottosson.github.io/posts/gamutclipping/
Lin_rgb gamut_clip_preserve_chroma(const Lin_rgb& rgb);
Lin_rgb gamut_clip_project_to_0_5(const Lin_rgb& rgb);
Lin_rgb gamut_clip_project_to_L_cusp(const Lin_rgb& rgb);
Lin_rgb gamut_clip_adaptive_L0_0_5(const Lin_rgb& rgb, float alpha = 0.05f);
Lin_rgb gamut_clip_adaptive_L0_L_cusp(const Lin_rgb& rgb, float alpha = 0.05f);

}

namespace klotter::colors
{

constexpr klotter::Rgb rgb(int r, int g, int b)
{
	return {static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f};
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
