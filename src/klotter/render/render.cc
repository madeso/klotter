#include "klotter/render/render.h"

#if INCLUDE_POST_PROC == 1
#include "pp.vert.glsl.h"
#include "pp.invert.frag.glsl.h"
#include "pp.grayscale.frag.glsl.h"
#include "pp.damage.frag.glsl.h"
#include "pp.blur.frag.glsl.h"
#endif

#include "klotter/cint.h"
#include "klotter/assert.h"
#include "klotter/str.h"
#include "klotter/log.h"

#include "klotter/render/opengl_utils.h"
#include "klotter/render/shader.source.h"
#include "klotter/render/geom.extract.h"
#include "klotter/render/geom.builder.h"
#include "klotter/render/uniform_buffer.h"

#include "imgui.h"

#include <string_view>

namespace
{
constexpr float ALMOST_ZERO = 0.01f;

/// if alpha goes above this limit, it is no longer considered transparent
constexpr float ALPHA_TRANSPARENCY_LIMIT = 1.0f - ALMOST_ZERO;

#if INCLUDE_POST_PROC == 1
constexpr int BLUR_SAMPLES = 10;
#endif
}  //  namespace

#define BLUR_USE_GAUSS 1

namespace klotter
{

// ------------------------------------------------------------------------------------------------
// std utils

// convert a enum class to it's underlying (int) type
// src: https://twitter.com/idoccor/status/1314664849276899328
template<typename E>
constexpr typename std::underlying_type_t<E> base_cast(E e) noexcept
{
	return static_cast<typename std::underlying_type_t<E>>(e);
}

template<typename E>
constexpr bool is_flag_set(E var, E flag)
{
	return (base_cast(var) & base_cast(flag)) > 0;
}

// ------------------------------------------------------------------------------------------------
// internal header

void set_gl_viewport(const glm::ivec2& sz);
void render_geom(std::shared_ptr<CompiledGeom> geom);

// ------------------------------------------------------------------------------------------------
// state management

template<typename T>
bool should_change(std::optional<T>* current_state, T new_state)
{
	// if there is a value, and that is the same... then don't update opengl
	if (*current_state && *current_state == new_state)
	{
		return false;
	}

	*current_state = new_state;
	return true;
}

void apply(std::optional<bool>* current_state, bool new_state, GLenum gl_type)
{
	if (should_change(current_state, new_state))
	{
		if (new_state)
		{
			glEnable(gl_type);
		}
		else
		{
			glDisable(gl_type);
		}
	}
}

GLenum C(Compare new_state)
{
	switch (new_state)
	{
	case Compare::always: return GL_ALWAYS;
	case Compare::never: return GL_NEVER;
	case Compare::less: return GL_LESS;
	case Compare::equal: return GL_EQUAL;
	case Compare::less_equal: return GL_LEQUAL;
	case Compare::greater: return GL_GREATER;
	case Compare::not_equal: return GL_NOTEQUAL;
	case Compare::greater_equal: return GL_GEQUAL;
	default: DIE("Invalid depth func"); return GL_LESS;
	}
}

struct StateChanger
{
	OpenglStates* states;

	explicit StateChanger(OpenglStates* s)
		: states(s)
	{
	}

	StateChanger& cull_face(bool new_state)
	{
		apply(&states->cull_face, new_state, GL_CULL_FACE);
		return *this;
	}

	StateChanger& blending(bool new_state)
	{
		apply(&states->blending, new_state, GL_BLEND);
		return *this;
	}

	StateChanger& depth_test(bool new_state)
	{
		apply(&states->depth_test, new_state, GL_DEPTH_TEST);
		return *this;
	}

	StateChanger& depth_mask(bool new_state)
	{
		if (should_change(&states->depth_mask, new_state))
		{
			glDepthMask(new_state ? GL_TRUE : GL_FALSE);
		}
		return *this;
	}

	StateChanger& depth_func(Compare new_state)
	{
		if (should_change(&states->depth_func, new_state))
		{
			const auto mode = C(new_state);
			glDepthFunc(mode);
		}

		return *this;
	}

	StateChanger& stencil_test(bool new_state)
	{
		apply(&states->stencil_test, new_state, GL_STENCIL_TEST);
		return *this;
	}

	/// Set a bitmask that is ANDed with the stencil value about to be written to the buffer.
	StateChanger& stencil_mask(u32 new_state)
	{
		if (should_change(&states->stencil_mask, new_state))
		{
			glStencilMask(new_state);
		}

		return *this;
	}

	/** 
	* @param func the stencil test function that determines whether a fragment passes or is discarded.
	This test function is applied to the stored stencil value and the ref value.
	* @param ref the reference value for the stencil test. The stencil buffer's content is compared to this value.
	* @param mask the mask that is ANDed with both the reference value and the stored stencil value before the test compares them.
	*/
	StateChanger& stencil_func(Compare func, i32 ref, u32 mask)
	{
		if (should_change(&states->stencil_func, {func, ref, mask}))
		{
			glStencilFunc(C(func), ref, mask);
		}

		return *this;
	}

	StateChanger& render_mode(RenderMode new_state)
	{
		if (should_change(&states->render_mode, new_state))
		{
			const auto mode = ([new_state]() -> GLenum
                {
                    switch (new_state)
                    {
                    case RenderMode::fill: return GL_FILL;
                    case RenderMode::line: return GL_LINE;
                    case RenderMode::point: return GL_POINT;
                    default: DIE("Invalid render mode"); return GL_FILL;
                    }
                })();
			glPolygonMode(GL_FRONT_AND_BACK, mode);
		}
		return *this;
	}

	/**
    * @param stencil_fail action to take if the stencil test fails.
    * @param depth_fail action to take if the stencil test passes, but the depth test fails.
    * @param pass action to take if both the stencil and the depth test pass.
	*/
	StateChanger& stencil_op(
		StencilAction stencil_fail, StencilAction depth_fail, StencilAction pass
	)
	{
		const auto Csa = [](StencilAction sa) -> GLenum
		{
			switch (sa)
			{
			case StencilAction::keep: return GL_KEEP;
			case StencilAction::zero: return GL_ZERO;
			case StencilAction::replace: return GL_REPLACE;
			case StencilAction::increase: return GL_INCR;
			case StencilAction::increase_wrap: return GL_INCR_WRAP;
			case StencilAction::decrease: return GL_DECR;
			case StencilAction::decrease_wrap: return GL_DECR_WRAP;
			case StencilAction::invert: return GL_INVERT;
			default: DIE("Invalid stencil action"); return GL_KEEP;
			}
		};

		if (should_change(&states->stencil_op, {stencil_fail, depth_fail, pass}))
		{
			// todo(Gustav): look into using glStencilOpSeparate instead to specify front and back faces
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glStencilOpSeparate.xhtml
			glStencilOp(Csa(stencil_fail), Csa(depth_fail), Csa(pass));
		}

		return *this;
	}

	StateChanger& cull_face_mode(CullFace new_state)
	{
		if (should_change(&states->cull_face_mode, new_state))
		{
			const auto mode = ([new_state]() -> GLenum
                {
                    switch (new_state)
                    {
                    case CullFace::front: return GL_FRONT;
                    case CullFace::back: return GL_BACK;
                    case CullFace::front_and_back: return GL_FRONT_AND_BACK;
                    default: DIE("Invalid cull face mode"); return GL_BACK;
                    }
                })();
			glCullFace(mode);
		}
		return *this;
	}

