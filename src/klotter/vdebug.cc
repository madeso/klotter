#include "klotter/vdebug.h"
#include <fstream>

namespace VisualDebugging
{

AABB2::AABB2()
	: min(std::nullopt)
	, max(std::nullopt)
{
}

AABB2::AABB2(const glm::vec2& f)
	: min(f)
	, max(f)
{
}

void AABB2::include(const glm::vec2& p)
{
	if (min)
	{
		min->x = std::min(p.x, min->x);
		min->y = std::min(p.y, min->y);
	}
	else
	{
		min = p;
	}

	if (max)
	{
		max->x = std::max(p.x, max->x);
		max->y = std::max(p.y, max->y);
	}
	else
	{
		max = p;
	}
}

SceneArtist::SceneArtist()
	: active_draw_color_(Colors::lightRed)
	, inactive_draw_color_(Colors::darkRed)
{
}

void SceneArtist::SetColor(const Color& act, const Color& bkg)
{
	active_draw_color_ = act;
	inactive_draw_color_ = bkg;
}

std::string hex(const Color& color)
{
	char buf[12];
	snprintf(buf, sizeof(buf), "#%02X%02X%02X", color.r, color.g, color.b);
	return buf;
}

void SceneArtist::WriteJsonBase(std::ostream& file) const
{
	file << "                        showWhenInBackground: " << (show_when_in_background ? "true":"false") << ",\n";
	file << "                        activeDrawColor: \"" << hex(active_draw_color_) << "\",\n";
	file << "                        inactiveDrawColor: \"" << hex(inactive_draw_color_)<< "\",\n";
}

Frame::Frame(const std::string& the_description, bool do_not_erase)
	: description(the_description)
	, keep_in_background(do_not_erase)
{
}

void Frame::AddArtist(std::unique_ptr<SceneArtist> artist)
{
	artists.emplace_back(std::move(artist));
}

AABB2 aabb_from_frames(const std::vector<Frame>& frames, std::size_t selected_frame_index)
{
	AABB2 r;
	for (std::size_t my_frame_index = 0; my_frame_index <= selected_frame_index && my_frame_index < frames.size();
		 my_frame_index += 1)
	{
		const auto& frame = frames[my_frame_index];

		const auto is_current_frame = selected_frame_index == my_frame_index;
		const auto show_frame = is_current_frame || (frame.keep_in_background && selected_frame_index > my_frame_index);
		if (! show_frame)
		{
			continue;
		}

		for (const auto& artist: frame.artists)
		{
			if (is_current_frame || artist->show_when_in_background)
			{
				artist->Include(&r);
			}
		}
	}
	return r;
}

void VisualDebug::add_artist_to_current_frame(std::unique_ptr<SceneArtist> artist)
{
	if (frames.empty())
	{
		begin_frame();
	}

	artist->SetColor(current_active_color, current_background_color);
	artist->show_when_in_background = ! don_not_show_next_element_when_frame_is_in_background;
	frames.rbegin()->AddArtist(std::move(artist));
}

/// Save visual debug data. Call this when finished creating frames.
void VisualDebug::save(const std::string& title)
{
	std::ofstream file((title + ".html").c_str());

	file << "<!DOCTYPE html>\n";
	file << "<html lang=\"en\">\n";
	file << "<head>\n";
	file << "    <meta charset=\"UTF-8\">\n";
	file << "    <title>" << title << "</title>\n";
	file << "    <style>\n";
	file << "        html, body {\n";
	file << "		    height: 100%;\n";
	file << "		    margin: 0;\n";
	file << "		    padding: 0;\n";
	file << "		}\n";
	file << "		body {\n";
	file << "		    background: #222;\n";
	file << "		    color: #fff;\n";
	file << "		    font-family: 'Segoe UI', 'Roboto', Arial, sans-serif;\n";
	file << "		    height: 100vh;\n";
	file << "		    overflow: hidden;\n";
	file << "		}\n";
	file << "		#canvas {\n";
	file << "		    position: absolute;\n";
	file << "		    top: 0; left: 0;\n";
	file << "		    width: 100vw;\n";
	file << "		    height: 100vh;\n";
	file << "		    border: none;\n";
	file << "		    margin: 0;\n";
	file << "		    padding: 0;\n";
	file << "		    display: block;\n";
	file << "		    background: #222;\n";
	file << "		}\n";
	file << "		#ui {\n";
	file << "		    position: absolute;\n";
	file << "		    top: 24px;\n";
	file << "		    left: 24px;\n";
	file << "		    background: rgba(30, 30, 40, 0.95);\n";
	file << "		    color: #fff;\n";
	file << "		    border-radius: 16px;\n";
	file << "		    box-shadow: 0 4px 24px rgba(44, 62, 80, 0.18);\n";
	file << "		    padding: 14px;\n";
	file << "		    z-index: 20;\n";
	file << "		    min-width: 320px;\n";
	file << "		    max-width: 420px;\n";
	file << "		}\n";
	file << "		#ui label, #ui span, #ui input, #ui p {\n";
	file << "		    color: #fff;\n";
	file << "		}\n";
	file << "		#ui p { margin : 0; padding : 3px; }\n";
	file << "		#ui h1 { margin : 0; padding : 0px }\n";
	file << "		.play-button {\n";
	file << "		    background: linear-gradient(90deg, #00a8ff 0%, #9c88ff 100%);\n";
	file << "		    color: #fff;\n";
	file << "		    padding: 8px 9px;\n";
	file << "		    border: none;\n";
	file << "		    border-radius: 6px;\n";
	file << "		    font-size: 0.88rem;\n";
	file << "		    font-weight: 500;\n";
	file << "		    cursor: pointer;\n";
	file << "		    transition: background 0.2s, box-shadow 0.2s;\n";
	file << "		    box-shadow: 0 1px 4px rgba(44, 62, 80, 0.08);\n";
	file << "		}\n";
	file << "		.play-button:hover {\n";
	file << "		    background: linear-gradient(90deg, #9c88ff 0%, #00a8ff 100%);\n";
	file << "		    box-shadow: 0 2px 8px rgba(44, 62, 80, 0.12);\n";
	file << "		}\n";
	file << "		.play-button:active {\n";
	file << "		    background: #273c75;\n";
	file << "		}\n";
	file << "		label {\n";
	file << "		    font-weight: 500;\n";
	file << "		    margin-right: 8px;\n";
	file << "		}\n";
	file << "		input[type=\"range\"] {\n";
	file << "		    width: 180px;\n";
	file << "		    margin: 0 8px;\n";
	file << "		}\n";
	file << "		#status, #description {\n";
	file << "		    display: inline-block;\n";
	file << "		    margin: 8px 0;\n";
	file << "		    font-size: 1.1rem;\n";
	file << "		}\n";
	file << "		#hover {\n";
	file << "		    position: fixed;\n";
	file << "		    top: 0;\n";
	file << "		    left: 0;\n";
	file << "		    margin: 0;\n";
	file << "		    padding: 4px 12px;\n";
	file << "		    background: rgba(255,255,255,0.98);\n";
	file << "		    color: #222;\n";
	file << "		    border: 1px solid #dcdde1;\n";
	file << "		    border-radius: 6px;\n";
	file << "		    font-family: monospace;\n";
	file << "		    font-size: 15px;\n";
	file << "		    pointer-events: none;\n";
	file << "		    display: none;\n";
	file << "		    z-index: 30;\n";
	file << "		    box-shadow: 0 2px 8px rgba(44, 62, 80, 0.08);\n";
	file << "		}\n";
	file << "		@media (max-width: 600px) {\n";
	file << "		    #ui {\n";
	file << "		        min-width: 0;\n";
	file << "		        max-width: 98vw;\n";
	file << "		        padding: 12px 2vw;\n";
	file << "		        left: 2vw;\n";
	file << "		        top: 2vw;\n";
	file << "		    }\n";
	file << "		}\n";
	file << "    </style>\n";
	file << "    <script src=\"vdebug.js\"></script>\n";
	file << "</head>\n";
	file << "<body>\n";
	file << "    <canvas id=\"canvas\">Canvas not supported</canvas>\n";
	file << "    <div id=\"ui\">\n";
	file << "        <h1>" << title << "</h1>\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"first-frame\" value=\"|<\" />\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"previous-frame\" value=\"<\" />\n";
	file << "        <input type=\"range\" id=\"frame\" name=\"frame\" min=\"10\" max=\"100\" />\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"next-frame\" value=\">\" />\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"last-frame\" value=\">|\" />\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"focus\" value=\"F\" />\n";
	file << "        <p>\n";
	file << "             <span id=\"status\">status</span>\n";
	file << "             <span id=\"description\">description</span>\n";
	file << "         </p>\n";
	file << "	</div>\n";
	file << "    <p id=\"hover\"></p>\n";
	file << "    <script>\n";
	file << "        const frames = [\n";
	std::size_t frame_index = 0;
	for (const auto& f: frames)
	{
		const auto current_frame_index = frame_index;
		frame_index += 1;
		if (current_frame_index != 0)
		{
			file << ",\n";
		}
		file << "            {\n";
		file << "                description: \""<< f.description << "\",\n";
		file << "                keepInBackground: " << (f.keep_in_background ? "true" : "false") << ",\n";
		const auto aabb = aabb_from_frames(frames, current_frame_index);
		if (aabb.min && aabb.max)
		{
			file << "                aabb: [" << aabb.min->x << ", " << aabb.min->y << ", " << aabb.max->x << ", " << aabb.max->y << "],\n";
		}
		file << "                artists: [\n";
		bool first_artist = true;
		for (const auto& a: f.artists)
		{
			if (first_artist)
			{
				first_artist = false;
			}
			else
			{
				file << ",\n";
			}
			a->WriteJson(file);
		}
		file << "                ]\n";
		file << "            }\n";
	}
	file << "        ]\n";
	file << "	        window.register_vdebug2({\n";
	file << "            canvas: document.getElementById(\"canvas\"),\n";
	file << "            focus: document.getElementById(\"focus\"),\n";
	file << "            status: document.getElementById(\"status\"),\n";
	file << "            description: document.getElementById(\"description\"),\n";
	file << "            hover: document.getElementById(\"hover\"),\n";
	file << "            first_frame: document.getElementById(\"first-frame\"),\n";
	file << "            previous_frame: document.getElementById(\"previous-frame\"),\n";
	file << "            frame: document.getElementById(\"frame\"),\n";
	file << "            next_frame: document.getElementById(\"next-frame\"),\n";
	file << "            last_frame: document.getElementById(\"last-frame\"),\n";
	file << "        }, frames);\n";
	file << "    </script>\n";
	file << "</body>\n";
	file << "</html>\n";
}

void VisualDebug::begin_frame(std::string description, bool keep_in_background)
{
	don_not_show_next_element_when_frame_is_in_background = false;
	frames.emplace_back(description, keep_in_background);
}

void VisualDebug::do_not_show_next_element_when_frame_is_in_background()
{
	don_not_show_next_element_when_frame_is_in_background = true;
}

void VisualDebug::set_color(const Color& new_active_color, const Color& new_background_color)
{
	current_active_color = new_active_color;
	current_background_color = new_background_color;
}

void VisualDebug::set_color(const Color& color)
{
	set_color(color, color);
}

void VisualDebug::set_default_font_size(int new_font_size)
{
	current_font_size = new_font_size;
}

void VisualDebug::reset_default_font_size()
{
	current_font_size = DEFAULT_FONT_SIZE;
}


struct LineArtist : public SceneArtist
{
	LineArtist(const std::vector<glm::vec2>& p)
		: points(p)
	{
	}
	std::vector<glm::vec2> points;

