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
	: activeDrawColor(Colors::lightRed)
	, inactiveDrawColor(Colors::darkRed)
{
}

void SceneArtist::SetColor(const Color& act, const Color& bkg)
{
	activeDrawColor = act;
	inactiveDrawColor = bkg;
}

std::string hex(const Color& color)
{
	char buf[12];
	snprintf(buf, sizeof(buf), "#%02X%02X%02X", color.r, color.g, color.b);
	return buf;
}

void SceneArtist::WriteJsonBase(std::ostream& file) const
{
	file << "                        showWhenInBackground: " << (showWhenInBackground ? "true":"false") << ",\n";
	file << "                        activeDrawColor: \"" << hex(activeDrawColor) << "\",\n";
	file << "                        inactiveDrawColor: \"" << hex(inactiveDrawColor)<< "\",\n";
}

Frame::Frame(const std::string& the_description, bool dontErase)
	: description(the_description)
	, keepInBackground(dontErase)
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

		const auto isCurrentFrame = selected_frame_index == my_frame_index;
		const auto showFrame = isCurrentFrame || (frame.keepInBackground && selected_frame_index > my_frame_index);
		if (! showFrame) continue;

		for (const auto& artist: frame.artists)
		{
			if (isCurrentFrame || artist->showWhenInBackground)
			{
				artist->Include(&r);
			}
		}
	}
	return r;
}

void VisualDebug::AddArtistToCurrentFrame(std::unique_ptr<SceneArtist> artist)
{
	if (frames.empty())
	{
		BeginFrame();
	}

	artist->SetColor(currentActiveColor, currentBackgroundColor);
	artist->showWhenInBackground = ! dontShowNextElementWhenFrameIsInBackground;
	frames.rbegin()->AddArtist(std::move(artist));
}

/// Save visual debug data. Call this when finished creating frames.
void VisualDebug::Save(const std::string& title)
{
	// todo(Gustav): move to argument
	const auto width = 400;
	const auto height = 300;

	std::ofstream file((title + ".html").c_str());

	file << "<!DOCTYPE html>\n";
	file << "<html lang=\"en\">\n";
	file << "<head>\n";
	file << "    <meta charset=\"UTF-8\">\n";
	file << "    <title>" << title << "</title>\n";
	file << "    <style>\n";
	file << "        body { background: #ddd; color: #000; }\n";
	file << "        #ui { display : block; margin : 20px auto;\n";
	file << "        	max-width : " << width << "px;\n";
	file << "        }\n";
	file << "        canvas { border: 1px solid #000; display: block; margin: 20px auto; }\n";
	file << "        #hover {\n";
	file << "            position: fixed;\n";
	file << "            top: 0;\n";
	file << "            left: 0;\n";
	file << "            margin: 0;\n";
	file << "            padding: 2px 6px;\n";
	file << "            background: rgba(255,255,255,0.95);\n";
	file << "            border: 1px solid #888;\n";
	file << "            border-radius: 4px;\n";
	file << "            font-family: monospace;\n";
	file << "            font-size: 14px;\n";
	file << "            pointer-events: none;\n";
	file << "            display: none;\n";
	file << "            z-index: 10;\n";
	file << "        }\n";
	file << "        .play-button {\n";
	file << "            background-color: #ccc;\n";
	file << "            color: #000;\n";
	file << "            padding: 6px;\n";
	file << "            border: none;\n";
	file << "        }\n";
	file << "        .play-button:hover {\n";
	file << "            background-color: #aaa;\n";
	file << "        }\n";
	file << "        .play-button:active {\n";
	file << "            background-color: #999;\n";
	file << "        }\n";
	file << "    </style>\n";
	file << "    <script src=\"vdebug.js\"></script>\n";
	file << "</head>\n";
	file << "<body>\n";
	file << "    <div id=\"ui\">\n";
	file << "        <h1>" << title << "</h1>\n";
	file << "        <label for=\"frame\">Frame:</label>\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"first-frame\" value=\"|<\" />\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"previous-frame\" value=\"<\" />\n";
	file << "        <input type=\"range\" id=\"frame\" name=\"frame\" min=\"10\" max=\"100\" />\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"next-frame\" value=\">\" />\n";
	file << "        <input type=\"button\" class=\"play-button\" id=\"last-frame\" value=\">|\" />\n";
	file << "        <p id=\"description\">\n";
	file << "            test\n";
	file << "        </p>\n";
	file << "\n";
	file << "    <canvas id=\"canvas\" width=\"" << width << "\" height=\"" << height << "\">Canvas not supported</canvas>\n";
	file << "    <p id=\"hover\"/>\n";
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
		file << "                keepInBackground: " << (f.keepInBackground ? "true" : "false") << ",\n";
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

void VisualDebug::BeginFrame(std::string description, bool keepInBackground)
{
	dontShowNextElementWhenFrameIsInBackground = false;
	frames.emplace_back(description, keepInBackground);
}

void VisualDebug::DontShowNextElementWhenFrameIsInBackground()
{
	dontShowNextElementWhenFrameIsInBackground = true;
}

void VisualDebug::SetColor(const Color& activeColor, const Color& backgroundColor)
{
	currentActiveColor = activeColor;
	currentBackgroundColor = backgroundColor;
}

void VisualDebug::SetColor(const Color& color)
{
	SetColor(color, color);
}

void VisualDebug::SetDefaultFontSize(int fontSize)
{
	currentFontSize = fontSize;
}

void VisualDebug::ResetDefaultFontSize()
{
	currentFontSize = defaultFontSize;
}


struct LineArtist : public SceneArtist
{
	LineArtist(const glm::vec2& start, const glm::vec2& end)
		: from(start)
		, to(end)
	{
	}
	glm::vec2 from;
	glm::vec2 to;

	void WriteJson(std::ostream& file) const override
	{
		file << "                    {\n";
		WriteJsonBase(file);
		file << "                        type: \"line\",\n";
		file << "                        x1: " << from.x << ", y1: " << from.y << ",\n";
		file << "                        x2: " << to.x << ", y2: " << to.y << ",\n";
		file << "                    }\n";
	}

	void Include(AABB2* aabb) override
	{
		aabb->include(from);
		aabb->include(to);
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

void VisualDebug::DrawLineSegmentWithLabel(const glm::vec2& lineStart, const glm::vec2& lineEnd, const std::string& text)
{
	AddArtistToCurrentFrame(std::make_unique<LineArtist>(lineStart, lineEnd));
	AddArtistToCurrentFrame(std::make_unique<TextArtist>((lineStart + lineEnd) * 0.5f, text));
}


void VisualDebug::DrawPoint(const glm::vec2& position, float radius, bool wireframe)
{
	AddArtistToCurrentFrame(std::make_unique<PointArtist>(position, radius, wireframe));
}


void VisualDebug::DrawPoints(const std::vector<glm::vec2>& points, float radius, bool wireframe)
{
	for (const auto&p : points)
	{
		AddArtistToCurrentFrame(std::make_unique<PointArtist>(p, radius, wireframe));
	}
}


void VisualDebug::DrawArrow(const glm::vec2& start, const glm::vec2& end, float size)
{
	AddArtistToCurrentFrame(std::make_unique<ArrowArtist>(start, end, size));
}

}  //  namespace VisualDebugging
