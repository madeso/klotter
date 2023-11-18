#pragma once

namespace klotter
{

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color;
};

struct Face
{
	u32 a;
	u32 b;
	u32 c;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<Face> faces;
};

}  //  namespace klotter