	StateChanger& blend_mode(Blend src, Blend dst)
	{
		if (should_change(&states->blend_mode, {src, dst}))
		{
			const auto convert = [](Blend b) -> GLenum
			{
				switch (b)
				{
				case Blend::zero: return GL_ZERO;
				case Blend::one: return GL_ONE;
				case Blend::src_color: return GL_SRC_COLOR;
				case Blend::one_minus_src_color: return GL_ONE_MINUS_SRC_COLOR;
				case Blend::dst_color: return GL_DST_COLOR;
				case Blend::one_minus_dst_color: return GL_ONE_MINUS_DST_COLOR;
				case Blend::src_alpha: return GL_SRC_ALPHA;
				case Blend::one_minus_src_alpha: return GL_ONE_MINUS_SRC_ALPHA;
				case Blend::dst_alpha: return GL_DST_ALPHA;
				case Blend::one_minus_dst_alpha: return GL_ONE_MINUS_DST_ALPHA;
				case Blend::constant_color: return GL_CONSTANT_COLOR;
				case Blend::one_minus_constant_color: return GL_ONE_MINUS_CONSTANT_COLOR;
				case Blend::constant_alpha: return GL_CONSTANT_ALPHA;
				case Blend::one_minus_constant_alpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
				case Blend::src_alpha_saturate: return GL_SRC_ALPHA_SATURATE;
				case Blend::src1_color: return GL_SRC1_COLOR;
				case Blend::one_minus_src1_color: return GL_ONE_MINUS_SRC1_COLOR;
				case Blend::src1_alpha: return GL_SRC1_ALPHA;
				case Blend::one_minus_src1_alpha: return GL_ONE_MINUS_SRC1_ALPHA;
				default: DIE("Invalid blend mode"); return GL_ZERO;
				}
			};
			glBlendFunc(convert(src), convert(dst));
		}
		return *this;
	}

	StateChanger& activate_texture(int new_texture)
	{
		if (should_change(&states->active_texture, new_texture))
		{
			glActiveTexture(Cint_to_glenum(GL_TEXTURE0 + new_texture));
		}
		return *this;
	}

	StateChanger& bind_texture_2d(int slot, unsigned int texture)
	{
		ASSERT(slot == states->active_texture);
		if (should_change(&states->texture_bound[Cint_to_sizet(slot)], texture))
		{
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		return *this;
	}

	StateChanger& bind_texture_cubemap(int slot, unsigned int texture)
	{
		ASSERT(slot == states->active_texture);
		if (should_change(&states->texture_bound[Cint_to_sizet(slot)], texture))
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		}
		return *this;
	}
};

// ------------------------------------------------------------------------------------------------
// shader resources

struct CameraUniformBuffer
{
	UniformBufferSetup setup;

	CompiledUniformProp projection_prop;
	CompiledUniformProp view_prop;

	// this replaces the u_projection and u_view matrices

	std::unique_ptr<UniformBuffer> buffer;

	void set_props(const CompiledCamera& cc)
	{
		buffer->set_mat4(projection_prop, cc.projection);
		buffer->set_mat4(view_prop, cc.view);
	}
};

struct LoadedShader
{
	LoadedShader(std::shared_ptr<ShaderProgram> p, const CompiledGeomVertexAttributes& l)
		: program(std::move(p))
		, geom_layout(l)
	{
	}

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
};

struct LoadedShader_SingleColor : LoadedShader
{
	LoadedShader_SingleColor(LoadedShader s, const CameraUniformBuffer& desc)
		: LoadedShader(std::move(s.program), s.geom_layout)
		, tint_color(program->get_uniform("u_material.diffuse_tint"))
		, model(program->get_uniform("u_model"))
	{
		program->setup_uniform_block(desc.setup);
	}

	Uniform tint_color;

	Uniform model;
};

struct LoadedShader_Skybox : LoadedShader
{
	LoadedShader_Skybox(LoadedShader s, const CameraUniformBuffer& desc)
		: LoadedShader(std::move(s.program), s.geom_layout)
		, tex_skybox(program->get_uniform("u_skybox_tex"))
	{
		setup_textures(program.get(), {&tex_skybox});
		program->setup_uniform_block(desc.setup);
	}

	Uniform tex_skybox;
};

struct Base_LoadedShader_Unlit
{
	std::shared_ptr<ShaderProgram> program;

	explicit Base_LoadedShader_Unlit(LoadedShader s, const CameraUniformBuffer& desc)
		: program(std::move(s.program))
		, tint_color(program->get_uniform("u_material.diffuse_tint"))
		, tex_diffuse(program->get_uniform("u_material.diffuse_tex"))
		, model(program->get_uniform("u_model"))
	{
		setup_textures(program.get(), {&tex_diffuse});
		program->setup_uniform_block(desc.setup);
	}

	Uniform tint_color;
	Uniform tex_diffuse;

	Uniform model;
};

struct DirectionalLightUniforms
{
	DirectionalLightUniforms(ShaderProgram* program, const std::string& base)
		: light_diffuse_color(program->get_uniform(base + "diffuse"))
		, light_specular_color(program->get_uniform(base + "specular"))
		, dir(program->get_uniform(base + "dir"))
	{
	}

	Uniform light_diffuse_color;
	Uniform light_specular_color;
	Uniform dir;
};

struct PointLightUniforms
{
	PointLightUniforms(ShaderProgram* program, const std::string& base)
		: light_diffuse_color(program->get_uniform(base + "diffuse"))
		, light_specular_color(program->get_uniform(base + "specular"))
		, light_attenuation(program->get_uniform(base + "attenuation"))
		, light_world(program->get_uniform(base + "world_pos"))
	{
	}

	Uniform light_diffuse_color;
	Uniform light_specular_color;
	Uniform light_attenuation;
	Uniform light_world;
};

struct FrustumLightUniforms
{
	FrustumLightUniforms(ShaderProgram* program, const std::string& base)
		: diffuse(program->get_uniform(base + "diffuse"))
		, specular(program->get_uniform(base + "specular"))
		, attenuation(program->get_uniform(base + "attenuation"))
		, world_to_clip(program->get_uniform(base + "world_to_clip"))
		, world_pos(program->get_uniform(base + "world_pos"))
		, cookie(program->get_uniform(base + "cookie"))
	{
	}

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

PostProcSetup operator|(PostProcSetup lhs, PostProcSetup rhs)
{
	return static_cast<PostProcSetup>(base_cast(lhs) | base_cast(rhs));
}

std::optional<Uniform> get_uniform(
	ShaderProgram& prog, const std::string& name, PostProcSetup setup, PostProcSetup flag
)
{
	if (is_flag_set(setup, flag))
	{
		return prog.get_uniform(name);
	}
	else
	{
		return std::nullopt;
	}
}

struct LoadedPostProcShader
{
	std::shared_ptr<ShaderProgram> program;
	Uniform texture;
	std::optional<Uniform> factor;
	std::optional<Uniform> resolution;
	std::optional<Uniform> time;

	explicit LoadedPostProcShader(std::shared_ptr<ShaderProgram> s, PostProcSetup setup)
		: program(std::move(s))
		, texture(program->get_uniform("u_texture"))
		, factor(get_uniform(*program, "u_factor", setup, PostProcSetup::factor))
		, resolution(get_uniform(*program, "u_resolution", setup, PostProcSetup::resolution))
		, time(get_uniform(*program, "u_time", setup, PostProcSetup::time))
	{
		setup_textures(program.get(), {&texture});
	}
};

struct Base_LoadedShader_Default
{
	std::shared_ptr<ShaderProgram> program;