	void WriteJson(std::ostream& file) const override
	{
		file << "                    {\n";
		WriteJsonBase(file);
		file << "                        type: \"line\",\n";
		file << "                        points: [";
		bool first = true;
		for (const auto& p: points)
		{
			if (first)
			{
				first = false;
			}
			else
			{
				file << ", ";
			}
			file << "{x:" << p.x << ",y:" << p.y << "}";
		}
		file << "]\n";
		file << "                    }\n";
	}

	void Include(AABB2* aabb) override
	{
		for (const auto& p: points)
		{
			aabb->include(p);
		}
	}
};

struct ArrowArtist : public SceneArtist
{
	ArrowArtist(const glm::vec2& start, const glm::vec2& end, float s)
		: from(start)
		, to(end)
		, size(s)
	{
	}

	glm::vec2 from;
	glm::vec2 to;
	float size;

	void WriteJson(std::ostream& file) const override
	{
		file << "                    {\n";
		WriteJsonBase(file);
		file << "                        type: \"arrow\",\n";
		file << "                        x1: " << from.x << ", y1: " << from.y << ",\n";
		file << "                        x2: " << to.x << ", y2: " << to.y << ",\n";
		file << "                        size: " << size << ",\n";
		file << "                    }\n";
	}

	void Include(AABB2* aabb) override
	{
		aabb->include(from);
		aabb->include(to);
	}
};

struct TextArtist : public SceneArtist
{
	TextArtist(const glm::vec2& pos, const std::string& txt)
		: position(pos)
		, text(txt)
	{
	}
	glm::vec2 position;
	std::string text;

