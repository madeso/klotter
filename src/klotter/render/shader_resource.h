#pragma once

#include "klotter/render/uniform.h"
#include "klotter/render/uniform_buffer.h"
#include "klotter/render/vertex_layout.h"

namespace klotter
{
struct FullScreenInfo;
struct RenderSettings;
struct CompiledGeomVertexAttributes;
struct ShaderProgram;
struct CompiledCamera;

/** \addtogroup render Renderer
 *  @{
*/

enum class ModelSource
{
	/// the model source is provided as a mat4 uniform.
	Uniform,

	/// the model source is provided as a (instanced) mat4 attribute
	Instanced_mat4
};

struct CameraUniformBuffer
{
	UniformBufferSetup setup;

	CompiledUniformProp projection_prop;
	CompiledUniformProp view_prop;

	// this replaces the u_projection and u_view matrices

	std::unique_ptr<UniformBuffer> buffer;

	void set_props(const CompiledCamera& cc);
};

struct LoadedShader
{
	LoadedShader(std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l);

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
};

struct LoadedShader_SingleColor : LoadedShader
{
	LoadedShader_SingleColor(LoadedShader s, const CameraUniformBuffer& desc);

	Uniform tint_color;
	Uniform model;
};

struct LoadedShader_Skybox : LoadedShader
{
	LoadedShader_Skybox(LoadedShader s, const CameraUniformBuffer& desc);

	Uniform tex_skybox;
};

struct Base_LoadedShader_Unlit
{
	std::shared_ptr<ShaderProgram> program;

	explicit Base_LoadedShader_Unlit(ModelSource model_source, LoadedShader s, const CameraUniformBuffer& desc);

	Uniform tint_color;
	Uniform tex_diffuse;

	std::optional<Uniform> model;
};

struct DirectionalLightUniforms
{
	DirectionalLightUniforms(ShaderProgram* program, const std::string& base);

	Uniform light_diffuse_color;
	Uniform light_specular_color;
	Uniform dir;
};

struct PointLightUniforms
{
	PointLightUniforms(ShaderProgram* program, const std::string& base);

	Uniform light_diffuse_color;
	Uniform light_specular_color;
	Uniform light_attenuation;
	Uniform light_world;
};

struct FrustumLightUniforms
{
	FrustumLightUniforms(ShaderProgram* program, const std::string& base);

	Uniform diffuse;
	Uniform specular;
	Uniform attenuation;
	Uniform world_to_clip;
	Uniform world_pos;
	Uniform cookie;
};

enum class PostProcSetup
{
	none = 0,
	factor = 1 << 1,
	resolution = 1 << 2,
	time = 1 << 3
};

PostProcSetup operator|(PostProcSetup lhs, PostProcSetup rhs);

std::optional<Uniform> get_uniform(
	ShaderProgram& prog, const std::string& name, PostProcSetup setup, PostProcSetup flag
);

struct LoadedPostProcShader
{
	std::shared_ptr<ShaderProgram> program;
	Uniform texture;
	std::optional<Uniform> factor;
	std::optional<Uniform> resolution;
	std::optional<Uniform> time;

	explicit LoadedPostProcShader(std::shared_ptr<ShaderProgram> s, PostProcSetup setup);
};

struct Base_LoadedShader_Default
{
	std::shared_ptr<ShaderProgram> program;

	Base_LoadedShader_Default(
		ModelSource model_source, LoadedShader s, const RenderSettings& settings, const CameraUniformBuffer& desc
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

enum class UseTransparency
{
	yes,
	no
};

struct RenderContext
{
	ModelSource model_source;
	UseTransparency use_transparency;
};

struct LoadedShader_Unlit
{
	CompiledGeomVertexAttributes geom_layout;
	Base_LoadedShader_Unlit default_shader;
	Base_LoadedShader_Unlit transparency_shader;

	// todo(Gustav): rename to something better than "base"
	[[nodiscard]] const Base_LoadedShader_Unlit& base(const RenderContext& rc) const;
	[[nodiscard]] bool is_loaded() const;
};

struct LoadedShader_Default
{
	CompiledGeomVertexAttributes geom_layout;
	Base_LoadedShader_Default default_shader;
	Base_LoadedShader_Default transparency_shader;
	Base_LoadedShader_Default default_shader_instance;

	[[nodiscard]] const Base_LoadedShader_Default& base(const RenderContext& rc) const;
	[[nodiscard]] bool is_loaded() const;
};

struct ShaderResource
{
	LoadedShader_SingleColor single_color_shader;
	LoadedShader_Skybox skybox_shader;

	LoadedShader_Unlit unlit_shader;
	LoadedShader_Default default_shader;

	// todo(Gustav): split into 
	std::shared_ptr<LoadedPostProcShader> pp_invert;
	std::shared_ptr<LoadedPostProcShader> pp_grayscale;
	std::shared_ptr<LoadedPostProcShader> pp_damage;
	std::shared_ptr<LoadedPostProcShader> pp_blurv;
	std::shared_ptr<LoadedPostProcShader> pp_blurh;

	/// verify that the shaders are loaded
	[[nodiscard]] bool is_loaded() const;
};

ShaderResource load_shaders(const CameraUniformBuffer& desc, const RenderSettings& settings, const FullScreenInfo& fsi);

/**
 * @}
*/

}  //  namespace klotter