	Base_LoadedShader_Default(
		LoadedShader s, const RenderSettings& settings, const CameraUniformBuffer& desc
	)
		: program(std::move(s.program))
		, tint_color(program->get_uniform("u_material.diffuse_tint"))
		, tex_diffuse(program->get_uniform("u_material.diffuse_tex"))
		, tex_specular(program->get_uniform("u_material.specular_tex"))
		, tex_emissive(program->get_uniform("u_material.emissive_tex"))
		, ambient_tint(program->get_uniform("u_material.ambient_tint"))
		, specular_color(program->get_uniform("u_material.specular_tint"))
		, shininess(program->get_uniform("u_material.shininess"))
		, emissive_factor(program->get_uniform("u_material.emissive_factor"))
		, model(program->get_uniform("u_model"))
		, view_position(program->get_uniform("u_view_position"))
		, light_ambient_color(program->get_uniform("u_ambient_light"))
	{
		for (int i = 0; i < settings.number_of_directional_lights; i += 1)
		{
			const std::string base = Str{} << "u_directional_lights[" << i << "].";
			directional_lights.emplace_back(program.get(), base);
		}

		for (int i = 0; i < settings.number_of_point_lights; i += 1)
		{
			const std::string base = Str{} << "u_point_lights[" << i << "].";
			point_lights.emplace_back(program.get(), base);
		}

		for (int i = 0; i < settings.number_of_frustum_lights; i += 1)
		{
			const std::string base = Str{} << "u_frustum_lights[" << i << "].";
			frustum_lights.emplace_back(program.get(), base);
		}

		std::vector<Uniform*> textures = {&tex_diffuse, &tex_specular, &tex_emissive};
		for (auto& fl: frustum_lights)
		{
			textures.emplace_back(&fl.cookie);
		}

		setup_textures(program.get(), textures);
		program->setup_uniform_block(desc.setup);
	}

	Uniform tint_color;
	Uniform tex_diffuse;
	Uniform tex_specular;
	Uniform tex_emissive;
	Uniform ambient_tint;
	Uniform specular_color;
	Uniform shininess;
	Uniform emissive_factor;

	Uniform model;

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
	UseTransparency use_transparency;
};

struct LoadedShader_Unlit
{
	CompiledGeomVertexAttributes geom_layout;
	Base_LoadedShader_Unlit default_shader;
	Base_LoadedShader_Unlit transparency_shader;

	const Base_LoadedShader_Unlit& base(const RenderContext& rc) const
	{
		return rc.use_transparency == UseTransparency::yes ? transparency_shader : default_shader;
	}

	bool is_loaded() const
	{
		return default_shader.program->is_loaded() && transparency_shader.program->is_loaded();
	}
};

struct LoadedShader_Default
{
	CompiledGeomVertexAttributes geom_layout;
	Base_LoadedShader_Default default_shader;
	Base_LoadedShader_Default transparency_shader;

	const Base_LoadedShader_Default& base(const RenderContext& rc) const
	{
		return rc.use_transparency == UseTransparency::yes ? transparency_shader : default_shader;
	}

	bool is_loaded() const
	{
		return default_shader.program->is_loaded() && transparency_shader.program->is_loaded();
	}
};

struct ShaderResource
{
	LoadedShader_SingleColor single_color_shader;
	LoadedShader_Skybox skybox_shader;

	LoadedShader_Unlit unlit_shader;
	LoadedShader_Default default_shader;

#if INCLUDE_POST_PROC == 1
	std::shared_ptr<LoadedPostProcShader> pp_invert;
	std::shared_ptr<LoadedPostProcShader> pp_grayscale;
	std::shared_ptr<LoadedPostProcShader> pp_damage;
	std::shared_ptr<LoadedPostProcShader> pp_blurv;
	std::shared_ptr<LoadedPostProcShader> pp_blurh;
#endif

	/// verify that the shaders are loaded
	bool is_loaded() const
	{
		return single_color_shader.program->is_loaded() && skybox_shader.program->is_loaded()
			&& unlit_shader.is_loaded() && default_shader.is_loaded()
#if INCLUDE_POST_PROC == 1
			&& pp_invert->program->is_loaded() && pp_grayscale->program->is_loaded()
			&& pp_blurv->program->is_loaded() && pp_blurh->program->is_loaded()
#endif
			;
	}
};

struct FullScreenInfo
{
	std::shared_ptr<CompiledGeom> full_screen_geom;
	CompiledShaderVertexAttributes full_scrren_layout;

	FullScreenInfo()
	{
		const auto layout_shader_material = ShaderVertexAttributes{
			{VertexType::position2xy, "a_position"}, {VertexType::texture2, "a_tex_coord"}
		};

		auto layout_compiler = compile_attribute_layouts({layout_shader_material});
		full_scrren_layout = compile_shader_layout(layout_compiler, layout_shader_material);
		const auto layout = get_geom_layout(layout_compiler);

		// ndc is [-1, 1], plane func generate a centered geom, so we set out plane to a size of 2
		full_screen_geom = compile_geom(
			geom::create_xy_plane(2.0f, 2.0f, geom::TwoSided::one_sided).to_geom(), layout
		);
	}
};

ShaderResource load_shaders(
	const CameraUniformBuffer& desc, const RenderSettings& settings, const FullScreenInfo& fsi
);

CameraUniformBuffer make_camera_uniform_buffer_desc()
{
	CameraUniformBuffer camera_uniform_buffer;

	{
		UniformBufferCompiler compiler;
		compiler.add(&camera_uniform_buffer.projection_prop, UniformType::mat4, "u_projection");
		compiler.add(&camera_uniform_buffer.view_prop, UniformType::mat4, "u_view");
		compiler.compile("Camera", &camera_uniform_buffer.setup, 0);
	}

	camera_uniform_buffer.buffer = std::make_unique<UniformBuffer>(camera_uniform_buffer.setup);

	return camera_uniform_buffer;
}

std::string string_from_gl_bytes(const GLubyte* bytes)
{
	return reinterpret_cast<const char*>(bytes);
}

struct RendererPimpl
{
	CameraUniformBuffer camera_uniform_buffer;
	ShaderResource shaders;
	OpenglStates states;
	DebugDrawer debug;
	std::shared_ptr<CompiledGeom> full_screen_geom;

