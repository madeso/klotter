#include "klotter/render/geom.builder.h"

#include "klotter/assert.h"
#include "klotter/cint.h"

#include "klotter/render/geom.h"

#include <fstream>
#include <map>

namespace klotter::geom
{

struct HashCombiner
{
	std::size_t result = 17;

	template<typename T>
	HashCombiner& combine(const T& t)
	{
		// src: https://stackoverflow.com/a/17017281/180307
		// numbers from: https://stackoverflow.com/a/1646913/180307
		// A word of warning, this is (a variation of) the Berstein hash, and because nobody
		// knows why it does well in tests it is not advisable when hashing is critical.
		// See eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
		result = result * 31 + std::hash<T>{}(t);
		return *this;
	}
};

struct Combo
{
	Index position;
	Index texture;
	Index normal;
	Index color;

	Combo(const Vertex& v)
		: position(v.position)
		, texture(v.texture)
		, normal(v.normal)
		, color(v.color)
	{
	}
};

bool operator==(const Combo& lhs, const Combo& rhs)
{
	return lhs.position == rhs.position && lhs.texture == rhs.texture && lhs.normal == rhs.normal
		&& lhs.color == rhs.color;
}

}  //  namespace klotter::geom

template<>
struct std::hash<klotter::geom::Combo>
{
	std::size_t operator()(const klotter::geom::Combo& c) const
	{
		return klotter::geom::HashCombiner{}
			.combine(c.position)
			.combine(c.texture)
			.combine(c.normal)
			.combine(c.color)
			.result;
	}
};

namespace klotter::geom
{

Vertex::Vertex(Index pnt, Index a_color)
	: position(pnt)
	, normal(pnt)
	, texture(pnt)
	, color(a_color)
{
}

Vertex::Vertex(Index a_position, Index a_normal, Index a_texture, Index a_color)
	: position(a_position)
	, normal(a_normal)
	, texture(a_texture)
	, color(a_color)
{
}

Triangle::Triangle(Vertex a, Vertex b, Vertex c)
	: v0(a)
	, v1(b)
	, v2(c)
{
}

// ==================================================================================================================================

namespace
{
	template<typename T>
	Index add_vec(std::vector<T>* v, T t)
	{
		v->emplace_back(t);
		return v->size() - 1;
	}

	template<typename T>
	std::optional<Index> find_vec(const std::vector<T>& v, T t, float diff)
	{
		const auto d2 = diff * diff;
		Index index = 0;
		for (const auto& r: v)
		{
			if (glm::distance2(r, t) < d2)
			{
				return index;
			}
			index += 1;
		}

		return std::nullopt;
	}

