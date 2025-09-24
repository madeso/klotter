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
	bool showWhenInBackground = true;

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

	Color activeDrawColor;
	Color inactiveDrawColor;
};

struct Frame
{
	std::string description;
	std::vector<std::unique_ptr<SceneArtist>> artists;

	Frame(const std::string& the_description, bool dontErase);

	void AddArtist(std::unique_ptr<SceneArtist> artist);

	/// should this frame be erased before drawing the next?
	bool keepInBackground;
};

struct VisualDebug
{
	static constexpr int defaultFontSize = 18;

	std::vector<Frame> frames;
	Color currentActiveColor = Colors::white;
	Color currentBackgroundColor = Colors::white;
	int currentFontSize = defaultFontSize;
	bool dontShowNextElementWhenFrameIsInBackground = false;

	void AddArtistToCurrentFrame(std::unique_ptr<SceneArtist> artist);

	/// Save visual debug data. Call this when finished creating frames.
	void Save(const std::string& title);

	/// Begin a new frame.
	/// @param description Description.
	/// @param keepInBackground If true, this frame will remain in the background when the next frames are drawn, i.e. it won't be erased.
	void BeginFrame(std::string description = "", bool keepInBackground = false);

	/// The next element drawn to the current frame will not be shown when the frame is in the background.
	/// (Only applicable to frames where keepInBackground is true).
	void DontShowNextElementWhenFrameIsInBackground();

	/// Set the active and inactive colors.
	/// @param activeColor Color to use when frame is active.
	/// @param backgroundColor Color to use when frame is in background (only applies to frames where keepInBackground is true).
	void SetColor(const Color& activeColor, const Color& backgroundColor);

	void SetColor(const Color& color);

	void SetDefaultFontSize(int fontSize);

	void ResetDefaultFontSize();


	void DrawLineSegmentWithLabel(const glm::vec2& lineStart, const glm::vec2& lineEnd, const std::string& text);
	void DrawPoint(const glm::vec2& position, float radius, bool wireframe = false);
	void DrawPoints(const std::vector<glm::vec2>& points, float radius, bool wireframe = false);
	void DrawArrow(const glm::vec2& start, const glm::vec2& end, float size);

};
}