	void WriteJson(std::ostream& file) const override
	{
		file << "                    {\n";
		WriteJsonBase(file);
		file << "                        type: \"text\",\n";
		file << "                        x: " << position.x << ", y: " << position.y << ",\n";
		file << "                        text: \"" << text << "\",\n";
		file << "                    }\n";
	}

	void Include(AABB2* aabb) override
	{
		aabb->include(position);
	}
};

struct PointArtist : public SceneArtist
{
	PointArtist(const glm::vec2& pos, float rad, bool wire)
		: position(pos)
		, radius(rad)
		, wireframe(wire)
	{
	}
	glm::vec2 position;
	float radius;
	bool wireframe;

	void WriteJson(std::ostream& file) const override
	{
		file << "                    {\n";
		WriteJsonBase(file);
		file << "                        type: \"" << (wireframe ? "point" : "fillpoint") << "\",\n";
		file << "                        x: " << position.x << ", y: " << position.y << ",\n";
		file << "                        radius: " << radius << ",\n";
		file << "                    }\n";
	}

	void Include(AABB2* aabb) override
	{
		aabb->include(position);
	}
};


struct PlotArtist : public SceneArtist
{

	PlotArtist(const PlotRange& r, const std::vector<float>& v)
		: range(r)
		, values(v)
	{
	}

