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

enum class ExtractedAttributeType
{
	Float
};

struct ExtractedAttribute
{
	ExtractedAttributeType type;
	int count;
	std::size_t size;
};

/// Extracted data from a Geom for a specific shader layout so a CompiledGeom can be created
struct ExtractedGeom
{
	std::vector<char> data;
	std::size_t stride;

	std::vector<ExtractedAttribute> attributes;
	std::vector<u32> indices;
	i32 face_size;
};

ExtractedGeom extract_geom(const Geom& geom, const CompiledGeomVertexAttributes& layout);

/**
 * @}
*/

}  //  namespace klotter
