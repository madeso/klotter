#pragma once

#include <memory>

#include "klotter/colors.h"

#include "klotter/render/camera.h"
#include "klotter/render/shader.h"
#include "klotter/render/texture.h"
#include "klotter/render/assets.h"
#include "klotter/render/vertex_layout.h"
#include "klotter/render/mesh.h"

namespace klotter
{

struct Lights;

struct LoadedShaderData
{
	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes mesh_layout;
};

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

struct CompiledMesh
{
	u32 vbo;
	u32 vao;
	u32 ebo;
	std::shared_ptr<Material> material;
	i32 number_of_triangles;

	explicit CompiledMesh(u32, u32, u32, std::shared_ptr<Material>, i32);
	~CompiledMesh();

	CompiledMesh(const CompiledMesh&) = delete;
	CompiledMesh(CompiledMesh&&) = delete;
	void operator=(const CompiledMesh&) = delete;
	void operator=(CompiledMesh&&) = delete;
};

std::shared_ptr<CompiledMesh> compile_Mesh(const Mesh&, std::shared_ptr<Material>);

struct MeshInstance
{
	std::shared_ptr<CompiledMesh> geom;

	glm::vec3 position;
	glm::vec3 rotation;	 ///< yaw pitch roll
};

std::shared_ptr<MeshInstance> make_MeshInstance(std::shared_ptr<CompiledMesh> geom);

struct PointLight
{
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::vec3 color = colors::white;
};

struct Lights
{
	PointLight point_light;
};

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

struct Renderer
{
	Renderer();

	ShaderResource shaders;
	OpenglStates states;
	Assets assets;

	// todo(Gustav): store window size elsewhere...
	glm::ivec2 window_size;

	std::shared_ptr<UnlitMaterial> make_unlit_material();
	std::shared_ptr<DefaultMaterial> make_default_material();

	/// verify that the renderer was fully loaded
	bool is_loaded() const;

	void render(const World&, const Camera&);
};


}  //  namespace klotter