	PlotRange range;
	std::vector<float> values;

	void WriteJson(std::ostream& file) const override
	{
		file << "                    {\n";
		WriteJsonBase(file);
		file << "                        type: \"plot\",\n";
		file << "                        start: " << range.start
									<< ", end: " << range.end
									<< ", step: " << range.step << ",\n";
		file << "                        values: [";
		bool first = true;
		for (auto& v: values)
		{
			if (first)
			{
				first = false;
			}
			else
			{
				file << ", ";
			}
			file << v;
		}
		file << "],\n";
		file << "                    }\n";
	}

	void Include(AABB2* aabb) override
	{
		float x = range.start;
		for (auto y: values)
		{
			aabb->include({x, y});
			x += range.step;
		}
	}
};


void VisualDebug::draw_line_segment_with_label(const glm::vec2& line_start, const glm::vec2& line_end, const std::string& text)
{
	add_artist_to_current_frame(std::make_unique<LineArtist>(std::vector{line_start, line_end}));
	add_artist_to_current_frame(std::make_unique<TextArtist>((line_start + line_end) * 0.5f, text));
}

void VisualDebug::draw_line(const std::vector<glm::vec2>& points)
{
	add_artist_to_current_frame(std::make_unique<LineArtist>(points));
}

void VisualDebug::draw_point(const glm::vec2& position, float radius, bool wireframe)
{
	add_artist_to_current_frame(std::make_unique<PointArtist>(position, radius, wireframe));
}


void VisualDebug::draw_points(const std::vector<glm::vec2>& points, float radius, bool wireframe)
{
	for (const auto&p : points)
	{
		add_artist_to_current_frame(std::make_unique<PointArtist>(p, radius, wireframe));
	}
}


void VisualDebug::draw_arrow(const glm::vec2& start, const glm::vec2& end, float size)
{
	add_artist_to_current_frame(std::make_unique<ArrowArtist>(start, end, size));
}

void VisualDebug::draw_plot(const PlotRange& range, const std::vector<float>& values)
{
	add_artist_to_current_frame(std::make_unique<PlotArtist>(range, values));
}


}  //  namespace VisualDebugging
