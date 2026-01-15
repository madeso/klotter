#include "klotter/dump.h"

#include <algorithm>
#include <iostream>
#include <cmath>


namespace
{
float clamp(float x, float lower, float upper)
{
    return std::max(lower, std::min(x, upper));
}

float calculate_contribution(float u_cutoff, float u_softness, float brightness)
{
	const float threshold = u_cutoff;
	const float knee = threshold * u_softness;
	const float x = clamp(brightness - threshold + knee, 0, 2 * knee);
	const float soft = (x * x) / (4 * knee + 0.00001f);
	const float contribution = std::max(soft, brightness - threshold) / std::max(brightness, 0.00001f);
	return contribution;
}

constexpr PlotRange range = {0.0f, 1.0f, 0.01f};

auto plot_knee(float softness)
{
	return plot_line(range, [softness](float x) {
        return calculate_contribution(0.4f, softness, x);
    });
}


float srgb_from_linear(float x)
{
	if (x >= 0.0031308f)
	{
	
		return 1.055f * std::pow(x, 1.0f / 2.4f) - 0.055f;
	}
	else
	{
	
		return 12.92f * x;
	}
}

float linear_from_srgb_simple(float value, float gamma)
{
	return std::pow(value, gamma);
}

float linear_from_srgb_exact(float x)
{
	if (x >= 0.04045f)
	{
		return std::pow((x + 0.055f) / (1 + 0.055f), 2.4f);
	}
	else
	{
		return x / 12.92f;
	}
}

// C_lin_3: https://youtu.be/_zQ_uBAHA4A?t=1304
float linear_from_srgb(float c)
{
	return 0.012522878f*c + 0.682171111f*c*c + 0.305306011f*c*c*c;
}

}

int main()
{
    const std::vector<glm::vec2> house = {
        {0.0f, 0.0f},   // bottom left
        {4.0f, 0.0f},   // bottom right
        {4.0f, 3.0f},   // top right
        {2.0f, 5.0f},   // roof peak
        {0.0f, 3.0f},   // top left
        {0.0f, 0.0f}    // close the base
    };

    Svg{}
		.add_line(svg::orange, house)
        .write("sample.html", 20.0f);

    Svg{}
		.add_line(svg::black, plot_knee(0.00f), "0.00")
		.add_line(svg::orange, plot_knee(0.25f), "0.25")
		.add_line(svg::reddish_purple, plot_knee(0.50f), "0.50")
		.add_line(svg::vermillion, plot_knee(0.75f), "0.75")
		.add_line(svg::sky_blue, plot_knee(1.00f), "1.00")
		.write("knee.html", 20.0f)
	;

	Svg{}
		.add_line(svg::black, plot_line(range, [](float x) { return x;}), "linear")
		.add_line(svg::reddish_purple, plot_line(range, [](float x) { return srgb_from_linear(x); }), "sRGB from linear")
		.add_line(svg::orange, plot_line(range, [](float x) { return linear_from_srgb_simple(x, 2.2f); }),"linear from sRGB (simple)")
		.add_line(svg::vermillion, plot_line(range, [](float x) { return linear_from_srgb_exact(x); }), "linear from sRGB (exact)")
		.add_line(svg::sky_blue, plot_line(range, [](float x) { return linear_from_srgb(x); }), "linear from sRGB (approx)")
		.write("linear_colors.html", 20.0f);

    std::cout << "Plot points dumped.\n";
    return 0;
}
