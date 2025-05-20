#pragma once

#include "klotter/colors.h"

namespace klotter
{
struct Geom;
}

namespace klotter::geom
{

/** \addtogroup geom-builder
 *  @{
*/
using Index = u64;

/// A enum describing if a face is one or two sided
enum class TwoSided
{
	one_sided,
	two_sided
};

/// A single vertex referencing the shared data in the Builder
struct Vertex
{
	Index position;
	Index normal;
	Index texture;
	Index color;

	Vertex(Index vnt, Index color);
	Vertex(Index vertex, Index normal, Index texture, Index color);
};

/// A simple triangle face in a Builder
struct Triangle
{
	// ccw = frontfacing
	Vertex v0;
	Vertex v1;
	Vertex v2;

	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
};

/// A helper utility to create a Geom
struct Builder
{
	Index add_text_coord(const glm::vec2& v);
	Index add_position(const glm::vec3& pos);
	Index add_normal(const glm::vec3& norm);
	Index add_color(const glm::vec4& color);

	// find or add
	Index foa_text_coord(const glm::vec2& v, float max_diff);
	Index foa_position(const glm::vec3& pos, float max_diff);
	Index foa_normal(const glm::vec3& norm, float max_diff);
	Index foa_color(const glm::vec4& color, float max_diff);

	// todo(Gustav): hrm... perhaps change so we support n-gon instead of just tris and just
	// triangulate when converting to a geom? the intent would be clearer in the obj dump...
	Builder& add_triangle(const Triangle& t);
	Builder& add_quad(bool reverse, const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3);
	Builder& add_face(const std::vector<Vertex>&);

	Builder& replace_with_smooth_normals();
	Builder& move(const glm::vec3& dir);
	Builder& scale(float scale);
	Builder& invert_normals();

	Geom to_geom() const;

	Builder& write_obj(const std::string& path);

	std::vector<Triangle> triangles;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec4> colors;
};

Builder create_box(float w, float h, float d, bool invert, const glm::vec3& color = colors::white);
Builder create_xz_plane(float x, float z, bool invert, const glm::vec3& color = colors::white);

Builder create_xy_plane(float x, float y, TwoSided two_sided, const glm::vec3& color = colors::white);

///
/// @param longitude_count number of lines to go vertical, from pole to pole, aka meridians. Must be at least 3.
/// @param lattitude_count number of lines that go horizontal, parallels to the equator, which is one of those lines
Builder create_uv_sphere(
	float diameter, int longitude_count, int lattitude_count, bool invert, const glm::vec3& color = colors::white
);

/**
 * @}
*/

}  //  namespace klotter::geom
