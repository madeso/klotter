#pragma once

#include <memory>

#include "klotter/colors.h"
#include "klotter/scurve.h"

#include "klotter/render/camera.h"
#include "klotter/render/constants.h"
#include "klotter/render/shader.h"
#include "klotter/render/texture.h"
#include "klotter/render/assets.h"
#include "klotter/render/vertex_layout.h"
#include "klotter/render/geom.h"

namespace klotter
{

struct Lights;

struct LoadedShader_Unlit;
struct LoadedShader_Default;



enum class RenderMode
{
	fill,
	line,
	point
};

enum class CullFace
{
	front,
	back,
	front_and_back
};

enum class Blend
{
	zero,
	one,
	src_color,
	one_minus_src_color,
	dst_color,
	one_minus_dst_color,
	src_alpha,
	one_minus_src_alpha,
	dst_alpha,
	one_minus_dst_alpha,
	constant_color,
	one_minus_constant_color,
	constant_alpha,
	one_minus_constant_alpha,
	src_alpha_saturate,
	src1_color,
	one_minus_src1_color,
	src1_alpha,
	one_minus_src1_alpha
};

using BlendMode = std::tuple<Blend, Blend>;

struct OpenglStates
{
	std::optional<bool> cull_face;
	std::optional<CullFace> cull_face_mode;
	std::optional<bool> blending;
	std::optional<BlendMode> blend_mode;
	std::optional<bool> depth_test;
	std::optional<RenderMode> render_mode;

	std::optional<int> active_texture;
	std::array<std::optional<unsigned int>, MAX_TEXTURES_SUPPORTED> texture_bound;
};

struct RenderSettings
{
	int number_of_point_lights = 5;
	int number_of_directional_lights = 5;
};

/// All loaded/known shaders
struct ShaderResource
{
	explicit ShaderResource(const RenderSettings& settings);
	~ShaderResource();

	struct ShaderResourcePimpl;

	std::unique_ptr<ShaderResourcePimpl> r;

	/// verify that the shaders are loaded
	bool is_loaded() const;
};

/// Base class for all materials
struct Material
{
	Material() = default;
	virtual ~Material() = default;

	Material(const Material&) = delete;
	Material(Material&&) = delete;
	void operator=(const Material&) = delete;
	void operator=(Material&&) = delete;

	virtual void use_shader() = 0;
	virtual void set_uniforms(const CompiledCamera&, const glm::mat4&) = 0;
	virtual void bind_textures(OpenglStates* states, Assets* assets) = 0;
	virtual void apply_lights(const Lights& lights, const RenderSettings& settings) = 0;
};

/// a unlit (or fully lit) material, not affected by light
struct UnlitMaterial : Material
{
	const LoadedShader_Unlit* shader;
	glm::vec3 color;
	float alpha;
	std::shared_ptr<Texture> texture;

	explicit UnlitMaterial(const ShaderResource& resource);
	void use_shader() override;
	void set_uniforms(const CompiledCamera&, const glm::mat4&) override;
	void bind_textures(OpenglStates* states, Assets* assets) override;
	void apply_lights(const Lights& lights, const RenderSettings& settings) override;
};

/// a material affected by light
struct DefaultMaterial : Material
{
	const LoadedShader_Default* shader;
	glm::vec3 color;
	float alpha;

	glm::vec3 ambient_tint;
	glm::vec3 specular_color;
	float shininess;
	float emissive_factor;

	std::shared_ptr<Texture> diffuse;
	std::shared_ptr<Texture> specular;
	std::shared_ptr<Texture> emissive;

	explicit DefaultMaterial(const ShaderResource& resource);
	void use_shader() override;
	void set_uniforms(const CompiledCamera&, const glm::mat4&) override;
	void bind_textures(OpenglStates* states, Assets* assets) override;
	void apply_lights(const Lights& lights, const RenderSettings& settings) override;
};

/// Represents a Geom on the GPU.
struct CompiledGeom
{
	u32 vbo;
	u32 vao;
	u32 ebo;
	i32 number_of_triangles;
	std::unordered_set<VertexType> debug_types;

	explicit CompiledGeom(u32, u32, u32, const CompiledGeomVertexAttributes&, i32);
	~CompiledGeom();

	CompiledGeom(const CompiledGeom&) = delete;
	CompiledGeom(CompiledGeom&&) = delete;
	void operator=(const CompiledGeom&) = delete;
	void operator=(CompiledGeom&&) = delete;
};

std::shared_ptr<CompiledGeom> compile_geom(const Geom&, const CompiledGeomVertexAttributes& layout);

/// Stores Geom + Material (aka a mesh) and its current transform
struct MeshInstance
{
	std::shared_ptr<CompiledGeom> geom;
	std::shared_ptr<Material> material;

	glm::vec3 position;
	glm::vec3 rotation;	 ///< yaw pitch roll
};

std::shared_ptr<MeshInstance> make_mesh_instance(
	std::shared_ptr<CompiledGeom> geom, std::shared_ptr<Material> mat
);

struct DirectionalLight
{
	glm::vec3 direction = {1.0f, 0.0f, 0.0f};

	glm::vec3 color = colors::white;
	float specular = 1.0f;
	float diffuse = 1.0f;
};

struct PointLight
{
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	float min_range = 10.0f;
	float max_range = 20.0f;
	SCurveAndDrag curve;  ///< curve and ui, needs to be trimmed down for actual renderer

	glm::vec3 color = colors::white;
	float specular = 1.0f;
	float diffuse = 1.0f;
};

/// All lights in a world
struct Lights
{
	glm::vec3 color = colors::white;
	float ambient = 0.2f;

	std::vector<PointLight> point_lights;
	std::vector<DirectionalLight> directional_lights;
};

/// also known as a scene
struct World
{
	std::vector<std::shared_ptr<MeshInstance>> meshes;
	Lights lights;
};

/// the renderering "engine"
struct Renderer
{
	RenderSettings settings;
	ShaderResource shaders;
	OpenglStates states;
	Assets assets;

	explicit Renderer(const RenderSettings& settings);

	std::shared_ptr<UnlitMaterial> make_unlit_material();
	std::shared_ptr<DefaultMaterial> make_default_material();

	CompiledGeomVertexAttributes unlit_geom_layout();
	CompiledGeomVertexAttributes default_geom_layout();

	/// verify that the renderer was fully loaded
	bool is_loaded() const;

	void render(const glm::ivec2& window_size, const World&, const Camera&);
};


}  //  namespace klotter
