#include "klotter/render/geom.extract.h"

#include "klotter/cint.h"
#include "klotter/assert.h"

#include "klotter/render/geom.h"
#include "klotter/render/vertex_layout.h"

#include <numeric>

namespace klotter
{

std::vector<u32> compile_indices(const Geom& geom)
{
	std::vector<u32> indices;
	const auto push = [&indices](const u32 i)
	{
		indices.emplace_back(i);
	};
	for (const auto& f: geom.faces)
	{
		push(f.a);
		push(f.b);
		push(f.c);
	}
	return indices;
}

void push2(VertexVector* vv, const glm::vec2& p)
{
	vv->emplace_back(p.x);
	vv->emplace_back(p.y);
}

void push3(VertexVector* vv, const glm::vec3& p)
{
	vv->emplace_back(p.x);
	vv->emplace_back(p.y);
	vv->emplace_back(p.z);
}

void push4(VertexVector* vv, const glm::vec4& p)
{
	vv->emplace_back(p.x);
	vv->emplace_back(p.y);
	vv->emplace_back(p.z);
	vv->emplace_back(p.w);
}

struct BufferData
{
	using PerVertex = void (*)(std::vector<float>*, const Vertex&);

	std::vector<int> count;
	std::vector<PerVertex> per_vertex;
};

ExtractedGeom extract_geom(const Geom& geom, const CompiledGeomVertexAttributes& layout)
{
	const auto indices = compile_indices(geom);

	const auto attribute_descriptions = [&]()
	{
		auto data = BufferData{};
		data.count.reserve(layout.elements.size());
		data.per_vertex.reserve(layout.elements.size());

		for (const auto& element: layout.elements)
		{
			switch (element.type)
			{
#define MAP(VT, PROP, COUNT) \
	case VT: \
		data.count.emplace_back(COUNT); \
		data.per_vertex.emplace_back([](VertexVector* vertices, const Vertex& vertex) \
									 { push##COUNT(vertices, PROP); }); \
		break
				MAP(VertexType::position2xy, (glm::vec2{vertex.position.x, vertex.position.y}), 2);
				MAP(VertexType::position2xz, (glm::vec2{vertex.position.x, vertex.position.z}), 2);
				MAP(VertexType::position3, vertex.position, 3);
				MAP(VertexType::normal3, vertex.normal, 3);
				MAP(VertexType::color3, vertex.color, 3);
				MAP(VertexType::color4, glm::vec4(vertex.color, 1.0f), 4);
				MAP(VertexType::texture2, vertex.uv, 2);
#undef MAP
			default: DIE("Invalid buffer type"); break;
			}
		}
		return data;
	}();

	const auto floats_per_vertex = Cint_to_sizet(std::accumulate(
		attribute_descriptions.count.begin(),
		attribute_descriptions.count.end(),
		0,
		[](auto s, const auto& d) { return s + d; }
	));
	const auto vertices = [&]()
	{
		auto verts = VertexVector{};
		verts.reserve(geom.vertices.size() * floats_per_vertex);
		for (const auto& vertex: geom.vertices)
		{
			for (const auto& per_vertex: attribute_descriptions.per_vertex)
			{
				per_vertex(&verts, vertex);
			}
		}
		return verts;
	}();

	const auto face_size = static_cast<i32>(geom.faces.size());

	return {vertices, floats_per_vertex, attribute_descriptions.count, indices, face_size};
}

}  //  namespace klotter
