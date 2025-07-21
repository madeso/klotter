#pragma once

namespace klotter
{

/// Represents color in gamma space (aka sRGB).
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

float linear_from_srgb(float value, float gamma = 2.2f);
glm::vec3 linear_from_srgb(const Color& value, float gamma = 2.2f);

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
