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

std::optional<AABB2> calc_bounding_box(const std::vector<glm::vec2>& points)
{
	if (points.empty()) return std::nullopt;

	auto r = AABB2{points[0]};
	for (const auto& p: points)
	{
		r.include(p);
	}

	return r;
}

void write_svg(const std::string& file_path, const std::vector<glm::vec2>& points, float spacing)
{
    std::ofstream ofs(file_path);

	const auto bb = calc_bounding_box(points).value_or(AABB2{{0.0f, 0.0f}});
	const auto size = bb.calc_size();

    ofs << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << (size.x + spacing*2)
        << "\" height=\"" << (size.y + spacing*2) << "\" viewBox=\""
        << (bb.min.x - spacing) << " " << (bb.min.y - spacing) << " " << (bb.max.x + spacing) << " " << (bb.max.y + spacing) << "\">\n";
    ofs << "<polyline fill=\"none\" stroke=\"black\" stroke-width=\"1\" points=\"";
    for (const auto& p : points) {
        ofs << p.x << "," << p.y << " ";
    }
    ofs << "\" />\n";
    ofs << "</svg>\n";
}
