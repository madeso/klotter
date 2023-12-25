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
	VertexVector vertices;
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
