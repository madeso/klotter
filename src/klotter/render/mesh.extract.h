#pragma once

namespace klotter
{
struct Mesh;
struct CompiledGeomVertexAttributes;
}  //  namespace klotter

namespace klotter
{

using VertexVector = std::vector<float>;

/// Extracted data from mesh for OpenGL
struct ExtractedMesh
{
	VertexVector vertices;
	std::size_t floats_per_vertex;
	std::vector<int> attribute_counts;
	std::vector<u32> indices;
	i32 face_size;
};

ExtractedMesh extract_mesh(const Mesh& mesh, const CompiledGeomVertexAttributes& layout);

}  //  namespace klotter
