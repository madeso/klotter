#include <iostream>

#include "klotter/dump.h"

#include <algorithm>

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

auto plot_func(float softness)
{
	return plot_line({0.0f, 1.0f, 0.01f}, [softness](float x) {
        return calculate_contribution(0.4f, softness, x);
    });
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
        .add_line(house, svg::orange)
        .write("sample.html", 20.0f);

    Svg{}
		.add_line(plot_func(0.00f), svg::black)
		.add_line(plot_func(0.25f), svg::orange)
		.add_line(plot_func(0.50f), svg::reddish_purple)
		.add_line(plot_func(0.75f), svg::vermillion)
		.add_line(plot_func(1.00f), svg::sky_blue)
		.write("knee.html", 20.0f)
	;


    std::cout << "Plot points dumped.\n";
    return 0;
}
