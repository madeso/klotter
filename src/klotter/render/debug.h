#pragma once

#include "klotter/render/color.h"

namespace klotter
{
struct CompiledCamera;

/** \addtogroup render Renderer
 *  @{
*/

/// Line + color
struct DebugLine
{
	glm::vec3 from;
	glm::vec3 to;
	Color color;
};

/// Just stores debug render requests and doesn't actually render anything.
/// @see Renderer
struct DebugRender
{
	std::vector<DebugLine> lines;

	void add_line(const glm::vec3& from, const glm::vec3& to, const Color& color);
};

void draw_frustum(DebugRender* debug, const CompiledCamera& camera, const Color& color);

/**
 * @}
*/

}  //  namespace klotter