	RendererPimpl(const RenderSettings& set, const FullScreenInfo& fsi)
		: camera_uniform_buffer(make_camera_uniform_buffer_desc())
		, shaders(load_shaders(camera_uniform_buffer, set, fsi))
		, full_screen_geom(fsi.full_screen_geom)
	{
		const auto vendor = string_from_gl_bytes(glGetString(GL_VENDOR));
		const auto renderer = string_from_gl_bytes(glGetString(GL_RENDERER));
		const auto version = string_from_gl_bytes(glGetString(GL_VERSION));
		const auto shading_language_version
			= string_from_gl_bytes(glGetString(GL_SHADING_LANGUAGE_VERSION));
		const auto extensions = string_from_gl_bytes(glGetStringi(GL_EXTENSIONS, 0));

		LOG_INFO("vendor %s, renderer %s", vendor.c_str(), renderer.c_str());
		LOG_INFO("version %s (glsl %s)", version.c_str(), shading_language_version.c_str());
		LOG_INFO("extensions %s", extensions.c_str());
	}
};

// ------------------------------------------------------------------------------------------------
// glue code

CompiledGeom::CompiledGeom(u32 b, u32 a, u32 e, const CompiledGeomVertexAttributes& att, i32 tc)
	: vbo(b)
	, vao(a)
	, ebo(e)
	, number_of_triangles(tc)
	, debug_types(att.debug_types.begin(), att.debug_types.end())

{
}

std::shared_ptr<MeshInstance> make_mesh_instance(
	std::shared_ptr<CompiledGeom> geom, std::shared_ptr<Material> mat
)
{
	auto instance = std::make_shared<MeshInstance>();
	instance->geom = geom;
	instance->material = mat;
	return instance;
}

std::shared_ptr<UnlitMaterial> Renderer::make_unlit_material()
{
	return std::make_shared<UnlitMaterial>(pimpl->shaders);
}

std::shared_ptr<DefaultMaterial> Renderer::make_default_material()
{
	return std::make_shared<DefaultMaterial>(pimpl->shaders);
}

CompiledGeomVertexAttributes Renderer::unlit_geom_layout() const
{
	return pimpl->shaders.unlit_shader.geom_layout;
}

CompiledGeomVertexAttributes Renderer::default_geom_layout() const
{
	return pimpl->shaders.default_shader.geom_layout;
}

bool Renderer::is_loaded() const
{
	return pimpl->shaders.is_loaded() && pimpl->debug.is_loaded();
}

void DebugRender::add_line(
	const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, LineStyle style
)
{
	debug_lines.emplace_back(DebugLine{from, to, color, style});
}

// ------------------------------------------------------------------------------------------------
// shader loading

using BaseShaderData = std::vector<VertexType>;

BaseShaderData get_vertex_types(const ShaderVertexAttributes& va)
{
	auto ret = BaseShaderData{};
	for (const auto& v: va)
	{
		ret.emplace_back(v.type);
	}
	return ret;
}

LoadedShader load_shader(const BaseShaderData& base_layout, const VertexShaderSource& source)
{
	auto layout_compiler = compile_attribute_layouts(base_layout, {source.layout});
	const auto geom_layout = get_geom_layout(layout_compiler);
	const auto compiled_layout = compile_shader_layout(layout_compiler, source.layout);

	auto program = std::make_shared<ShaderProgram>(source.vertex, source.fragment, compiled_layout);

	return {program, geom_layout};
}

ShaderResource load_shaders(
	const CameraUniformBuffer& desc, const RenderSettings& settings, [[maybe_unused]] const FullScreenInfo& fsi
)
{
	const auto single_color_shader = load_shader_source({}, desc.setup.source);

	// todo(Gustav): fix skybox shader
	const auto skybox_source = load_skybox_source(desc.setup.source);
	const auto skybox_shader = VertexShaderSource{
		ShaderVertexAttributes{{VertexType::position3, "a_position"}},
		skybox_source.vertex,
		skybox_source.fragment
	};

	BaseShaderData global_shader_data = get_vertex_types(single_color_shader.layout);

	ShaderOptions unlit_shader_options;
	unlit_shader_options.use_texture = true;

	ShaderOptions default_shader_options;
	default_shader_options.use_lights = true;
	default_shader_options.use_texture = true;
	default_shader_options.number_of_directional_lights = settings.number_of_directional_lights;
	default_shader_options.number_of_point_lights = settings.number_of_point_lights;
	default_shader_options.number_of_frustum_lights = settings.number_of_frustum_lights;

	auto loaded_unlit = load_shader(
		global_shader_data,
		load_shader_source(unlit_shader_options.with_transparent_cutoff(), desc.setup.source)
	);
	auto loaded_default = load_shader(
		global_shader_data,
		load_shader_source(default_shader_options.with_transparent_cutoff(), desc.setup.source)
	);

	auto loaded_unlit_transparency = load_shader(
		global_shader_data, load_shader_source(unlit_shader_options, desc.setup.source)
	);
	auto loaded_default_transparency = load_shader(
		global_shader_data, load_shader_source(default_shader_options, desc.setup.source)
	);

	// todo(Gustav): should the asserts here be runtime errors? currently all setups are compile-time...
	assert(
		loaded_unlit.geom_layout.debug_types == loaded_unlit_transparency.geom_layout.debug_types
	);
	assert(
		loaded_default.geom_layout.debug_types
		== loaded_default_transparency.geom_layout.debug_types
	);

#if INCLUDE_POST_PROC == 1
	auto pp_invert = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			std::string{PP_VERT_GLSL}, std::string{PP_INVERT_FRAG_GLSL}, fsi.full_scrren_layout
		),
		PostProcSetup::factor
	);
	auto pp_grayscale = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			std::string{PP_VERT_GLSL}, std::string{PP_GRAYSCALE_FRAG_GLSL}, fsi.full_scrren_layout
		),
		PostProcSetup::factor
	);
	auto pp_damage = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			std::string{PP_VERT_GLSL}, std::string{PP_DAMAGE_FRAG_GLSL}, fsi.full_scrren_layout
		),
		PostProcSetup::factor | PostProcSetup::resolution | PostProcSetup::time
	);

	constexpr IsGauss use_gauss =
#if BLUR_USE_GAUSS == 1
		IsGauss::yes
#else
		IsGauss::no
#endif
		;

	auto pp_blurv = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			std::string{PP_VERT_GLSL},
			generate_blur(PP_BLUR_FRAG_GLSL, {BlurType::vertical, BLUR_SAMPLES, use_gauss}),
			fsi.full_scrren_layout
		),
		PostProcSetup::factor
	);
	auto pp_blurh = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			std::string{PP_VERT_GLSL},
			generate_blur(PP_BLUR_FRAG_GLSL, {BlurType::horizontal, BLUR_SAMPLES, use_gauss}),
			fsi.full_scrren_layout
		),
		PostProcSetup::factor | PostProcSetup::resolution
	);
#endif

	return {
		LoadedShader_SingleColor{load_shader(global_shader_data, single_color_shader), desc},
		LoadedShader_Skybox{load_shader({}, skybox_shader), desc},
		LoadedShader_Unlit{
			loaded_unlit.geom_layout,
			Base_LoadedShader_Unlit{loaded_unlit, desc},
			Base_LoadedShader_Unlit{loaded_unlit_transparency, desc}
		},
		LoadedShader_Default{
			loaded_default.geom_layout,
			Base_LoadedShader_Default{loaded_default, settings, desc},
			Base_LoadedShader_Default{loaded_default_transparency, settings, desc}
		}
#if INCLUDE_POST_PROC == 1
		,
		pp_invert,
		pp_grayscale,
		pp_damage,
		pp_blurv,
		pp_blurh
#endif
	};
}

// ------------------------------------------------------------------------------------------------
// material

void bind_texture_2d(OpenglStates* states, const Uniform& uniform, const Texture2d& texture)
{
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.texture >= 0);

	StateChanger{states}
		.activate_texture(uniform.texture)
		.bind_texture_2d(uniform.texture, texture.id);
}

void bind_texture_cubemap(
	OpenglStates* states, const Uniform& uniform, const TextureCubemap& texture
)
{
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.texture >= 0);

	StateChanger{states}
		.activate_texture(uniform.texture)
		.bind_texture_cubemap(uniform.texture, texture.id);
}

UnlitMaterial::UnlitMaterial(const ShaderResource& resource)
	: shader(&resource.unlit_shader)
{
}

void UnlitMaterial::use_shader(const RenderContext& rc)
{
	shader->base(rc).program->use();
}

void UnlitMaterial::set_uniforms(
	const RenderContext& rc, const CompiledCamera&, const glm::mat4& transform
)
{
	const auto& base = shader->base(rc);
	base.program->set_vec4(base.tint_color, {color, alpha});
	base.program->set_mat(base.model, transform);
}

void UnlitMaterial::bind_textures(const RenderContext& rc, OpenglStates* states, Assets* assets)
{
	std::shared_ptr<Texture2d> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}

	bind_texture_2d(states, shader->base(rc).tex_diffuse, *t);
}

