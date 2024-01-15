#include "klotter/render/uniform_buffer.h"

#include "klotter/assert.h"

namespace klotter
{

/// returns the size in bytes
int size_of(UniformProp prop, bool align)
{
	constexpr int n = 4;  // in bytes
	constexpr int n2 = n * 2;
	constexpr int n4 = n * 4;
	switch (prop.array_count == 1 ? prop.type : UniformType::vec4)
	{
	case UniformType::bool_type: return n;
	case UniformType::int_type: return n;
	case UniformType::float_type: return n;
	case UniformType::vec2: return n2;
	case UniformType::vec3: return n4;
	case UniformType::vec4: return n4;
	case UniformType::mat4: return n4 * (align ? 1 : 4);
	default: DIE("invalid uniform type"); return 0;
	}
}

int calculate_alignment_bytes(int current_size, UniformProp prop)
{
	const auto size = size_of(prop, true);
	const auto mod = current_size % size;
	if (mod == 0) return 0;
	const auto to_add = size - mod;
	ASSERT((to_add + mod) % size == 0);
	return to_add;
}

CompiledUniformBufferDescription compile(const UniformBufferDescription& desc)
{
	auto compiled = CompiledUniformBufferDescription{};
	for (const auto& p: desc)
	{
		const auto size = size_of(p, false);
		compiled.total_size += calculate_alignment_bytes(compiled.total_size, p);
		compiled.props[p.name] = {compiled.total_size, p.type, p.array_count};
		compiled.total_size += size * p.array_count;
	}
	return compiled;
}

std::string to_source(const std::string& name, const UniformBufferDescription& desc)
{
	const auto type_to_string = [](UniformType t)
	{
		switch (t)
		{
		case UniformType::bool_type: return "bool";
		case UniformType::int_type: return "int";
		case UniformType::float_type: return "float";
		case UniformType::vec2: return "vec2";
		case UniformType::vec3: return "vec3";
		case UniformType::vec4: return "vec4";
		case UniformType::mat4: return "mat4";
		default: DIE("invalid uniform type"); return "unhandled_type";
		}
	};
	std::ostringstream ss;
	ss << "layout (std140) uniform " << name << "\n";
	ss << "{"
	   << "\n";
	for (const auto& p: desc)
	{
		ss << "\t" << type_to_string(p.type) << " " << p.name;
		if (p.array_count > 1)
		{
			ss << "[" << p.array_count << "]";
		}
		ss << ";\n";
	}
	ss << "};"
	   << "\n";
	return ss.str();
}

}  //  namespace klotter
