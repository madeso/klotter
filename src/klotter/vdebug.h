#pragma once


#include <string>
#include <vector>

namespace VisualDebugging
{
struct Color
{
	unsigned int r;
	unsigned int g;
	unsigned int b;

	constexpr Color(unsigned int red, unsigned int green, unsigned int blue)
		: r(red)
		, g(green)
		, b(blue)
	{
	}
};

namespace Colors
{
	constexpr Color lightRed = Color{0xF9, 0x40, 0x40};
	constexpr Color darkRed = Color{0x87, 0x0C, 0x0C};
	constexpr Color lightBlue = Color{0x1D, 0x92, 0xF4};
	constexpr Color darkBlue = Color{0x1B, 0x36, 0x9A};
	constexpr Color lightGreen = Color{0x19, 0xF4, 0x3F};
	constexpr Color darkGreen = Color{0x11, 0x67, 0x20};
	constexpr Color lightGrey = Color{0xA7, 0xA7, 0xA7};
	constexpr Color darkGrey = Color{0x5C, 0x5C, 0x5C};
	constexpr Color veryDarkGrey = Color{0x1C, 0x1C, 0x1C};
	constexpr Color white = Color{0xFF, 0xFF, 0xFF};
	constexpr Color black = Color{0x00, 0x00, 0x00};
}  //  namespace Colors


struct AABB2
{
	std::optional<glm::vec2> min;
	std::optional<glm::vec2> max;

	AABB2();
	explicit AABB2(const glm::vec2& f);
	void include(const glm::vec2& p);
};


struct SceneArtist
{
	bool show_when_in_background = true;

	void SetColor(const Color& act, const Color& bkg);

	SceneArtist();
	virtual ~SceneArtist() = default;
	SceneArtist(const SceneArtist&) = delete;
	SceneArtist& operator=(const SceneArtist&) = delete;
	SceneArtist(SceneArtist&&) = default;
	SceneArtist& operator=(SceneArtist&&) = default;

	void WriteJsonBase(std::ostream& file) const;
	virtual void WriteJson(std::ostream& file) const = 0;
	virtual void Include(AABB2* aabb) = 0;

   protected:

	Color active_draw_color_;
	Color inactive_draw_color_;
};

struct Frame
{
	std::string description;
	std::vector<std::unique_ptr<SceneArtist>> artists;

	Frame(const std::string& the_description, bool do_not_erase);

	void AddArtist(std::unique_ptr<SceneArtist> artist);

	/// should this frame be erased before drawing the next?
	bool keep_in_background;
};

struct VisualDebug
{
	static constexpr int DEFAULT_FONT_SIZE = 18;

	std::vector<Frame> frames;
	Color current_active_color = Colors::white;
	Color current_background_color = Colors::white;
	int current_font_size = DEFAULT_FONT_SIZE;
	bool don_not_show_next_element_when_frame_is_in_background = false;

	void add_artist_to_current_frame(std::unique_ptr<SceneArtist> artist);

	/// Save visual debug data. Call this when finished creating frames.
	void save(const std::string& title);

	/// Begin a new frame.
	/// @param description Description.
	/// @param keep_in_background If true, this frame will remain in the background when the next frames are drawn, i.e. it won't be erased.
	void begin_frame(std::string description = "", bool keep_in_background = false);

	/// The next element drawn to the current frame will not be shown when the frame is in the background.
	/// (Only applicable to frames where keepInBackground is true).
	void do_not_show_next_element_when_frame_is_in_background();

	/// Set the active and inactive colors.
	/// @param new_active_color Color to use when frame is active.
	/// @param new_background_color Color to use when frame is in background (only applies to frames where keepInBackground is true).
	void set_color(const Color& new_active_color, const Color& new_background_color);

	void set_color(const Color& color);

	void set_default_font_size(int new_font_size);

	void reset_default_font_size();


	void draw_line_segment_with_label(const glm::vec2& line_start, const glm::vec2& line_end, const std::string& text);
	void draw_point(const glm::vec2& position, float radius, bool wireframe = false);
	void draw_points(const std::vector<glm::vec2>& points, float radius, bool wireframe = false);
	void draw_arrow(const glm::vec2& start, const glm::vec2& end, float size);

};
}