void UnlitMaterial::
	apply_lights(const RenderContext&, const Lights&, const RenderSettings&, OpenglStates*, Assets*)
{
	// no lights for unlit material
}

bool UnlitMaterial::is_transparent() const
{
	// todo(Gustav): improve trasparency
	return alpha < ALPHA_TRANSPARENCY_LIMIT;
}

DefaultMaterial::DefaultMaterial(const ShaderResource& resource)
	: shader(&resource.default_shader)
{
}

void DefaultMaterial::use_shader(const RenderContext& rc)
{
	shader->base(rc).program->use();
}

void DefaultMaterial::set_uniforms(
	const RenderContext& rc, const CompiledCamera& cc, const glm::mat4& transform
)
{
	const auto& base = shader->base(rc);

	base.program->set_vec4(base.tint_color, {color, alpha});
	base.program->set_vec3(base.ambient_tint, ambient_tint);
	base.program->set_vec3(base.specular_color, specular_color);
	base.program->set_float(base.shininess, shininess);
	base.program->set_float(base.emissive_factor, emissive_factor);

	base.program->set_mat(base.model, transform);
	base.program->set_vec3(base.view_position, cc.position);
}

std::shared_ptr<Texture2d> get_or_white(Assets* assets, std::shared_ptr<Texture2d> t)
{
	if (t == nullptr)
	{
		return assets->get_white();
	}
	else
	{
		return t;
	}
};

std::shared_ptr<Texture2d> get_or_black(Assets* assets, std::shared_ptr<Texture2d> t)
{
	if (t == nullptr)
	{
		return assets->get_black();
	}
	else
	{
		return t;
	}
};

void DefaultMaterial::bind_textures(const RenderContext& rc, OpenglStates* states, Assets* assets)
{
	const auto& base = shader->base(rc);
	bind_texture_2d(states, base.tex_diffuse, *get_or_white(assets, diffuse));
	bind_texture_2d(states, base.tex_specular, *get_or_white(assets, specular));
	bind_texture_2d(states, base.tex_emissive, *get_or_black(assets, emissive));
}

void DefaultMaterial::apply_lights(
	const RenderContext& rc,
	const Lights& lights,
	const RenderSettings& settings,
	OpenglStates* states,
	Assets* assets
)
{
	const auto& base = shader->base(rc);
	base.program->set_vec3(base.light_ambient_color, lights.color * lights.ambient);

	const auto no_directional_light = ([]() {
		DirectionalLight p;
		p.color = colors::black;
		p.diffuse = 0.0f;
		p.specular = 0.0f;
		return p;
	})();
	const auto no_point_light = ([]() {
		PointLight p;
		p.color = colors::black;
		p.diffuse = 0.0f;
		p.specular = 0.0f;
		return p;
	})();
	const auto no_frustum_light = ([]() {
		FrustumLight p;
		p.color = colors::black;
		p.diffuse = 0.0f;
		p.specular = 0.0f;
		return p;
	})();

	// todo(Gustav): graph the most influental lights instead of the first N lights
	for (int i = 0; i < settings.number_of_directional_lights; i += 1)
	{
		const auto& p = Cint_to_sizet(i) < lights.directional_lights.size()
						  ? lights.directional_lights[Cint_to_sizet(i)]
						  : no_directional_light;
		const auto& u = base.directional_lights[Cint_to_sizet(i)];
		base.program->set_vec3(u.light_diffuse_color, p.color * p.diffuse);
		base.program->set_vec3(u.light_specular_color, p.color * p.specular);
		base.program->set_vec3(u.dir, p.direction);
	}

	for (int i = 0; i < settings.number_of_point_lights; i += 1)
	{
		const auto& p = Cint_to_sizet(i) < lights.point_lights.size()
						  ? lights.point_lights[Cint_to_sizet(i)]
						  : no_point_light;
		const auto& u = base.point_lights[Cint_to_sizet(i)];
		base.program->set_vec3(u.light_diffuse_color, p.color * p.diffuse);
		base.program->set_vec3(u.light_specular_color, p.color * p.specular);
		base.program->set_vec3(u.light_world, p.position);
		base.program->set_vec4(
			u.light_attenuation, {p.min_range, p.max_range, p.curve.curve.s, p.curve.curve.t}
		);
	}

	for (int i = 0; i < settings.number_of_frustum_lights; i += 1)
	{
		const auto& p = Cint_to_sizet(i) < lights.frustum_lights.size()
						  ? lights.frustum_lights[Cint_to_sizet(i)]
						  : no_frustum_light;
		const auto& u = base.frustum_lights[Cint_to_sizet(i)];
		base.program->set_vec3(u.diffuse, p.color * p.diffuse);
		base.program->set_vec3(u.specular, p.color * p.specular);
		base.program->set_vec3(u.world_pos, p.position);
		base.program->set_vec4(
			u.attenuation, {p.min_range, p.max_range, p.curve.curve.s, p.curve.curve.t}
		);

		const auto view = create_view_mat(p.position, create_vectors(p.yaw, p.pitch));
		const auto projection = glm::perspective(glm::radians(p.fov), p.aspect, 0.1f, p.max_range);
		base.program->set_mat(u.world_to_clip, projection * view);

		bind_texture_2d(states, u.cookie, *get_or_white(assets, p.cookie));
	}
}

bool DefaultMaterial::is_transparent() const
{
	// todo(Gustav): improve trasparency
	return alpha < ALPHA_TRANSPARENCY_LIMIT;
}

// ------------------------------------------------------------------------------------------------
// post proc

std::shared_ptr<FrameBuffer> FrameBufferCache::get(
	glm::ivec2 size, TextureEdge edge, TextureRenderStyle render_style, Transparency transperency
) const
{
	// todo(Gustav): reuse buffers created from a earlier build
	// todo(Gustav): reuse buffers from earlier in the stack, that aren't in use
	auto buffer = create_frame_buffer(size.x, size.y, edge, render_style, transperency);
	return buffer;
}

bool Effect::enabled() const
{
	return is_enabled;
}

void Effect::set_enabled(bool n)
{
	if (is_enabled == n) return;
	is_enabled = n;
	if (owner)
	{
		owner->dirty = true;
	}
}

struct RenderWorld : RenderSource
{
	void render(const PostProcArg& arg) override
	{
		arg.renderer->render_world(arg.window_size, *arg.world, *arg.camera);
	}
};

RenderTask::RenderTask(
	std::shared_ptr<RenderSource> s, std::shared_ptr<FrameBuffer> f, ShaderPropertyProvider* e
)
	: source(s)
	, fbo(f)
	, effect(e)
{
	ASSERT(effect);
}

void RenderTask::render(const PostProcArg& arg)
{
	ASSERT(effect);

	StateChanger{&arg.renderer->pimpl->states}
		.cull_face(false)
		.stencil_test(false)
		.depth_test(false)
		.depth_mask(false)
		.blending(false);

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	effect->use_shader(arg, fbo->texture);
	render_geom(arg.renderer->pimpl->full_screen_geom);
}

void RenderTask::update(const PostProcArg& arg)
{
	auto bound = BoundFbo{fbo};
	set_gl_viewport({fbo->texture.width, fbo->texture.height});
	source->render(arg);
}

void EffectStack::update(float dt) const
{
	for (auto e: effects)
	{
		if (e->is_enabled == false)
		{
			continue;
		}

		e->update(dt);
	}
}

