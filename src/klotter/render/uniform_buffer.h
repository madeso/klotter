#pragma once

namespace klotter
{


enum class UniformType
{
	invalid,
	bool_type,
	int_type,
	float_type,
	vec2,
	vec3,
	vec4,
	mat4
};

struct UniformBufferSetup
{
	int size = 512;
	int binding_point = 1;
	std::string name;
	std::string source;
};

struct CompiledUniformProp
{
	int offset = -1;
	UniformType type = UniformType::invalid;
	int array_count = -1;
};

/// internal
struct UniformProp
{
	CompiledUniformProp* target;
	UniformType type;
	std::string name;
	int array_count;
};

struct UniformBufferCompiler
{
	void add(
		CompiledUniformProp* target, UniformType type, const std::string& name, int array_count = 1
	);
	void compile(const std::string& name, UniformBufferSetup* target, int binding_point);

	std::vector<UniformProp> props;
};

struct UniformBuffer
{
	explicit UniformBuffer(const UniformBufferSetup& setup);
	~UniformBuffer();

	void set_mat4(const CompiledUniformProp& prop, const glm::mat4& m);

	UniformBuffer(const UniformBuffer&) = delete;
	void operator=(const UniformBuffer&) = delete;

	UniformBuffer(UniformBuffer&&) noexcept;
	UniformBuffer& operator=(UniformBuffer&&) noexcept;

	// clears the loaded buffer to a invalid buffer
	void unload();

	unsigned int id;
};

}  //  namespace klotter