	template<typename T>
	Index find_or_add_vec(std::vector<T>* v, T t, float diff)
	{
		const auto found = find_vec(*v, t, diff);
		if (found)
		{
			return *found;
		}

		return add_vec(v, t);
	}
}  //  namespace

Index Builder::add_text_coord(const glm::vec2& tc)
{
	return add_vec(&texcoords, tc);
}

Index Builder::add_position(const glm::vec3& pos)
{
	return add_vec(&positions, pos);
}

Index Builder::add_normal(const glm::vec3& norm)
{
	return add_vec(&normals, norm);
}

Index Builder::add_color(const glm::vec4& c)
{
	return add_vec(&colors, c);
}

Index Builder::foa_text_coord(const glm::vec2& v, float max_diff)
{
	return find_or_add_vec(&texcoords, v, max_diff);
}

Index Builder::foa_position(const glm::vec3& pos, float max_diff)
{
	return find_or_add_vec(&positions, pos, max_diff);
}

Index Builder::foa_normal(const glm::vec3& norm, float max_diff)
{
	return find_or_add_vec(&normals, norm, max_diff);
}

Index Builder::foa_color(const glm::vec4& c, float max_diff)
{
	return find_or_add_vec(&colors, c, max_diff);
}

Builder& Builder::add_triangle(const Triangle& t)
{
	triangles.emplace_back(t);
	return *this;
}

Builder& Builder::add_quad(
	bool ccw, const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3
)
{
	if (ccw)
	{
		// add counter clock wise
		add_triangle(Triangle(v0, v1, v2));
		add_triangle(Triangle(v0, v2, v3));
	}
	else
	{
		// add clock wise
		add_triangle(Triangle(v2, v1, v0));
		add_triangle(Triangle(v3, v2, v0));
	}

	return *this;
}

Builder& Builder::move(const glm::vec3& dir)
{
	for (glm::vec3& p: positions)
	{
		p += dir;
	}

	return *this;
}

Builder& Builder::scale(float scale)
{
	for (glm::vec3& p: positions)
	{
		p *= scale;
	}

	return *this;
}

Builder& Builder::invert_normals()
{
	for (glm::vec3& p: normals)
	{
		p = -p;
	}

	return *this;
}

Builder& Builder::add_face(const std::vector<Vertex>& vertices)
{
	// we currently doesnt support ton-triangular faces so - triangulate it
	const std::vector<Vertex>::size_type size = vertices.size();
	[[maybe_unused]] bool added = false;
	for (std::vector<Vertex>::size_type i = 2; i < size; ++i)
	{
		add_triangle(Triangle(vertices[0], vertices[i - 1], vertices[i]));
		added = true;
	}
	assert(added);

	return *this;
}

glm::vec3 FromTo(const glm::vec3& f, const glm::vec3& t)
{
	return t - f;
}

glm::vec3 crossNorm(const glm::vec3& lhs, const glm::vec3& rhs)
{
	return glm::normalize(glm::cross(lhs, rhs));
}

Builder& Builder::replace_with_smooth_normals()
{
	// start with empty sum, this will become the smooth normals
	std::vector<glm::vec3> vertexNormalsSum(positions.size(), glm::vec3(0, 0, 0));


	for (Triangle& t: triangles)
	{
		const glm::vec3 p0 = positions[t.v0.position];
		const glm::vec3 p1 = positions[t.v1.position];
		const glm::vec3 p2 = positions[t.v2.position];

		const glm::vec3 d0 = FromTo(p1, p0);
		const glm::vec3 d1 = FromTo(p1, p2);

		const glm::vec3 faceNormal = crossNorm(d1, d0);


		auto update_vert = [&](Vertex& vert)
		{
			// add face normal to the vertex normal
			vertexNormalsSum[vert.position] += faceNormal;

			// set the vertex normal to the same index as the position
			vert.normal = vert.position;
		};

		update_vert(t.v0);
		update_vert(t.v1);
		update_vert(t.v2);
	}

	//  normalize the sums
	normals.clear();
	for (const glm::vec3& normalSum: vertexNormalsSum)
	{
		normals.push_back(glm::normalize(normalSum));
	}
	assert(normals.size() == positions.size());

	return *this;
}

Geom Builder::to_geom() const
{
	std::unordered_map<Combo, u32> combinations;

	// foreach triangle
	std::vector<klotter::Vertex> vertices;
	std::vector<klotter::Face> faces;

	auto convert_vert = [&](const Vertex& vert) -> u32
	{
		const Combo c(vert);
		auto result = combinations.find(c);
		if (result != combinations.end())
		{
			return result->second;
		}
		else
		{
			const glm::vec3 pos = positions[c.position];
			const glm::vec2 text = texcoords.empty() ? glm::vec2(0, 0) : texcoords[c.texture];
			const glm::vec4 col = colors.empty() ? glm::vec4{colors::white, 1.0f} : colors[c.color];
			const glm::vec3 normal
				= normals.empty() == false ? normals[c.normal] : glm::vec3(1, 0, 0);
			const auto ind = vertices.size();
			vertices.emplace_back(klotter::Vertex{pos, normal, text, col});
			combinations.insert({c, Csizet_to_u32(ind)});
			return Csizet_to_u32(ind);
		}
	};

	for (const Triangle& tri: triangles)
	{
		const auto v0 = convert_vert(tri.v0);
		const auto v1 = convert_vert(tri.v1);
		const auto v2 = convert_vert(tri.v2);

		// add traingle to geom
		faces.emplace_back(Face{v0, v1, v2});
	}

	return {std::move(vertices), std::move(faces)};
}

Builder& Builder::write_obj(const std::string& path)
{
	std::ofstream f(path.c_str());
	if (f.good() == false)
	{
		DIE("Failed to create file");
		return *this;
	}

	f << "# Vertices\n";
	for (const auto& p: positions)
	{
		f << "v " << p.x << " " << p.y << " " << p.z << '\n';
	}
	f << '\n';

	f << "# Normals\n";
	for (const auto& p: normals)
	{
		f << "vn " << p.x << " " << p.y << " " << p.z << '\n';
	}
	f << '\n';

	f << "# Texcoords\n";
	for (const auto& p: texcoords)
	{
		f << "vt " << p.x << " " << p.y << '\n';
	}
	f << '\n';

	f << "# Triangles\n";
	for (const auto& p: triangles)
	{
		f << "f";
		const std::array<Vertex, 3> t{p.v0, p.v1, p.v2};
		for (const auto& v: t)
		{
			f << ' ' << (v.position + 1) << '/' << (v.texture + 1) << '/' << (v.normal + 1);
		}
		f << '\n';
	}
	f << '\n';

	return *this;
}

// ==================================================================================================================================

Builder create_box(float x, float y, float z, bool invert, const glm::vec3& color)
{
	Builder b;

	// position and texture coord struct
	struct Pt
	{
		glm::vec3 pos;
		glm::vec2 tex;
	};

	const auto add_quad_to_builder = [&](glm::vec3 normal, Pt p0, Pt p1, Pt p2, Pt p3)
	{
		constexpr float pd = 0.1f;
		constexpr float td = 0.01f;
		const auto ci = b.foa_color({color, 1.0f}, 0.001f);
		const auto no = b.add_normal(normal);

		const auto v0 = Vertex{b.foa_position(p0.pos, pd), no, b.foa_text_coord(p0.tex, td), ci};
		const auto v1 = Vertex{b.foa_position(p1.pos, pd), no, b.foa_text_coord(p1.tex, td), ci};
		const auto v2 = Vertex{b.foa_position(p2.pos, pd), no, b.foa_text_coord(p2.tex, td), ci};
		const auto v3 = Vertex{b.foa_position(p3.pos, pd), no, b.foa_text_coord(p3.tex, td), ci};

		b.add_quad(invert, v0, v1, v2, v3);
	};

	// texel scale
	const float ts = 1.0f;

	// half sizes
	const float hx = x * 0.5f;
	const float hy = y * 0.5f;
	const float hz = z * 0.5f;

	const float s = invert ? -1.0f : 1.0f;

	// front
	add_quad_to_builder(
		{0, 0, -s},
		{{-hx, -hy, -hz}, {0.0f, 0.0f}},
		{{hx, -hy, -hz}, {x * ts, 0.0f}},
		{{hx, hy, -hz}, {x * ts, y * ts}},
		{{-hx, hy, -hz}, {0.0f, y * ts}}
	);

	// back
	add_quad_to_builder(
		{0, 0, s},
		{{-hx, -hy, hz}, {0.0f, 0.0f}},
		{{-hx, hy, hz}, {0.0f, y * ts}},
		{{hx, hy, hz}, {x * ts, y * ts}},
		{{hx, -hy, hz}, {x * ts, 0.0f}}
	);

	// left
	add_quad_to_builder(
		{-s, 0, 0},
		{{-hx, hy, -hz}, {y * ts, 0.0f}},
		{{-hx, hy, hz}, {y * ts, z * ts}},
		{{-hx, -hy, hz}, {0.0f, z * ts}},
		{{-hx, -hy, -hz}, {0.0f, 0.0f}}
	);

	// right
	add_quad_to_builder(
		{s, 0, 0},
		{{hx, hy, hz}, {z * ts, y * ts}},
		{{hx, hy, -hz}, {0.0f, y * ts}},
		{{hx, -hy, -hz}, {0.0f, 0.0f}},
		{{hx, -hy, hz}, {z * ts, 0.0f}}
	);

	// bottom
	add_quad_to_builder(
		{0, -s, 0},
		{{-hx, -hy, -hz}, {0.0f, 0.0f}},
		{{-hx, -hy, hz}, {0.0f, z * ts}},
		{{hx, -hy, hz}, {x * ts, z * ts}},
		{{hx, -hy, -hz}, {x * ts, 0.0f}}
	);

	// top
	add_quad_to_builder(
		{0, s, 0},
		{{-hx, hy, -hz}, {0.0f, 0.0f}},
		{{hx, hy, -hz}, {x * ts, 0.0f}},
		{{hx, hy, hz}, {x * ts, z * ts}},
		{{-hx, hy, hz}, {0.0f, z * ts}}
	);

	return b;
}

Builder create_plane(float x, float z, bool invert, const glm::vec3& color)
{
	Builder b;

	// position and texture coord struct
	struct Pt
	{
		glm::vec3 pos;
		glm::vec2 tex;
	};

	const auto add_quad_to_builder = [&](glm::vec3 normal, Pt p0, Pt p1, Pt p2, Pt p3)
	{
		constexpr float pd = 0.1f;
		constexpr float td = 0.01f;
		const auto ci = b.foa_color({color, 1.0f}, 0.001f);
		const auto no = b.add_normal(normal);

		const auto v0 = Vertex{b.foa_position(p0.pos, pd), no, b.foa_text_coord(p0.tex, td), ci};
		const auto v1 = Vertex{b.foa_position(p1.pos, pd), no, b.foa_text_coord(p1.tex, td), ci};
		const auto v2 = Vertex{b.foa_position(p2.pos, pd), no, b.foa_text_coord(p2.tex, td), ci};
		const auto v3 = Vertex{b.foa_position(p3.pos, pd), no, b.foa_text_coord(p3.tex, td), ci};

		b.add_quad(invert, v0, v1, v2, v3);
	};

	// texel scale
	const float ts = 1.0f;

	// half sizes
	const float hx = x * 0.5f;
	const float hz = z * 0.5f;

	const float s = invert ? -1.0f : 1.0f;
	add_quad_to_builder(
		{0, s, 0},
		{{-hx, 0.0f, -hz}, {0.0f, 0.0f}},
		{{hx, 0.0f, -hz}, {x * ts, 0.0f}},
		{{hx, 0.0f, hz}, {x * ts, z * ts}},
		{{-hx, 0.0f, hz}, {0.0f, z * ts}}
	);

	return b;
}

// ==================================================================================================================================


// based on https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
Builder create_uv_sphere(
	float diameter, int longitudes, int latitudes, bool invert, const glm::vec3& color
)
{
	assert(longitudes >= 3);
	assert(latitudes >= 2);

	constexpr float pi = 3.14159265358979323846f;

	Builder ret;
	ret.add_color({color, 1.0f});

	const auto radius = diameter / 2;

	const auto delta_lat = pi / Cint_to_float(latitudes);
	const auto delta_lon = 2 * pi / Cint_to_float(longitudes);

	for (int i = 0; i <= latitudes; ++i)
	{
		const auto lat_angle = pi / 2 - Cint_to_float(i) * delta_lat;
		const auto xy = std::cos(lat_angle);
		const auto z = std::sin(lat_angle);

		for (int j = 0; j <= longitudes; ++j)
		{
			const auto lon_angle = Cint_to_float(j) * delta_lon;

			const auto normal_x = xy * std::cos(lon_angle);
			const auto normal_y = xy * std::sin(lon_angle);
			const auto normal_z = z;

			const auto vertex_s = Cint_to_float(j) / Cint_to_float(longitudes);
			const auto vertex_t = Cint_to_float(i) / Cint_to_float(latitudes);
			ret.add_position({radius * normal_x, radius * normal_y, radius * normal_z});
			ret.add_text_coord({vertex_s, vertex_t});

			const auto normal_scale = invert ? -1.0f : 1.0f;
			ret.add_normal(
				{normal_scale * normal_x, normal_scale * normal_y, normal_scale * normal_z}
			);
		}
	}


	//  Indices
	//  k1--k1+1     a----b
	//  |  / |       |  / |
	//  | /  |       | /  |
	//  k2--k2+1     c----d
	for (Index i = 0; i < static_cast<Index>(latitudes); ++i)
	{
		for (Index j = 0; j < static_cast<Index>(longitudes); ++j)
		{
			const auto k1 = i * (static_cast<Index>(longitudes) + 1) + j;
			const auto k2 = k1 + static_cast<Index>(longitudes) + 1;

			const auto a = k1;
			const auto b = k1 + 1;
			const auto c = k2;
			const auto d = k2 + 1;

			if (i != 0)
			{
				if (invert)
				{
					ret.add_triangle({{a, 0}, {b, 0}, {c, 0}});	 // cw
				}
				else
				{
					ret.add_triangle({{a, 0}, {c, 0}, {b, 0}});	 // ccw
				}
			}

			if (i != static_cast<Index>(latitudes - 1))
			{
				if (invert)
				{
					ret.add_triangle({{b, 0}, {d, 0}, {c, 0}});	 // cw
				}
				else
				{
					ret.add_triangle({{b, 0}, {c, 0}, {d, 0}});	 // ccw
				}
			}
		}
	}

	return ret;
}


}  //  namespace klotter::geom