void EffectStack::render(const PostProcArg& arg)
{
	// set the owner, if any new then we are dirty
	for (auto e: effects)
	{
		if (e->owner != this)
		{
			e->owner = this;
			dirty = true;
		}
	}

	if (window_size.has_value() == false || *window_size != arg.window_size)
	{
		window_size = arg.window_size;
		dirty = true;
	}

	// if dirty, update the compiled state
	if (dirty)
	{
		dirty = false;
		LOG_INFO("Bulding effects stack");

		compiled.targets.clear();
		compiled.last_source = std::make_shared<RenderWorld>();

		for (auto& e: effects)
		{
			if (e->is_enabled)
			{
				e->build({&compiled, &fbos, arg.window_size});
			}
		}
	}

	// the stack is now compiled
	// before rendering, update all targets/fbos and present
	for (auto& action: compiled.targets)
	{
		action->update(arg);
	}

	// render the final image to the screen
	set_gl_viewport(arg.window_size);
	compiled.last_source->render(arg);
}

void EffectStack::gui() const
{
	int index = 0;
	for (auto e: effects)
	{
		ImGui::PushID(index);
		e->gui();
		ImGui::PopID();
		index += 1;
	}
}

FactorEffect::FactorEffect()
{
	set_enabled(false);
}

float FactorEffect::get_factor() const
{
	return factor;
}

void FactorEffect::set_factor(float f)
{
	factor = f;
	set_enabled(factor > ALMOST_ZERO);
}

struct ShaderProp
{
	virtual ~ShaderProp() = default;

	virtual void use(const PostProcArg& a, ShaderProgram& shader) = 0;
	virtual void gui() = 0;
};

struct FloatDragShaderProp : ShaderProp
{
	Uniform uniform;
	std::string name;
	float value;
	float speed;

	FloatDragShaderProp(
		std::shared_ptr<LoadedPostProcShader> shader, const std::string& n, float v, float s
	)
		: uniform(shader->program->get_uniform(n))
		, name(n)
		, value(v)
		, speed(s)
	{
	}

	void use(const PostProcArg&, ShaderProgram& shader) override
	{
		shader.set_float(uniform, value);
	}

	void gui() override
	{
		ImGui::DragFloat(name.c_str(), &value, speed);
	}
};

struct FloatSliderShaderProp : ShaderProp
{
	Uniform uniform;
	std::string name;
	float value;
	float min;
	float max;

	FloatSliderShaderProp(
		std::shared_ptr<LoadedPostProcShader> shader,
		const std::string& n,
		float v,
		float mi,
		float ma
	)
		: uniform(shader->program->get_uniform(n))
		, name(n)
		, value(v)
		, min(mi)
		, max(ma)
	{
	}

	void use(const PostProcArg&, ShaderProgram& shader) override
	{
		shader.set_float(uniform, value);
	}

	void gui() override
	{
		ImGui::SliderFloat(name.c_str(), &value, min, max);
	}
};

struct SimpleEffect
	: FactorEffect
	, ShaderPropertyProvider
{
	std::string name;
	std::shared_ptr<LoadedPostProcShader> shader;
	std::vector<std::shared_ptr<ShaderProp>> properties;
	float time = 0.0f;

	SimpleEffect(const std::string& n, std::shared_ptr<LoadedPostProcShader> s)
		: name(n)
		, shader(s)
	{
		ASSERT(shader->factor.has_value());
	}

	void add_float_drag_prop(const std::string& prop_name, float value, float speed)
	{
		properties.emplace_back(
			std::make_shared<FloatDragShaderProp>(shader, prop_name, value, speed)
		);
	}

	void add_float_slider_prop(const std::string& prop_name, float value, float min, float max)
	{
		properties.emplace_back(
			std::make_shared<FloatSliderShaderProp>(shader, prop_name, value, min, max)
		);
	}

	void gui() override
	{
		if (properties.empty())
		{
			return;
		}

		int index = 0;

		if (ImGui::CollapsingHeader(name.c_str()) == false)
		{
			return;
		}

		for (auto& p: properties)
		{
			ImGui::PushID(index);
			p->gui();
			ImGui::PopID();
			index += 1;
		}
	}

	void update(float dt) override
	{
		time += dt;
	}

	void use_shader(const PostProcArg& a, const Texture2d& t) override
	{
		shader->program->use();
		if (shader->factor)
		{
			shader->program->set_float(*shader->factor, get_factor());
		}
		if (shader->resolution)
		{
			shader->program->set_vec2(*shader->resolution, a.window_size);
		}
		if (shader->time)
		{
			shader->program->set_float(*shader->time, time);
		}
		for (auto& p: properties)
		{
			p->use(a, *shader->program);
		}
		bind_texture_2d(&a.renderer->pimpl->states, shader->texture, t);
	}

	void build(const BuildArg& arg) override
	{
		time = 0.0f;

		auto fbo = arg.fbo->get(
			arg.window_size, TextureEdge::clamp, TextureRenderStyle::linear, Transparency::exclude
		);

		auto src = arg.builder->last_source;
		auto target = std::make_shared<RenderTask>(src, fbo, this);

		arg.builder->targets.emplace_back(target);
		arg.builder->last_source = target;
	}
};

struct BlurEffect;

struct VertProvider : ShaderPropertyProvider
{
	explicit VertProvider(BlurEffect* b)
		: blur(b)
	{
	}

	BlurEffect* blur;
	void use_shader(const PostProcArg& a, const Texture2d& t) override;
};

struct HoriProvider : ShaderPropertyProvider
{
	explicit HoriProvider(BlurEffect* b)
		: blur(b)
	{
	}

	BlurEffect* blur;
	void use_shader(const PostProcArg& a, const Texture2d& t) override;
};

struct BlurEffect : FactorEffect
{
	std::string name;
	VertProvider vert_p;
	HoriProvider hori_p;
	std::shared_ptr<LoadedPostProcShader> vert;
	std::shared_ptr<LoadedPostProcShader> hori;

	Uniform blur_size_v;
	Uniform blur_size_h;

#if BLUR_USE_GAUSS == 1
	Uniform std_dev_v;
	Uniform std_dev_h;
#endif

	float blur_size = 0.02f;
#if BLUR_USE_GAUSS == 1
	float std_dev = 0.02f;
#endif

	BlurEffect(
		const std::string& n,
		std::shared_ptr<LoadedPostProcShader> v,
		std::shared_ptr<LoadedPostProcShader> h
	)
		: name(n)
		, vert_p(this)
		, hori_p(this)
		, vert(v)
		, hori(h)
		, blur_size_v(vert->program->get_uniform("u_blur_size"))
		, blur_size_h(hori->program->get_uniform("u_blur_size"))
#if BLUR_USE_GAUSS == 1
		, std_dev_v(vert->program->get_uniform("u_std_dev"))
		, std_dev_h(hori->program->get_uniform("u_std_dev"))
#endif
	{
		ASSERT(vert->factor.has_value());
		ASSERT(hori->factor.has_value());
	}

	void gui() override
	{
		if (ImGui::CollapsingHeader(name.c_str()) == false)
		{
			return;
		}

		ImGui::SliderFloat("Blur size", &blur_size, 0.0f, 0.2f);
#if BLUR_USE_GAUSS == 1
		ImGui::SliderFloat("Standard deviation", &std_dev, 0.0f, 0.1f);
#endif
	}

	void update(float) override
	{
		// no update needed
	}

	void use_vert_shader(const PostProcArg& a, const Texture2d& t) const
	{
		vert->program->use();
		ASSERT(vert->factor);
		vert->program->set_float(*vert->factor, get_factor());
		vert->program->set_float(blur_size_v, blur_size);
#if BLUR_USE_GAUSS == 1
		vert->program->set_float(std_dev_v, std_dev);
#endif
		bind_texture_2d(&a.renderer->pimpl->states, vert->texture, t);
	}

