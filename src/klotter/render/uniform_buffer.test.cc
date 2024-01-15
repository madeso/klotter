#include "catch.hpp"
// #include "catchy/vectorequals.h"
#include "catchy/stringeq.h"

#include "klotter/render/uniform_buffer.h"


using namespace klotter;

#if 0

template<>
struct fmt::formatter<VertexType> : formatter<string_view>
{
	template<typename FormatContext>
	auto format(VertexType c, FormatContext& ctx)
	{
		string_view name = "<unknown>";
		switch (c)
		{
		case VertexType::position2xy: name = "position2xy"; break;
		case VertexType::position2xz: name = "position2xz"; break;
		case VertexType::position3: name = "position3"; break;
		case VertexType::normal3: name = "normal3"; break;
		case VertexType::color3: name = "color3"; break;
		case VertexType::color4: name = "color4"; break;
		case VertexType::texture2: name = "texture2"; break;
		}
		return formatter<string_view>::format(name, ctx);
	}
};

namespace
{
bool is_equal(const CompiledVertexElement& lhs, const CompiledVertexElement& rhs)
{
	return lhs.type == rhs.type && lhs.name == rhs.name && lhs.index == rhs.index;
}

bool is_equal(const CompiledVertexElementNoName& lhs, const CompiledVertexElementNoName& rhs)
{
	return lhs.type == rhs.type && lhs.index == rhs.index;
}

catchy::FalseString is_equal(
	const std::vector<CompiledVertexElement> lhs, const std::vector<CompiledVertexElement>& rhs
)
{
	return catchy::VectorEquals(
		lhs,
		rhs,
		[](const CompiledVertexElement& f) -> std::string
		{ return fmt::format("{} {} ({})", f.type, f.name, f.index); },
		[](const CompiledVertexElement& a, const CompiledVertexElement& b) -> catchy::FalseString
		{
			if (is_equal(a, b))
			{
				return catchy::FalseString::True();
			}
			else
			{
				return catchy::FalseString::False(fmt::format(
					"{}!={} {}!={} ({}!={})", a.type, b.type, a.name, b.name, a.index, b.index
				));
			}
		}
	);
}

catchy::FalseString is_equal(
	const std::vector<CompiledVertexElementNoName> lhs,
	const std::vector<CompiledVertexElementNoName>& rhs
)
{
	return catchy::VectorEquals(
		lhs,
		rhs,
		[](const CompiledVertexElementNoName& f) -> std::string
		{ return fmt::format("{} ({})", f.type, f.index); },
		[](const CompiledVertexElementNoName& a,
		   const CompiledVertexElementNoName& b) -> catchy::FalseString
		{
			if (is_equal(a, b))
			{
				return catchy::FalseString::True();
			}
			else
			{
				return catchy::FalseString::False(
					fmt::format("{}!={} ({}!={})", a.type, b.type, a.index, b.index)
				);
			}
		}
	);
}

catchy::FalseString is_equal(const std::vector<VertexType> lhs, const std::vector<VertexType>& rhs)
{
	return catchy::VectorEquals(
		lhs,
		rhs,
		[](const VertexType& f) -> std::string { return fmt::format("{}", f); },
		[](const VertexType& a, const VertexType& b) -> catchy::FalseString
		{
			if (a == b)
			{
				return catchy::FalseString::True();
			}
			else
			{
				return catchy::FalseString::False(fmt::format("{} != {}", a, b));
			}
		}
	);
}

catchy::FalseString is_equal(
	const CompiledShaderVertexAttributes& lhs, const CompiledShaderVertexAttributes& rhs
)
{
	const auto same_elements = is_equal(lhs.elements, rhs.elements);
	const auto same_debug_types = is_equal(lhs.debug_types, rhs.debug_types);

	return catchy::FalseString::Combine(same_elements, same_debug_types);
}

catchy::FalseString is_equal(
	const CompiledGeomVertexAttributes& lhs, const CompiledGeomVertexAttributes& rhs
)
{
	const auto same_elements = is_equal(lhs.elements, rhs.elements);
	const auto same_debug_types = is_equal(lhs.debug_types, rhs.debug_types);

	return catchy::FalseString::Combine(same_elements, same_debug_types);
}
}  //  namespace

#endif

int offset_of(const CompiledUniformBufferDescription& compiled, const std::string& name)
{
	auto found = compiled.props.find(name);
	if (found == compiled.props.end()) return -42;
	return found->second.offset;
}

TEST_CASE("uniform_buffer_test_simple", "[uniform_buffer]")
{
	const auto compiled = compile(UniformBufferDescription{
		{UniformType::float_type, "value", 1},
		{UniformType::vec3, "vector", 1},
		{UniformType::mat4, "matrix", 1},
		{UniformType::float_type, "values", 3},
		{UniformType::bool_type, "boolean", 1},
		{UniformType::int_type, "integer", 1}});

	CHECK(offset_of(compiled, "value") == 0);
	CHECK(offset_of(compiled, "vector") == 16);
	CHECK(offset_of(compiled, "matrix") == 32);
	CHECK(offset_of(compiled, "values") == 96);
	CHECK(offset_of(compiled, "boolean") == 144);
	CHECK(offset_of(compiled, "integer") == 148);
	CHECK(compiled.total_size == 152);
}

TEST_CASE("uniform_buffer_test_basic_mat", "[uniform_buffer]")
{
	const auto desc = UniformBufferDescription{
		{UniformType::mat4, "projection", 1}, {UniformType::mat4, "view", 1}};
	const auto compiled = compile(desc);

	CHECK(offset_of(compiled, "projection") == 0);
	CHECK(offset_of(compiled, "view") == 64);
	CHECK(compiled.total_size == 128);

	CHECK(catchy::StringEq(
		to_source("D", desc),
		"layout (std140) uniform D\n"
		"{\n"
		"\tmat4 projection;\n"
		"\tmat4 view;\n"
		"};\n"
	));
}
