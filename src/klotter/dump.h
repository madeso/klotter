#pragma once

namespace svg
{
    // bang-wong color palette
    constexpr std::string_view black = "#000000";
	constexpr std::string_view orange = "#e69f00";
	constexpr std::string_view sky_blue = "#56b4e9";
	constexpr std::string_view bluish_green = "#009e73";
	constexpr std::string_view yellow = "#f0e442";
    constexpr std::string_view blue = "#0072b2";
    constexpr std::string_view vermillion = "#d55e00";
    constexpr std::string_view reddish_purple = "#cc79a7";
}

struct PlotRange
{
	float start;
	float end;
	float step;
};

template<typename F>
std::vector<glm::vec2> plot_line(const PlotRange& p, F f)
{
	std::vector<glm::vec2> points;
	for (float x = p.start; x <= p.end; x += p.step)
	{
		points.emplace_back(x, f(x));
	}
	return points;
}

struct SvgLine
{
	std::string label;
    std::string_view color;
    std::vector<glm::vec2> points;
};


struct Svg
{
    std::vector<SvgLine> lines;

    Svg& add_line(std::string_view color, const std::vector<glm::vec2>& points, std::string label = "");

    void write(const std::string& file_path, float spacing);
};