	void use_hori_shader(const PostProcArg& a, const Texture2d& t)
	{
		hori->program->use();
		ASSERT(hori->factor);
		hori->program->set_float(*hori->factor, get_factor());
		ASSERT(hori->resolution);
		hori->program->set_vec2(*hori->resolution, a.window_size);
		hori->program->set_float(blur_size_h, blur_size);
#if BLUR_USE_GAUSS == 1
		hori->program->set_float(std_dev_h, std_dev);
#endif
		bind_texture_2d(&a.renderer->pimpl->states, hori->texture, t);
	}

	void build(const BuildArg& arg) override
	{
		auto src = arg.builder->last_source;

		// todo(Gustav): modify resolution to get better blur and at a lower cost!

		// step 1: vertical
		auto fbo_v = arg.fbo->get(
			arg.window_size, TextureEdge::clamp, TextureRenderStyle::linear, Transparency::exclude
		);
		auto target_v = std::make_shared<RenderTask>(src, fbo_v, &vert_p);
		arg.builder->targets.emplace_back(target_v);

		// step 2: horizontal
		auto fbo_h = arg.fbo->get(
			arg.window_size, TextureEdge::clamp, TextureRenderStyle::linear, Transparency::exclude
		);
		auto target_h = std::make_shared<RenderTask>(target_v, fbo_h, &hori_p);
		arg.builder->targets.emplace_back(target_h);

		// done
		arg.builder->last_source = target_h;
	}
};

void VertProvider::use_shader(const PostProcArg& a, const Texture2d& t)
{
	blur->use_vert_shader(a, t);
}

void HoriProvider::use_shader(const PostProcArg& a, const Texture2d& t)
{
	blur->use_hori_shader(a, t);
}

#if INCLUDE_POST_PROC == 1
std::shared_ptr<FactorEffect> Renderer::make_invert_effect()
{
	return std::make_shared<SimpleEffect>("Invert", pimpl->shaders.pp_invert);
}

std::shared_ptr<FactorEffect> Renderer::make_grayscale_effect()
{
	return std::make_shared<SimpleEffect>("Grayscale", pimpl->shaders.pp_grayscale);
}

std::shared_ptr<FactorEffect> Renderer::make_damage_effect()
{
	auto r = std::make_shared<SimpleEffect>("Damage", pimpl->shaders.pp_damage);
	r->add_float_drag_prop("u_vignette_radius", 0.13f, 0.01f);
	r->add_float_slider_prop("u_vignette_smoothness", 1.0f, 0.001f, 1.0f);
	r->add_float_slider_prop("u_vignette_darkening", 1.0f, 0.0f, 1.0f);
	r->add_float_drag_prop("u_noise_scale", 25.0f, 1.0f);
	return r;
}

std::shared_ptr<FactorEffect> Renderer::make_blur_effect()
{
	return std::make_shared<BlurEffect>("Blur", pimpl->shaders.pp_blurv, pimpl->shaders.pp_blurh);
}
#endif

// ------------------------------------------------------------------------------------------------
// renderer

u32 create_vertex_array()
{
	u32 vao;
	glGenVertexArrays(1, &vao);
	return vao;
}

void destroy_vertex_array(u32 vao)
{
	glDeleteVertexArrays(1, &vao);
}

void set_gl_viewport(const glm::ivec2& sz)
{
	glViewport(0, 0, sz.x, sz.y);
}

std::shared_ptr<CompiledGeom> compile_geom(
	const Geom& geom, const CompiledGeomVertexAttributes& geom_layout
)
{
	const auto ex = extract_geom(geom, geom_layout);

	const auto vbo = create_buffer();
	const auto vao = create_vertex_array();
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, Csizet_to_glsizeiptr(ex.data.size()), ex.data.data(), GL_STATIC_DRAW
	);

	const auto get_type = [](const ExtractedAttribute& extracted) -> GLenum
	{
		switch (extracted.type)
		{
		case ExtractedAttributeType::Float: return GL_FLOAT;
		default: DIE("invalid extracted attribute"); return GL_FLOAT;
		}
	};

	const auto stride = ex.stride;
	int location = 0;
	std::size_t offset = 0;
	for (const auto& att: ex.attributes)
	{
		const auto normalize = false;
		glVertexAttribPointer(
			Cint_to_gluint(location),
			att.count,
			get_type(att),
			normalize ? GL_TRUE : GL_FALSE,
			Csizet_to_glsizei(stride),
			reinterpret_cast<void*>(offset)
		);
		glEnableVertexAttribArray(Cint_to_gluint(location));

		location += 1;
		offset += att.size;
	}


	const auto ebo = create_buffer();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		Csizet_to_glsizeiptr(sizeof(u32) * ex.indices.size()),
		ex.indices.data(),
		GL_STATIC_DRAW
	);

	return std::make_shared<CompiledGeom>(vbo, vao, ebo, geom_layout, ex.face_size);
}

CompiledGeom::~CompiledGeom()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	destroy_buffer(ebo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	destroy_buffer(vbo);

	glBindVertexArray(0);
	destroy_vertex_array(vao);
}

Renderer::Renderer(const RenderSettings& set)
	: settings(set)
	, pimpl(std::make_unique<RendererPimpl>(set, FullScreenInfo{}))
{
}

Renderer::~Renderer() = default;

Skybox Renderer::make_skybox(std::shared_ptr<TextureCubemap> texture) const
{
	constexpr float size = 1.0f;
	constexpr bool invert = true;

	const auto layout = pimpl->shaders.skybox_shader.geom_layout;

	const auto triangle = geom::create_box(size, size, size, invert, colors::white).to_geom();
	auto geom = compile_geom(triangle, layout);

	LOG_INFO("Creating skybox");
	return {geom, texture};
}

struct TransparentMesh
{
	std::shared_ptr<MeshInstance> mesh;
	float squared_distance_to_camera;
};

glm::mat4 get_mesh_rotation_matrix(const MeshInstance* m)
{
	return glm::yawPitchRoll(m->rotation.x, m->rotation.y, m->rotation.z);
}

LocalAxis MeshInstance::get_local_axis() const
{
	const auto m = get_mesh_rotation_matrix(this);
	return {
		glm::vec3{m * glm::vec4{1, 0, 0, 0}},
		glm::vec3{m * glm::vec4{0, 1, 0, 0}},
		glm::vec3{m * glm::vec4{0, 0, 1, 0}}
	};
}

glm::mat4 rot_from_basis(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	return glm::mat4{glm::vec4{a, 0}, glm::vec4{b, 0}, glm::vec4{c, 0}, glm::vec4{0, 0, 0, 1}};
}

