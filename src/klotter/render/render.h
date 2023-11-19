#pragma once

#include <memory>

#include "klotter/colors.h"

#include "klotter/render/camera.h"
#include "klotter/render/shader.h"
#include "klotter/render/texture.h"
#include "klotter/render/assets.h"
#include "klotter/render/vertex_layout.h"
#include "klotter/render/geom.h"

namespace klotter
{

struct Lights;

struct LoadedShaderData
{
	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
};

/// All loaded/known shaders
struct ShaderResource
{
	ShaderResource();

	LoadedShaderData unlit_shader;
	LoadedShaderData default_shader;

	/// verify that the shaders are loaded
	bool is_loaded() const;
};

struct Material
{
	LoadedShaderData shader;

	explicit Material(LoadedShaderData data);
	virtual ~Material() = default;

	Material(const Material&) = delete;
	Material(Material&&) = delete;
	void operator=(const Material&) = delete;
	void operator=(Material&&) = delete;

	virtual void set_uniforms(const CompiledCamera&, const glm::mat4&) = 0;
	virtual void bind_textures(Assets* assets) = 0;
	virtual void apply_lights(const Lights& lights) = 0;
};

/// a unlit (or fully lit) material, not affected by light
struct UnlitMaterial : Material
{
	glm::vec3 color;
	float alpha;
	std::shared_ptr<Texture> texture;

	explicit UnlitMaterial(const ShaderResource& resource);
	void set_uniforms(const CompiledCamera&, const glm::mat4&) override;
	void bind_textures(Assets* assets) override;
	void apply_lights(const Lights& lights) override;
};

/// a material affected by light
struct DefaultMaterial : Material
{
	glm::vec3 color;
	float alpha;
	std::shared_ptr<Texture> texture;

	explicit DefaultMaterial(const ShaderResource& resource);
	void set_uniforms(const CompiledCamera&, const glm::mat4&) override;
	void bind_textures(Assets* assets) override;
	void apply_lights(const Lights& lights) override;
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

struct PointLight
{
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::vec3 color = colors::white;
};

/// All lights in a world
struct Lights
{
	PointLight point_light;
};

/// also known as a scene
struct World
{
	std::vector<std::shared_ptr<MeshInstance>> meshes;
	Lights lights;
};

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
};

/// the renderering "engine"
struct Renderer
{
	Renderer();

	ShaderResource shaders;
	OpenglStates states;
	Assets assets;

	std::shared_ptr<UnlitMaterial> make_unlit_material();
	std::shared_ptr<DefaultMaterial> make_default_material();

	CompiledGeomVertexAttributes unlit_geom_layout();
	CompiledGeomVertexAttributes default_geom_layout();

	/// verify that the renderer was fully loaded
	bool is_loaded() const;

	void render(const glm::ivec2& window_size, const World&, const Camera&);
};


}  //  namespace klotter
