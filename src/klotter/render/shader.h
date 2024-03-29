#pragma once

#include <unordered_set>

#include "klotter/render/vertex_layout.h"
#include "klotter/render/uniform.h"

namespace klotter
{

struct UniformBufferSetup;

struct ShaderProgram
{
	ShaderProgram(
		const std::string& vertex_source,
		const std::string& fragment_source,
		const CompiledShaderVertexAttributes& layout
	);

	~ShaderProgram();

	ShaderProgram(const ShaderProgram&) = delete;
	void operator=(const ShaderProgram&) = delete;

	ShaderProgram(ShaderProgram&&) noexcept;
	void operator=(ShaderProgram&&) noexcept;

	void clear();

	bool is_loaded() const;
	void use() const;

	Uniform get_uniform(const std::string& name) const;

	// shader needs to be bound
	void set_float(const Uniform& uniform, float value) const;
	void set_vec2(const Uniform& uniform, float x, float y) const;
	void set_vec2(const Uniform& uniform, const glm::vec2& v) const;
	void set_vec3(const Uniform& uniform, float x, float y, float z) const;
	void set_vec3(const Uniform& uniform, const glm::vec3& v) const;
	void set_vec4(const Uniform& uniform, float x, float y, float z, float w) const;
	void set_vec4(const Uniform& uniform, const glm::vec4& v) const;
	void set_texture(const Uniform& uniform) const;
	void set_mat(const Uniform& uniform, const glm::mat4& mat) const;
	void set_mat(const Uniform& uniform, const glm::mat3& mat) const;

	void setup_uniform_block(const UniformBufferSetup& setup);

	unsigned int shader_program;
	VertexTypes debug_vertex_types;
};

void setup_textures(ShaderProgram* shader, const std::vector<Uniform*>& uniform_list);
void set_shader_program(unsigned int new_program, const VertexTypes& types);
bool is_bound_for_shader(const std::unordered_set<VertexType>& debug_shader_types);
bool is_shader_bound(unsigned int program);
void clear_shader_program();

}  //  namespace klotter