glm::mat4 calc_mesh_transform(std::shared_ptr<MeshInstance> m, const CompiledCamera& cc)
{
	const auto calc_fixed_right = [](const glm::vec3& normal, const glm::vec3& up)
	{
		const auto right = glm::normalize(glm::cross(normal, up));
		const auto new_up = glm::normalize(glm::cross(right, normal));
		return rot_from_basis(right, new_up, normal);
	};

	const auto calc_fixed_up = [](const glm::vec3& normal, const glm::vec3& up)
	{
		const auto right = glm::normalize(glm::cross(normal, up));
		const auto new_normal = glm::normalize(glm::cross(right, up));
		return rot_from_basis(right, up, new_normal);
	};

	const auto translation = glm::translate(glm::mat4(1.0f), m->position);

	// todo(Gustav): verify that the billboards are oriented correctly, grass in example 3 is twosided...
	switch (m->billboarding)
	{
	case Billboarding::screen:
		{
			const auto rotation
				= calc_fixed_right(glm::normalize(m->position - cc.position), glm::vec3{0, 1, 0});
			return translation * rotation;
		}
	case Billboarding::screen_fast:
		{
			// todo(Gustav): move to precalculated or remove?
			const auto rotation = calc_fixed_right(cc.in, glm::vec3{0, 1, 0});
			return translation * rotation;
		}
	case Billboarding::axial_y:
		{
			const auto rotation
				= calc_fixed_up(glm::normalize(m->position - cc.position), glm::vec3{0, 1, 0});
			return translation * rotation;
		}
	case Billboarding::axial_y_fast:
		{
			// todo(Gustav): move to precalculated or remove?
			const auto rotation = calc_fixed_up(cc.in, glm::vec3{0, 1, 0});
			return translation * rotation;
		}
	default:
		{
			const auto rotation = get_mesh_rotation_matrix(m.get());
			return translation * rotation;
		}
	}
};

void render_geom(std::shared_ptr<CompiledGeom> geom)
{
	ASSERT(is_bound_for_shader(geom->debug_types));
	glBindVertexArray(geom->vao);
	glDrawElements(GL_TRIANGLES, geom->number_of_triangles * 3, GL_UNSIGNED_INT, nullptr);
}

void render_debug_lines(
	Renderer* r, const CompiledCamera& compiled_camera, const glm::ivec2& window_size
)
{
	int line_state = -1;
	constexpr int line_state_solid = 1;
	constexpr int line_state_dash = 0;

	// todo(Gustav): draw solid lines here
	r->pimpl->debug.line_shader.use();
	r->pimpl->debug.line_shader.set_mat(
		r->pimpl->debug.line_projection, compiled_camera.projection
	);
	r->pimpl->debug.line_shader.set_mat(r->pimpl->debug.line_view, compiled_camera.view);
	for (const auto& line: r->debug.debug_lines)
	{
		if (line.style == LineStyle::always_visible) continue;
		StateChanger{&r->pimpl->states}.depth_func(Compare::less).depth_test(true);
		if (line_state != line_state_solid)
		{
			r->pimpl->debug.set_line_line();
			line_state = line_state_solid;
		}
		r->pimpl->debug.line_batch.line(line.from, line.to, line.color);
	}
	r->pimpl->debug.line_batch.submit();

	for (const auto& line: r->debug.debug_lines)
	{
		if (line.style != LineStyle::always_visible) continue;
		StateChanger{&r->pimpl->states}.depth_test(false);
		if (line_state != line_state_solid)
		{
			r->pimpl->debug.set_line_line();
			line_state = line_state_solid;
		}
		r->pimpl->debug.line_batch.line(line.from, line.to, line.color);
	}
	r->pimpl->debug.line_batch.submit();

	// todo(Gustav): start drawing dashed lines here
	for (const auto& line: r->debug.debug_lines)
	{
		if (line.style != LineStyle::dashed_when_hidden) continue;
		StateChanger{&r->pimpl->states}.depth_func(Compare::greater).depth_test(false);
		if (line_state != line_state_dash)
		{
			r->pimpl->debug.set_line_dash({window_size.x, window_size.y}, 20.0f, 20.0f);
			line_state = line_state_dash;
		}
		r->pimpl->debug.line_batch.line(line.from, line.to, line.color);
	}
	r->pimpl->debug.line_batch.submit();
	r->debug.debug_lines.clear();
}

void Renderer::render_world(const glm::ivec2& window_size, const World& world, const Camera& camera)
{
	const auto has_outlined_meshes = std::any_of(
		world.meshes.begin(),
		world.meshes.end(),
		[](const auto& mesh) { return mesh->outline.has_value(); }
	);
	StateChanger{&pimpl->states}
		.cull_face(true)
		.cull_face_mode(CullFace::back)
		.stencil_mask(0xFF)
		.stencil_test(has_outlined_meshes)
		.depth_test(true)
		.depth_mask(true)
		.stencil_op(StencilAction::keep, StencilAction::replace, StencilAction::replace)
		.blend_mode(Blend::src_alpha, Blend::one_minus_src_alpha);

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	const auto compiled_camera = compile(camera, window_size);

	auto bound_camera_buffer = BoundUniformBuffer{pimpl->camera_uniform_buffer.buffer.get()};
	pimpl->camera_uniform_buffer.set_props(compiled_camera);

	std::vector<TransparentMesh> transparent_meshes;

	for (auto& m: world.meshes)
	{
		const auto not_transparent = RenderContext{UseTransparency::no};

		if (m->material->is_transparent())
		{
			transparent_meshes.emplace_back(
				TransparentMesh{m, glm::length2(camera.position - m->position)}
			);
			continue;
		}
		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(true)
			.depth_func(Compare::less)
			.blending(false)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);

		if (m->outline)
		{
			StateChanger{&pimpl->states}.stencil_func(Compare::always, 1, 0xFF).stencil_mask(0xFF);
		}
		m->material->use_shader(not_transparent);
		m->material->set_uniforms(
			not_transparent, compiled_camera, calc_mesh_transform(m, compiled_camera)
		);
		m->material->bind_textures(not_transparent, &pimpl->states, &assets);
		m->material->apply_lights(not_transparent, world.lights, settings, &pimpl->states, &assets);

		render_geom(m->geom);
	}

	render_debug_lines(this, compiled_camera, window_size);

	// render skybox
	if (world.skybox.cubemap != nullptr && world.skybox.geom != nullptr)
	{
		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(false)
			.depth_func(Compare::less_equal)
			.blending(false)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);

		auto& shader = pimpl->shaders.skybox_shader;

		shader.program->use();
		bind_texture_cubemap(&pimpl->states, shader.tex_skybox, *world.skybox.cubemap);

		render_geom(world.skybox.geom);
	}

	std::sort(
		transparent_meshes.begin(),
		transparent_meshes.end(),
		[](const auto& lhs, const auto& rhs)
		{ return lhs.squared_distance_to_camera > rhs.squared_distance_to_camera; }
	);

	for (auto& tm: transparent_meshes)
	{
		const auto transparent = RenderContext{UseTransparency::yes};

		const auto& m = tm.mesh;
		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(true)
			.depth_func(Compare::less)
			.blending(true)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);

		if (m->outline)
		{
			StateChanger{&pimpl->states}.stencil_func(Compare::always, 1, 0xFF).stencil_mask(0xFF);
		}
		m->material->use_shader(transparent);
		m->material->set_uniforms(
			transparent, compiled_camera, calc_mesh_transform(m, compiled_camera)
		);
		m->material->bind_textures(transparent, &pimpl->states, &assets);
		m->material->apply_lights(transparent, world.lights, settings, &pimpl->states, &assets);

		render_geom(m->geom);
	}

	// render outline over all other meshes
	if (has_outlined_meshes)
	{
		for (auto& m: world.meshes)
		{
			if (m->outline)
			{
				StateChanger{&pimpl->states}
					.stencil_func(Compare::not_equal, 1, 0xFF)
					.stencil_mask(0x00)
					.depth_test(false);
				const auto small_scale = 1.1;
				const auto small_scale_mat
					= glm::scale(glm::mat4(1.0f), {small_scale, small_scale, small_scale});

				auto& shader = pimpl->shaders.single_color_shader;
				shader.program->use();
				shader.program->set_vec4(shader.tint_color, {*m->outline, 1});

				shader.program->set_mat(
					shader.model, calc_mesh_transform(m, compiled_camera) * small_scale_mat
				);

				render_geom(m->geom);
			}
		}
	}
}


}  //  namespace klotter
