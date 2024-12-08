#pragma once

#include <unordered_set>

#include "klotter/render/vertex_layout.h"
#include "klotter/render/uniform.h"

namespace klotter
{

struct UniformBufferSetup;

/** RAII representation of a open gl shader program.
 */
struct ShaderProgram
{
	/** Compile and link a shader.
     * If there are any errors, they are logged and a zombie object is created instead.
     * @param vertex_source glsl code for the vertex shader
     * @param fragment_source glsl code for the fragment shader
     * @param layout the vertex layout to use
     */
	ShaderProgram(
		const std::string& vertex_source,
		const std::string& fragment_source,
		const CompiledShaderVertexAttributes& layout
	);

	/** Destroy the shader if it's loaded.
	 * @see clear
     */
	~ShaderProgram();

	/// Copying is not allowed.
	ShaderProgram(const ShaderProgram&) = delete;

	/// Copying is not allowed.
	void operator=(const ShaderProgram&) = delete;

	/** Take ownership of another shader.
	 * @param other the shader to take owenership from
     */
	ShaderProgram(ShaderProgram&& other) noexcept;

	/** Take ownership of another shader.
	 * @param other the shader to take owenership from
     */
	void operator=(ShaderProgram&& other) noexcept;

	/** Turn this object into a zombie by destroying it.
      * @note Does nothing if it already is a zombie
     */
	void clear();

	/** Checks if the ShaderProgram is loaded and valid.
     * @return `true` if the program is valid; otherwise, `false`
     */
	bool is_loaded() const;

	/** Activates the ShaderProgram for rendering.
     */
	void use() const;

	/** Get a uniform by name.
     * @param name the name of the uniform in the shader program
     * @return Uniform object representing the location or a zombie if not found
     */
	Uniform get_uniform(const std::string& name) const;

	/** Sets a uniform float value.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform
     * @param value the value
     * @note The shader must be bound before calling this method
     */
	void set_float(const Uniform& uniform, float value) const;

	/** Sets a uniform 2d vector.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform
     * @param x the x value of the vector
     * @param y the y value of the vector
     * @note The shader must be bound before calling this method.
     */
	void set_vec2(const Uniform& uniform, float x, float y) const;

	/** Sets a uniform 2d vector.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform
     * @param v the value
     * @note The shader must be bound before calling this method.
     */
	void set_vec2(const Uniform& uniform, const glm::vec2& v) const;

	/** Sets a uniform 3d vector.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform
     * @param x the x value of the vector
     * @param y the y value of the vector
     * @param z the z value of the vector
     * @note The shader must be bound before calling this method.
     */
	void set_vec3(const Uniform& uniform, float x, float y, float z) const;

	/** Sets a uniform 3d vector.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform
     * @param v the vector
     * @note The shader must be bound before calling this method.
     */
	void set_vec3(const Uniform& uniform, const glm::vec3& v) const;

	/** Sets a uniform 4d vector.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform
     * @param x the x value of the vector
     * @param y the y value of the vector
     * @param z the z value of the vector
     * @param w the w value of the vector
     * @note The shader must be bound before calling this method.
     */
	void set_vec4(const Uniform& uniform, float x, float y, float z, float w) const;

	/** Sets a uniform 4d vector.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform
     * @param v the vector
     * @note The shader must be bound before calling this method.
     */
	void set_vec4(const Uniform& uniform, const glm::vec4& v) const;

	/** Sets a texture for a uniform.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform (must be a texture uniform)
     * @note The shader must be bound before calling this method.
     */
	void set_texture(const Uniform& uniform) const;

	/** Sets a uniform 4x4 matrix.
     * @param uniform the target uniform
     * @param mat the matrix data
     * @note The shader must be bound before calling this method.
     */
	void set_mat(const Uniform& uniform, const glm::mat4& mat) const;

	/** Sets a uniform 3x3 matrix.
	 * Does nothing if the uniform is a zombie.
     * @param uniform the target uniform
     * @param mat the matrix data
     * @note The shader must be bound before calling this method.
     */
	void set_mat(const Uniform& uniform, const glm::mat3& mat) const;

	/** Configures a uniform block binding for the shader.
     * @param setup the information about the uniform block setup
     */
	void setup_uniform_block(const UniformBufferSetup& setup);

	unsigned int shader_program;  ///< The id of the shader program.
	VertexTypes debug_vertex_types;	 ///< The debug information describing the vertex layout that this shader expects.
};

/** Sets up textures for a shader program.
 * @param shader the shader to use
 * @param uniform_list the uniforms to associate with texture units
 * @note The shader must be bound before calling this function.
 */
void setup_textures(ShaderProgram* shader, const std::vector<Uniform*>& uniform_list);

/** Verifies if the current shader matches the required vertex types.
 * @param debug_shader_types Required set of vertex types
 * @return `true` if all required types are present; otherwise, `false`.
 * @note The shader must be bound before calling this function.
 * @note Internal function, should only be available in debug builds
 */
bool is_bound_for_shader(const std::unordered_set<VertexType>& debug_shader_types);

/** Checks if a specific shader program is currently active.
 * @param program the id of the shader program to check
 * @return `true` if the program is active; otherwise, `false`.
 * @note Internal function, should only be available in debug builds
 */
bool is_shader_bound(unsigned int program);

/** Clears the current open gl shader program binding.
 */
void clear_shader_program();

}  //  namespace klotter
