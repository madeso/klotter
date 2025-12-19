#include "klotter/vdebug.h"

#include "klotter/str.h"

#include "glm/glm.hpp"
#include <random>
#include <vector>

// ==================================================
// CLOSEST PAIR
namespace closest_pair_demo
{
using Pair = std::pair<glm::vec2, glm::vec2>;

// Returns an array containing the points nearest to one another out of the given set
Pair find_closest_pair_of_points(const std::vector<glm::vec2>& points)
{
	auto vd = VisualDebugging::VisualDebug{};

    vd.begin_frame("All points", true);
    vd.draw_points(points, 5);
    Pair closest_point_pair;
    float shortest_distance_found = std::numeric_limits<float>::max();

    for (std::size_t outer_point_index = 0; outer_point_index < points.size(); outer_point_index++)
    {
        for (std::size_t inner_point_index = outer_point_index + 1; inner_point_index < points.size(); inner_point_index++)
        {
            const auto current_distance = glm::length(points[outer_point_index] - points[inner_point_index]);
            if (current_distance < shortest_distance_found)
            {
                shortest_distance_found = current_distance;
                closest_point_pair.first = points[outer_point_index];
                closest_point_pair.second = points[inner_point_index];
            }
            vd.begin_frame("Compare dst", true);
			vd.set_color(VisualDebugging::Colors::lightRed, VisualDebugging::Colors::veryDarkGrey);
            vd.draw_point(points[outer_point_index], 6);
            vd.draw_arrow(points[outer_point_index], points[inner_point_index], 10);
            vd.do_not_show_next_element_when_frame_is_in_background();
            vd.set_color(VisualDebugging::Colors::lightGreen);
			vd.draw_line_segment_with_label(closest_point_pair.first, closest_point_pair.second, Str {}  << shortest_distance_found);
        }
    }

    vd.begin_frame("Finished");
    vd.set_color(VisualDebugging::Colors::lightGreen);
	vd.draw_points({closest_point_pair.first, closest_point_pair.second}, 7);
	vd.draw_line_segment_with_label(closest_point_pair.first, closest_point_pair.second, Str{} << shortest_distance_found);
	vd.save("closest points");
    return closest_point_pair;
}

std::vector<glm::vec2> generate_points(int num_points, float radius)
{
    std::random_device r;
	std::uniform_real_distribution uniform(-radius, radius);

    std::vector<glm::vec2> points;
    points.reserve(num_points);
    for (int point_index = 0; point_index < num_points; point_index+=1)
    {
		points.emplace_back(uniform(r), uniform(r));
    }
    return points;
}

void main()
{
	find_closest_pair_of_points(generate_points(8, 100.0f));
}
}


// ==================================================
// KNEE
namespace knee_demo
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

std::vector<float> plot_func(const VisualDebugging::PlotRange& range, float softness)
{
	return VisualDebugging::plot_line(range, [softness](float x) { return calculate_contribution(0.4f, softness, x); });
}

void main()
{
	auto vd = VisualDebugging::VisualDebug{};
	vd.y_positive();
	constexpr VisualDebugging::PlotRange range = {0.0f, 1.0f, 0.01f};
    vd.set_color(VisualDebugging::Colors::black);      vd.draw_plot(range, plot_func(range, 0.00f));
    vd.set_color(VisualDebugging::Colors::darkGreen);  vd.draw_plot(range, plot_func(range, 0.25f));
    vd.set_color(VisualDebugging::Colors::darkRed);    vd.draw_plot(range, plot_func(range, 0.50f));
    vd.set_color(VisualDebugging::Colors::lightGreen); vd.draw_plot(range, plot_func(range, 0.75f));
    vd.set_color(VisualDebugging::Colors::lightBlue);  vd.draw_plot(range, plot_func(range, 1.00f));
	vd.save("knee");
}
}

// =================================================
// HOUSE
namespace house_demo
{
void main()
{
	const std::vector<glm::vec2> house = {
		{0.0f, 0.0f},  // bottom left
		{4.0f, 0.0f},  // bottom right
		{4.0f, 3.0f},  // top right
		{2.0f, 5.0f},  // roof peak
		{0.0f, 3.0f},  // top left
		{0.0f, 0.0f}  // close the base
	};

    auto vd = VisualDebugging::VisualDebug{};
	vd.y_positive();
	vd.draw_line(house);
	vd.save("house");
}
}

int main(int, char*[])
{
	closest_pair_demo::main();
	knee_demo::main();
	house_demo::main();
}

