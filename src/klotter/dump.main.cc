#include "klotter/dump.h"

#include <algorithm>
#include <iostream>

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
		.add_line(svg::orange, house)
        .write("sample.html", 20.0f);

    Svg{}
		.add_line(svg::black, plot_func(0.00f), "0.00")
		.add_line(svg::orange, plot_func(0.25f), "0.25")
		.add_line(svg::reddish_purple, plot_func(0.50f), "0.50")
		.add_line(svg::vermillion, plot_func(0.75f), "0.75")
		.add_line(svg::sky_blue, plot_func(1.00f), "1.00")
		.write("knee.html", 20.0f)
	;


    std::cout << "Plot points dumped.\n";
    return 0;
}
