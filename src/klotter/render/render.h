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
#include "klotter/render/linebatch.h"
#include "klotter/render/framebuffer.h"

namespace klotter
{

/** \addtogroup render Renderer
 *  @{
*/

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

enum class Compare
{
	always,
	never,
	less,
	equal,
	less_equal,
	greater,
	not_equal,
	greater_equal
};

enum class StencilAction
{
	/// The currently stored stencil value is kept.
	keep,
	/// The stencil value is set to 0.
	zero,
	/// The stencil value is replaced with the reference value set with stencil_func.
	replace,
	/// The stencil value is increased by 1 if it is lower than the maximum value.
	increase,
	/// Same as increase, but wraps it back to 0 as soon as the maximum value is exceeded.
	increase_wrap,
	/// The stencil value is decreased by 1 if it is higher than the minimum value.
	decrease,
	/// Same as decrease, but wraps it to the maximum value if it ends up lower than 0.
	decrease_wrap,
	/// Bitwise inverts the current stencil buffer value.
	invert
};

using BlendMode = std::tuple<Blend, Blend>;
using StencilFunc = std::tuple<Compare, i32, u32>;
using StencilOp = std::tuple<StencilAction, StencilAction, StencilAction>;

struct OpenglStates
{
	std::optional<bool> cull_face;
	std::optional<CullFace> cull_face_mode;

	std::optional<bool> blending;
	std::optional<BlendMode> blend_mode;

	std::optional<bool> depth_test;
	std::optional<bool> depth_mask;
	std::optional<Compare> depth_func;

	std::optional<bool> stencil_test;
	std::optional<u32> stencil_mask;

	std::optional<RenderMode> render_mode;
	std::optional<StencilFunc> stencil_func;
	std::optional<StencilOp> stencil_op;

	std::optional<int> active_texture;
	std::array<std::optional<unsigned int>, MAX_TEXTURES_SUPPORTED> texture_bound;
};

struct RenderSettings
{
	int number_of_directional_lights = 5;
	int number_of_point_lights = 5;
	int number_of_frustum_lights = 5;
};

/// All loaded/known shaders
struct ShaderResource;
struct RenderContext;

/// Base class for all materials
struct Material
{
	Material() = default;
	virtual ~Material() = default;

	Material(const Material&) = delete;
	Material(Material&&) = delete;
	void operator=(const Material&) = delete;
	void operator=(Material&&) = delete;

	virtual void use_shader(const RenderContext&) = 0;
	virtual void set_uniforms(const RenderContext&, const CompiledCamera&, const glm::mat4&) = 0;
	virtual void bind_textures(const RenderContext&, OpenglStates* states, Assets* assets) = 0;
	virtual void apply_lights(
		const RenderContext&,
		const Lights& lights,
		const RenderSettings& settings,
		OpenglStates* states,
		Assets* assets
	) = 0;

	virtual bool is_transparent() const = 0;
};

/// a unlit (or fully lit) material, not affected by light
struct UnlitMaterial : Material
{
	const LoadedShader_Unlit* shader;
	glm::vec3 color;
	float alpha;
	std::shared_ptr<Texture> texture;

	explicit UnlitMaterial(const ShaderResource& resource);
	void use_shader(const RenderContext&) override;
	void set_uniforms(const RenderContext&, const CompiledCamera&, const glm::mat4&) override;
	void bind_textures(const RenderContext&, OpenglStates* states, Assets* assets) override;
	void apply_lights(
		const RenderContext&,
		const Lights& lights,
		const RenderSettings& settings,
		OpenglStates* states,
		Assets* assets
	) override;

	bool is_transparent() const override;
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
	void use_shader(const RenderContext&) override;
	void set_uniforms(const RenderContext&, const CompiledCamera&, const glm::mat4&) override;
	void bind_textures(const RenderContext&, OpenglStates* states, Assets* assets) override;
	void apply_lights(
		const RenderContext&,
		const Lights& lights,
		const RenderSettings& settings,
		OpenglStates* states,
		Assets* assets
	) override;

	bool is_transparent() const override;
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

struct LocalAxis
{
	glm::vec3 x;
	glm::vec3 y;
	glm::vec3 z;
};

enum Billboarding
{
	none,
	screen,
	screen_fast,
	axial_y,
	axial_y_fast
};

/// Stores Geom + Material (aka a mesh) and its current transform
struct MeshInstance
{
	std::shared_ptr<CompiledGeom> geom;
	std::shared_ptr<Material> material;

	std::optional<glm::vec3> outline;

	glm::vec3 position;
	glm::vec3 rotation;	 ///< yaw pitch roll
	Billboarding billboarding = Billboarding::none;	 ///< if not none, rotation is ignored

	LocalAxis get_local_axis() const;
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

struct FrustumLight
{
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	float yaw = 0.0f;
	float pitch = 0.0f;
	float fov = 10.0f;	// in angles
	float aspect = 1.0;

