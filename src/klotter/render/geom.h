#pragma once

namespace klotter
{

/** \addtogroup geom Geom
 *  @{
*/

/// A single vertex in a Geom
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color;
};

/// A triangulated face in a Geom
struct Face
{
	u32 a;
	u32 b;
	u32 c;
};

/// vertices and triangles
struct Geom
{
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
};

/**
 * @}
*/

}  //  namespace klotter
