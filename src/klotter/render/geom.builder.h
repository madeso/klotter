#pragma once

#include "klotter/colors.h"

namespace klotter
{
struct Geom;
}

namespace klotter::geom
{
using Index = u64;

struct Vertex
{
	Index position;
	Index normal;
	Index texture;
	Index color;

	Vertex(Index vertex, Index normal, Index texture, Index color);
};

struct Triangle
{
	//ccw = frontfacing
	Vertex v0;
	Vertex v1;
	Vertex v2;

	Triangle(Vertex v0, Vertex v1, Vertex v2);
};

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

	Builder& add_triangle(const Triangle& t);
	Builder& add_quad(
		bool reverse, const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3
	);
	Builder& add_face(const std::vector<Vertex>&);

	Builder& replace_with_smooth_normals();
	Builder& move(const glm::vec3& dir);
	Builder& scale(float scale);
	Builder& invert_normals();

	Geom to_geom() const;

	std::vector<Triangle> triangles;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<glm::vec4> colors;
};

Builder create_box(
	float w, float h, float d, bool face_out, const glm::vec3& color = colors::white
);

}  //  namespace klotter::geom
