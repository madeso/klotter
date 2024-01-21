#pragma once

namespace klotter
{
struct Geom;
struct CompiledGeomVertexAttributes;
}  //  namespace klotter

namespace klotter
{

/** \addtogroup geom-builder
 *  @{
*/

using VertexVector = std::vector<float>;

/// Extracted data from a Geom for a specific shader layout so a CompiledGeom can be created
struct ExtractedGeom
{
	// todo(Gustav): replace with a byte buffer to support non-float types (like bone assignments)
	VertexVector vertices;

	// todo(Gustav): replace with stride
	std::size_t floats_per_vertex;

	std::vector<int> attribute_counts;
	std::vector<u32> indices;
	i32 face_size;
};

ExtractedGeom extract_geom(const Geom& geom, const CompiledGeomVertexAttributes& layout);

/**
 * @}
*/

}  //  namespace klotter
