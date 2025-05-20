#include "klotter/render/debug.h"

namespace klotter
{

void DebugRender::add_line(
	const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, LineStyle style
)
{
	debug_lines.emplace_back(DebugLine{from, to, color, style});
}


}  //  namespace klotter
