#include "klotter/dump.h"

#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

struct AABB2
{
	glm::vec2 min;
	glm::vec2 max;

	explicit AABB2(const glm::vec2& f)
		: min(f)
		, max(f)
	{
	}

	void include(const glm::vec2& p)
	{
		min.x = std::min(p.x, min.x);
		min.y = std::min(p.y, min.y);

		max.x = std::max(p.x, max.x);
		max.y = std::max(p.y, max.y);
	}

	glm::vec2 calc_size() const
	{
		return max - min;
	}
};

std::optional<AABB2> calc_bounding_box(const Svg& svg)
{
	if (svg.lines.empty()) return std::nullopt;

	auto r = AABB2{svg.lines[0].points[0]};

    for(const auto& line: svg.lines)
    {
        for (const auto& p: line.points)
        {
            r.include(p);
        }
    }

	return r;
}

Svg& Svg::add_line(const std::vector<glm::vec2>& points, std::string_view color)
{
	lines.push_back(SvgLine{color, points});
	return *this;
}

void Svg::write(const std::string& file_path, float spacing)
{
    std::ofstream ofs(file_path);

	const auto bb = calc_bounding_box(*this).value_or(AABB2{{0.0f, 0.0f}});
	const auto size = bb.calc_size();

    ofs << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << (size.x + spacing*2)
        << "\" height=\"" << (size.y + spacing*2) << "\" viewBox=\""
        << (bb.min.x - spacing) << " " << (bb.min.y - spacing) << " " << (bb.max.x + spacing) << " " << (bb.max.y + spacing) << "\">\n";

    for(const auto& line: lines)
    {
        ofs << "<polyline fill=\"none\" stroke=\"" << line.color << "\" stroke-width=\"1\" points=\"";
        for (const auto& p : line.points) {
            ofs << p.x << "," << p.y << " ";
        }
        ofs << "\" />\n";
    }
    ofs << "</svg>\n";
}
