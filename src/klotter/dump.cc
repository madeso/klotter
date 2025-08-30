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

	[[nodiscard]] glm::vec2 calc_size() const
	{
		return max - min;
	}
};

std::optional<AABB2> calc_bounding_box(const Svg& svg)
{
	if (svg.lines.empty())
	{
		return std::nullopt;
	}

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

void Svg::write(const std::string& file_path, float space)
{
    // todo(Gustav): move to argument/settings
	constexpr float width = 800;
	constexpr float height = 600;
	constexpr int stroke_width = 3;
	constexpr std::string_view html_background = "gray";
	constexpr std::string_view canvas_color = "white";

    std::ofstream ofs(file_path);

	const auto bb = calc_bounding_box(*this).value_or(AABB2{{0.0f, 0.0f}});
	const auto size = bb.calc_size();

    const auto offset = bb.min;
    const auto scale = std::min((width-space*2) / size.x, (height-space*2) / size.y);

    const auto dx = offset.x * scale;
    const auto dy = offset.y * scale;
    const auto px = [=](float x) -> float { return static_cast<float>(space) + dx + x * scale; };
    const auto py = [=](float y) -> float { return static_cast<float>(height) - (static_cast<float>(space) + dy + y * scale); };

    ofs << "<!DOCTYPE html>\n";
    ofs << "<html style=\"height: 100%\">\n";
    ofs << "<body style=\"background-color:" << html_background << "; height: 100%\">\n";
    ofs << "<div style=\"display: grid; grid-template-columns: 1fr auto 1fr; grid-template-rows: 1fr auto 1fr; width:100%; height:100%\">\n";
    ofs << "<div style=\"grid-row-start: 2; grid-column-start: 2; position: relative;\">\n";
    ofs << "<svg id=\"svg\" width=\""  << width << "\" height=\"" << height << "\">\n";
    ofs << "<rect width=\"" << width << "\" height=\"" << height << "\""
            " style=\"fill:" << canvas_color << ";stroke-width:0\" />\n";

    for(const auto& line: lines)
    {
        ofs << "<polyline fill=\"none\" stroke=\"" << line.color << "\" stroke-width=\"" << stroke_width << "px\" points=\"";
        for (const auto& p : line.points) {
            // todo(Gustav): this transforms the points... can we use a viewbox or a transform instead and perhaps just scale the line thickness and (future) texts instead?
            ofs << px(p.x) << "," << py(p.y) << " ";
        }
        ofs << "\" />\n";
    }
    
    ofs << "</svg>\n";
	ofs << "</div>\n";
    ofs << "</div>\n";
	ofs << "<p style=\"position: fixed;"
		" top: 0;"
		" left: 0;"
		" margin: 0;"
		" padding: 2px 6px;"
		" background: rgba(255,255,255,0.95);"
		" border: 1px solid #888;"
		" border-radius: 4px;"
		" font-family: monospace;"
		" font-size: 14px;"
		" pointer-events: none;"
		" display: none;"
		" z-index: 10;"
		"\"  id=\"hover\" />\n";

    ofs << "<script type=\"text/javascript\">\n";
	ofs << "const dx = " << dx << ";\n";
	ofs << "const dy = " << dy << ";\n";
	ofs << "const scale = " << scale << ";\n";
	ofs << "const space = " << space << ";\n";
	ofs << "const width = " << width << ";\n";
	ofs << "const height = " << height << ";\n";

    ofs << R"js(
    const hover = document.getElementById('hover');
    const svg = document.getElementById('svg');

    const px = x => (x - (space + dx)) / scale;
    const py = y => (-(y-height)-space-dy)/scale;

    const d2 = (lhs, rhs) => {
        const dx = lhs.x - rhs.x;
        const dy = lhs.y - rhs.y;
        return dx*dx + dy*dy;
    }

    const closest = (polyline, p) => {
        const points = Array.from(polyline.points);
        let closest = null;
        let smallest = 0;
        for(const src of points) {
            const d = d2(p, src);
            if(!closest || smallest > d) {
                closest = src;
                smallest = d;
            }
        }
        return [closest, Math.sqrt(smallest)]
    };

    const lines = Array.from(svg.children).filter(x => x.tagName === 'polyline');
    console.log(lines);
    console.log(closest(lines[0], {x: 100, y:100}));

    const display_hover = (ev) => {
		hover.style.display = 'block';
        let html = '' + px(ev.offsetX) + ' x ' + py(ev.offsetY);
        for(const poly of lines) {
            const [p, dist] = closest(poly, {x: ev.offsetX, y: ev.offsetY});
            if(dist > 10) continue;
            html += '<br /><span style="color:' + poly.attributes.stroke.value + '"">' +
                px(p.x) + ' x ' + py(p.y) + '</span>';
        }
        hover.innerHTML = html;
		hover.style.left = (ev.clientX + 10) + 'px';
        hover.style.top = (ev.clientY + 10) + 'px';
    };
    
    svg.addEventListener('mouseenter', display_hover);
    svg.addEventListener('mousemove', display_hover);
    svg.addEventListener('mouseleave', (ev) => {
        hover.style.display = 'none';
    });
    )js";
    ofs << "</script>\n";
    ofs << "</body>\n";
    ofs << "</html>\n";
}
