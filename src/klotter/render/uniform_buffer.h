#pragma once

namespace klotter
{


enum class UniformType
{
	bool_type,
	int_type,
	float_type,
	vec2,
	vec3,
	vec4,
	mat4
};

struct UniformProp
{
	UniformType type;
	std::string name;
	int array_count;
};

/// describes a layout (std140) compatible uniform buffer layout
using UniformBufferDescription = std::vector<UniformProp>;

struct CompiledUniformProp
{
	int offset;
	UniformType type;
	int array_count;
};

struct CompiledUniformBufferDescription
{
	int total_size = 0;
	std::unordered_map<std::string, CompiledUniformProp> props;
};

CompiledUniformBufferDescription compile(const UniformBufferDescription& desc);
std::string to_source(const std::string& name, const UniformBufferDescription& desc);

}  //  namespace klotter
