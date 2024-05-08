#pragma once

namespace klotter
{

/** Represents a found shader uniform and created via ShaderProgram::GetUniform()
 */
struct Uniform
{
	std::string name = "<unknown>";
	int location = -1;
	unsigned int debug_shader_program = 0;
	int texture = -1;  ///< The value is >=0 if this is uniform maps to a texture

	/// Creates a invalid uniform
	Uniform() = default;

	Uniform(const std::string& n, int l, unsigned int sp);

	bool is_valid() const;
};

}  //  namespace klotter