	float min_range = 10.0f;
	float max_range = 20.0f;
	SCurveAndDrag curve;  ///< curve and ui, needs to be trimmed down for actual renderer

	glm::vec3 color = colors::white;
	float specular = 1.0f;
	float diffuse = 1.0f;

	std::shared_ptr<Texture> cookie;  // if null, pure white is used
};

/// All lights in a world
struct Lights
{
	glm::vec3 color = colors::white;
	float ambient = 0.2f;

	std::vector<DirectionalLight> directional_lights;
	std::vector<PointLight> point_lights;
	std::vector<FrustumLight> frustum_lights;
};

/// also known as a scene
struct World
{
	std::vector<std::shared_ptr<MeshInstance>> meshes;
	Lights lights;
};

enum LineStyle
{
	depth,
	always_visible,
	dashed_when_hidden
};

struct DebugLine
{
	glm::vec3 from;
	glm::vec3 to;
	glm::vec3 color;
	LineStyle style;
};

struct DebugRender
{
	std::vector<DebugLine> debug_lines;

	void add_line(
		const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, LineStyle style
	);
};

struct Renderer;

struct PostProcArg
{
	const World* world;
	glm::ivec2 window_size;
	const Camera* camera;
	Renderer* renderer;
};

struct RenderSource
{
	virtual ~RenderSource() = default;
	virtual void render(const PostProcArg& arg) = 0;
};

struct ShaderPropertyProvider
{
	virtual ~ShaderPropertyProvider() = default;
	virtual void use_shader(const PostProcArg& a, const Texture& t) = 0;
};

struct RenderTask : RenderSource
{
	std::shared_ptr<RenderSource> source;
	std::shared_ptr<FrameBuffer> fbo;
	ShaderPropertyProvider* effect;

	RenderTask(
		std::shared_ptr<RenderSource> s, std::shared_ptr<FrameBuffer> f, ShaderPropertyProvider* e
	);

	/// render internal fbo to a quad with a shader
	void render(const PostProcArg& arg) override;

	/// call render on linked source and render to fbo
	void update(const PostProcArg& arg);
};

/// when compiled it could be:
/// * [render world to screen]
/// * [render world to fbo], [render fbo to screen with shader]
/// * [render world to fbo] [[render fbo to intermediate with shader] [render int to screen with shader]]
/// * [render world to fbo] [[render fbo to intermediate with shader] [render int to fbo with shader]] [render fbo to screen with shader]
struct CompiledStack
{
	/// start with a simple world, but depending on the current effect list, could be more...
	std::shared_ptr<RenderSource> last_source;

	std::vector<std::shared_ptr<RenderTask>> targets;
};

struct FrameBufferCache
{
	std::shared_ptr<FrameBuffer> get(
		glm::ivec2 size,
		TextureEdge edge,
		TextureRenderStyle render_style,
		Transparency transperency
	);
};

struct BuildArg
{
	CompiledStack* builder;
	FrameBufferCache* fbo;
	glm::ivec2 window_size;
};


struct EffectStack;

struct Effect
{
	virtual ~Effect() = default;

	virtual void build(const BuildArg& arg) = 0;
	virtual void update(float dt) = 0;
	virtual void gui() = 0;

	bool enabled() const;

   protected:

	void set_enabled(bool n);

   private:

	friend EffectStack;

	bool is_enabled = false;
	EffectStack* owner = nullptr;
};

struct FactorEffect : Effect
{
	FactorEffect();
	float get_factor() const;
	void set_factor(float f);

   private:

	float factor;
};

/// The facade of the post-proc framework.
struct EffectStack
{
	bool dirty = true;
	std::optional<glm::ivec2> window_size;
	std::vector<std::shared_ptr<Effect>> effects;
	CompiledStack compiled;
	FrameBufferCache fbos;

	/// rebuilds stack if dirty, update all targets, then render the last_source
	void render(const PostProcArg& arg);
	void update(float dt);
	void gui();
};


struct RendererPimpl;

/// the renderering "engine"
struct Renderer
{
	RenderSettings settings;
	Assets assets;
	DebugRender debug;

	std::unique_ptr<RendererPimpl> pimpl;

	explicit Renderer(const RenderSettings& settings);
	~Renderer();

	std::shared_ptr<UnlitMaterial> make_unlit_material();
	std::shared_ptr<DefaultMaterial> make_default_material();

	CompiledGeomVertexAttributes unlit_geom_layout();
	CompiledGeomVertexAttributes default_geom_layout();

	std::shared_ptr<FactorEffect> make_invert_effect();
	std::shared_ptr<FactorEffect> make_grayscale_effect();
	std::shared_ptr<FactorEffect> make_damage_effect();

	/// verify that the renderer was fully loaded
	bool is_loaded() const;

	/// doesn't set the size, prefer EffectStack::render
	void render_world(const glm::ivec2& window_size, const World&, const Camera&);
};

/**
 * @}
*/

}  //  namespace klotter
