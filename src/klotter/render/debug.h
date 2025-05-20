#pragma once

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

enum class LineStyle
{
	/// The line is not visible when things are in front it.
	depth,

	/// The line is always visible.
	always_visible,

	/// The line renders fully when in front of objects and dashed when behind things.
	dashed_when_hidden
};

struct DebugLine
{
	glm::vec3 from;
	glm::vec3 to;
	glm::vec3 color;
	LineStyle style;
};

struct DebugRender
{
	std::vector<DebugLine> debug_lines;

	void add_line(
		const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, LineStyle style
	);
};

/**
 * @}
*/

}  //  namespace klotter
