#pragma once

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

struct DebugLine
{
	glm::vec3 from;
	glm::vec3 to;
	glm::vec3 color;
};

struct DebugRender
{
	std::vector<DebugLine> lines;

	void add_line(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);
};

/**
 * @}
*/

}  //  namespace klotter
