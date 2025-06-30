#pragma once

#include "klotter/render/uniform.h"
#include "klotter/render/uniform_buffer.h"
#include "klotter/render/vertex_layout.h"

namespace klotter
{
struct FullScreenGeom;
struct RenderSettings;
struct CompiledGeomVertexAttributes;
struct ShaderProgram;
struct CompiledCamera;

/** \addtogroup render Renderer
 *  @{
*/

/// Enum describing on how the model transform is provided.
enum class TransformSource
{
	/// the model source is provided as a mat4 uniform.
	Uniform,

	/// the model source is provided as a (instanced) mat4 attribute
	Instanced_mat4
};

/// "Global state" for the shaders describing the state of the camera.
struct CameraUniformBuffer
{
	UniformBufferSetup setup;

	CompiledUniformProp projection_prop;
	CompiledUniformProp view_prop;

	// this replaces the u_projection and u_view matrices

	std::unique_ptr<UniformBuffer> buffer;

	void set_props(const CompiledCamera& cc);
};


/// A single color shader.
struct LoadedShader_SingleColor
{
	LoadedShader_SingleColor(
		std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
	);

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
	Uniform tint_color;
	Uniform model;
};

/// A skybox shader.
struct LoadedShader_Skybox
{
	LoadedShader_Skybox(
		std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
	);

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
	Uniform tex_skybox;
};

/// Parts of a loaded unlit shader
/// @see \ref LoadedShader_Unlit_Container
struct LoadedShader_Unlit
{
	std::shared_ptr<ShaderProgram> program;

	explicit LoadedShader_Unlit(TransformSource model_source, std::shared_ptr<ShaderProgram> p, const CameraUniformBuffer& desc);

	Uniform tint_color;
	Uniform tex_diffuse;

	std::optional<Uniform> model;
};

/// Uniform for a directional light.
/// @see \ref DirectionalLight
struct DirectionalLightUniforms
{
	DirectionalLightUniforms(const ShaderProgram* program, const std::string& base);

	Uniform light_diffuse_color;
	Uniform light_specular_color;
	Uniform dir;
};

/// Uniforms for a point light.
/// @see \ref PointLight
struct PointLightUniforms
{
	PointLightUniforms(const ShaderProgram* program, const std::string& base);

	Uniform light_diffuse_color;
	Uniform light_specular_color;
	Uniform light_attenuation;
	Uniform light_world;
};

/// Uniforms for a frustum light.
/// @see \ref FrustumLight
struct FrustumLightUniforms
{
	FrustumLightUniforms(const ShaderProgram* program, const std::string& base);

	Uniform diffuse;
	Uniform specular;
	Uniform attenuation;
	Uniform world_to_clip;
	Uniform world_pos;
	Uniform cookie;
};

/// Bitmask for what features each postproc shader wants.
enum class PostProcSetup
{
	none = 0,
	factor = 1 << 1,
	resolution = 1 << 2,
	time = 1 << 3
};
PostProcSetup operator|(PostProcSetup lhs, PostProcSetup rhs);

// todo(Gustav): why is this exposed? remove?
std::optional<Uniform> get_uniform(
	ShaderProgram& prog, const std::string& name, PostProcSetup setup, PostProcSetup flag
);

/// The "base class" for a loaded postproc shader.
struct LoadedPostProcShader
{
	std::shared_ptr<ShaderProgram> program;
	Uniform texture;
	std::optional<Uniform> factor;
	std::optional<Uniform> resolution;
	std::optional<Uniform> time;

	explicit LoadedPostProcShader(std::shared_ptr<ShaderProgram> s, PostProcSetup setup);
};

/// Part of a loaded "default" shader.
/// @see \ref LoadedShader_Default_Container
struct LoadedShader_Default
{
	std::shared_ptr<ShaderProgram> program;

	LoadedShader_Default(
		TransformSource model_source, std::shared_ptr<ShaderProgram> p, const RenderSettings& settings, const CameraUniformBuffer& desc
	);

	Uniform tint_color;
	Uniform tex_diffuse;
	Uniform tex_specular;
	Uniform tex_emissive;
	Uniform ambient_tint;
	Uniform specular_color;
	Uniform shininess;
	Uniform emissive_factor;

	std::optional<Uniform> model;

	Uniform view_position;
	Uniform light_ambient_color;

	std::vector<DirectionalLightUniforms> directional_lights;
	std::vector<PointLightUniforms> point_lights;
	std::vector<FrustumLightUniforms> frustum_lights;
};

/// A "named boolean"
enum class UseTransparency
{
	yes,
	no
};

/// Stores information that is useful when selecting a shader part from a shader container.
struct RenderContext
{
	TransformSource model_source;
	UseTransparency use_transparency;
};

/// A unlit shader.
struct LoadedShader_Unlit_Container
{
	CompiledGeomVertexAttributes geom_layout;

	LoadedShader_Unlit default_shader;
	LoadedShader_Unlit transparency_shader;

	[[nodiscard]] bool is_loaded() const;
};

/// A default shader
struct LoadedShader_Default_Container
{
	CompiledGeomVertexAttributes geom_layout;

	LoadedShader_Default default_shader;
	LoadedShader_Default transparency_shader;
	LoadedShader_Default default_shader_instance;

	[[nodiscard]] bool is_loaded() const;
};

/// Select the correct sub shader from a container.
[[nodiscard]] const LoadedShader_Unlit& shader_from_container(const LoadedShader_Unlit_Container& container, const RenderContext& rc);

/// Select the correct sub shader from a container.
[[nodiscard]] const LoadedShader_Default& shader_from_container(const LoadedShader_Default_Container& container, const RenderContext& rc);

/// All loaded shaders.
struct ShaderResource
{
	LoadedShader_SingleColor single_color_shader;
	LoadedShader_Skybox skybox_shader;

	LoadedShader_Unlit_Container unlit_shader_container;
	LoadedShader_Default_Container default_shader_container;

	// todo(Gustav): split into custom struct
	// todo(Gustav): make post proc more data driven
	std::shared_ptr<LoadedPostProcShader> pp_invert;
	std::shared_ptr<LoadedPostProcShader> pp_grayscale;
	std::shared_ptr<LoadedPostProcShader> pp_damage;
	std::shared_ptr<LoadedPostProcShader> pp_blurv;
	std::shared_ptr<LoadedPostProcShader> pp_blurh;

	/// the realization shader that is always run
	std::shared_ptr<LoadedPostProcShader> pp_always;

	/// verify that the shaders are loaded
	[[nodiscard]] bool is_loaded() const;
};

ShaderResource load_shaders(const CameraUniformBuffer& desc, const RenderSettings& settings, const FullScreenGeom& full_screen);

/**
 * @}
*/

}  //  namespace klotter
