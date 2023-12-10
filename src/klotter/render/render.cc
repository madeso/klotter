#include "klotter/render/render.h"

#include "klotter/cint.h"
#include "klotter/assert.h"
#include "klotter/str.h"

#include "klotter/render/opengl_utils.h"
#include "klotter/render/shader.source.h"
#include "klotter/render/geom.extract.h"

#include <string_view>

namespace klotter
{

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
		const auto Csa = [](StencilAction sa)
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

	StateChanger& bind_texture(int slot, unsigned int texture)
	{
		ASSERT(slot == states->active_texture);
		if (should_change(&states->texture_bound[Cint_to_sizet(slot)], texture))
		{
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		return *this;
	}
};

// ------------------------------------------------------------------------------------------------
// ShaderResource

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
	LoadedShader_SingleColor(LoadedShader s)
		: LoadedShader(std::move(s.program), s.geom_layout)
		, tint_color(program->get_uniform("u_material.diffuse_tint"))
		, model(program->get_uniform("u_model"))
		, projection(program->get_uniform("u_projection"))
		, view(program->get_uniform("u_view"))
	{
	}

	Uniform tint_color;

	Uniform model;
	Uniform projection;
	Uniform view;
};

struct LoadedShader_Unlit : LoadedShader
{
	LoadedShader_Unlit(LoadedShader s)
		: LoadedShader(std::move(s.program), s.geom_layout)
		, tint_color(program->get_uniform("u_material.diffuse_tint"))
		, tex_diffuse(program->get_uniform("u_material.diffuse_tex"))
		, model(program->get_uniform("u_model"))
		, projection(program->get_uniform("u_projection"))
		, view(program->get_uniform("u_view"))
	{
		setup_textures(program.get(), {&tex_diffuse});
	}

	Uniform tint_color;
	Uniform tex_diffuse;

	Uniform model;
	Uniform projection;
	Uniform view;
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

struct LoadedShader_Default : LoadedShader
{
	LoadedShader_Default(LoadedShader s, const RenderSettings& settings)
		: LoadedShader(std::move(s.program), s.geom_layout)
		, tint_color(program->get_uniform("u_material.diffuse_tint"))
		, tex_diffuse(program->get_uniform("u_material.diffuse_tex"))
		, tex_specular(program->get_uniform("u_material.specular_tex"))
		, tex_emissive(program->get_uniform("u_material.emissive_tex"))
		, ambient_tint(program->get_uniform("u_material.ambient_tint"))
		, specular_color(program->get_uniform("u_material.specular_tint"))
		, shininess(program->get_uniform("u_material.shininess"))
		, emissive_factor(program->get_uniform("u_material.emissive_factor"))
		, model(program->get_uniform("u_model"))
		, projection(program->get_uniform("u_projection"))
		, view(program->get_uniform("u_view"))
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
	Uniform projection;
	Uniform view;

	Uniform view_position;
	Uniform light_ambient_color;

	std::vector<DirectionalLightUniforms> directional_lights;
	std::vector<PointLightUniforms> point_lights;
	std::vector<FrustumLightUniforms> frustum_lights;
};

struct ShaderResource
{
	ShaderResource(
		LoadedShader single_color,
		LoadedShader unlit,
		LoadedShader def,
		const RenderSettings& settings
	)
		: single_color_shader(std::move(single_color))
		, unlit_shader(std::move(unlit))
		, default_shader(std::move(def), settings)
	{
	}

	LoadedShader_SingleColor single_color_shader;
	LoadedShader_Unlit unlit_shader;
	LoadedShader_Default default_shader;

	/// verify that the shaders are loaded
	bool is_loaded() const
	{
		return unlit_shader.program->is_loaded() && default_shader.program->is_loaded();
	}
};

ShaderResource load_shaders(const RenderSettings& settings);

struct RendererPimpl
{
	ShaderResource shaders;
	OpenglStates states;

	RendererPimpl(const RenderSettings& set)
		: shaders(load_shaders(set))
	{
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

CompiledGeomVertexAttributes Renderer::unlit_geom_layout()
{
	return pimpl->shaders.unlit_shader.geom_layout;
}

CompiledGeomVertexAttributes Renderer::default_geom_layout()
{
	return pimpl->shaders.default_shader.geom_layout;
}

bool Renderer::is_loaded() const
{
	return pimpl->shaders.is_loaded();
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

LoadedShader load_shader(const BaseShaderData& base_layout, const ShaderSource& source)
{
	auto layout_compiler = compile_attribute_layouts(base_layout, {source.layout});
	const auto geom_layout = get_geom_layout(layout_compiler);
	const auto compiled_layout = compile_shader_layout(layout_compiler, source.layout);

	auto program = std::make_shared<ShaderProgram>(source.vertex, source.fragment, compiled_layout);

	return {program, geom_layout};
}

ShaderResource load_shaders(const RenderSettings& settings)
{
	const auto single_color_shader = load_shader_source({});

	BaseShaderData global_shader_data = get_vertex_types(single_color_shader.layout);

	ShaderOptions unlit_shader_options;
	unlit_shader_options.use_texture = true;

	ShaderOptions default_shader_options;
	default_shader_options.use_lights = true;
	default_shader_options.use_texture = true;
	default_shader_options.number_of_directional_lights = settings.number_of_directional_lights;
	default_shader_options.number_of_point_lights = settings.number_of_point_lights;
	default_shader_options.number_of_frustum_lights = settings.number_of_frustum_lights;

	return {
		load_shader(global_shader_data, single_color_shader),
		load_shader(global_shader_data, load_shader_source(unlit_shader_options)),
		load_shader(global_shader_data, load_shader_source(default_shader_options)),
		settings};
}

// ------------------------------------------------------------------------------------------------
// material

void bind_texture(OpenglStates* states, const Uniform& uniform, const Texture& texture)
{
	if (uniform.is_valid() == false)
	{
		return;
	}
	ASSERT(uniform.texture >= 0);

	StateChanger{states}
		.activate_texture(uniform.texture)
		.bind_texture(uniform.texture, texture.id);

	// glActiveTexture(Cint_to_glenum(GL_TEXTURE0 + uniform.texture));
	// glBindTexture(GL_TEXTURE_2D, texture.id);
}

UnlitMaterial::UnlitMaterial(const ShaderResource& resource)
	: shader(&resource.unlit_shader)
	, color(colors::white)
	, alpha(1.0f)
{
}

void UnlitMaterial::use_shader()
{
	shader->program->use();
}

void set_model_projection_view(
	std::shared_ptr<ShaderProgram> program,
	Uniform model,
	Uniform projection,
	Uniform view,
	const CompiledCamera& cc,
	const glm::mat4& transform
)
{
	program->set_mat(model, transform);
	program->set_mat(projection, cc.projection);
	program->set_mat(view, cc.view);
}

void UnlitMaterial::set_uniforms(const CompiledCamera& cc, const glm::mat4& transform)
{
	shader->program->set_vec4(shader->tint_color, {color, alpha});

	set_model_projection_view(
		shader->program, shader->model, shader->projection, shader->view, cc, transform
	);
}

void UnlitMaterial::bind_textures(OpenglStates* states, Assets* assets)
{
	std::shared_ptr<Texture> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}

	bind_texture(states, shader->tex_diffuse, *t);
}

void UnlitMaterial::apply_lights(const Lights&, const RenderSettings&, OpenglStates*, Assets*)
{
}

bool UnlitMaterial::is_transparent() const
{
	// todo(Gustav): improve trasparency
	return alpha < 0.99f;
}

DefaultMaterial::DefaultMaterial(const ShaderResource& resource)
	: shader(&resource.default_shader)
	, color(colors::white)
	, alpha(1.0f)
	, ambient_tint(colors::white)
	, specular_color(colors::white)
	, shininess(32.0f)
	, emissive_factor(0.0f)
{
}

void DefaultMaterial::use_shader()
{
	shader->program->use();
}

void DefaultMaterial::set_uniforms(const CompiledCamera& cc, const glm::mat4& transform)
{
	shader->program->set_vec4(shader->tint_color, {color, alpha});
	shader->program->set_vec3(shader->ambient_tint, ambient_tint);
	shader->program->set_vec3(shader->specular_color, specular_color);
	shader->program->set_float(shader->shininess, shininess);
	shader->program->set_float(shader->emissive_factor, emissive_factor);

	set_model_projection_view(
		shader->program, shader->model, shader->projection, shader->view, cc, transform
	);
	shader->program->set_vec3(shader->view_position, cc.position);
}

std::shared_ptr<Texture> get_or_white(Assets* assets, std::shared_ptr<Texture> t)
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

std::shared_ptr<Texture> get_or_black(Assets* assets, std::shared_ptr<Texture> t)
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

void DefaultMaterial::bind_textures(OpenglStates* states, Assets* assets)
{
	bind_texture(states, shader->tex_diffuse, *get_or_white(assets, diffuse));
	bind_texture(states, shader->tex_specular, *get_or_white(assets, specular));
	bind_texture(states, shader->tex_emissive, *get_or_black(assets, emissive));
}

void DefaultMaterial::apply_lights(
	const Lights& lights, const RenderSettings& settings, OpenglStates* states, Assets* assets
)
{
	shader->program->set_vec3(shader->light_ambient_color, lights.color * lights.ambient);

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
		const auto& u = shader->directional_lights[Cint_to_sizet(i)];
		shader->program->set_vec3(u.light_diffuse_color, p.color * p.diffuse);
		shader->program->set_vec3(u.light_specular_color, p.color * p.specular);
		shader->program->set_vec3(u.dir, p.direction);
	}

	for (int i = 0; i < settings.number_of_point_lights; i += 1)
	{
		const auto& p = Cint_to_sizet(i) < lights.point_lights.size()
						  ? lights.point_lights[Cint_to_sizet(i)]
						  : no_point_light;
		const auto& u = shader->point_lights[Cint_to_sizet(i)];
		shader->program->set_vec3(u.light_diffuse_color, p.color * p.diffuse);
		shader->program->set_vec3(u.light_specular_color, p.color * p.specular);
		shader->program->set_vec3(u.light_world, p.position);
		shader->program->set_vec4(
			u.light_attenuation, {p.min_range, p.max_range, p.curve.curve.s, p.curve.curve.t}
		);
	}

	for (int i = 0; i < settings.number_of_frustum_lights; i += 1)
	{
		const auto& p = Cint_to_sizet(i) < lights.frustum_lights.size()
						  ? lights.frustum_lights[Cint_to_sizet(i)]
						  : no_frustum_light;
		const auto& u = shader->frustum_lights[Cint_to_sizet(i)];
		shader->program->set_vec3(u.diffuse, p.color * p.diffuse);
		shader->program->set_vec3(u.specular, p.color * p.specular);
		shader->program->set_vec3(u.world_pos, p.position);
		shader->program->set_vec4(
			u.attenuation, {p.min_range, p.max_range, p.curve.curve.s, p.curve.curve.t}
		);

		const auto view = create_view_mat(p.position, create_vectors(p.yaw, p.pitch));
		const auto projection = glm::perspective(glm::radians(p.fov), p.aspect, 0.1f, p.max_range);
		shader->program->set_mat(u.world_to_clip, projection * view);

		bind_texture(states, u.cookie, *get_or_white(assets, p.cookie));
	}
}

bool DefaultMaterial::is_transparent() const
{
	// todo(Gustav): improve trasparency
	return alpha < 0.99f;
}

// ------------------------------------------------------------------------------------------------
// renderer

u32 create_buffer()
{
	u32 vbo;
	glGenBuffers(1, &vbo);
	return vbo;
}

void destroy_buffer(u32 vbo)
{
	glDeleteBuffers(1, &vbo);
}

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
		GL_ARRAY_BUFFER,
		Csizet_to_glsizeiptr(sizeof(float) * ex.vertices.size()),
		ex.vertices.data(),
		GL_STATIC_DRAW
	);

	const auto stride = ex.floats_per_vertex * sizeof(float);
	int location = 0;
	std::size_t offset = 0;
	for (const auto& a_count: ex.attribute_counts)
	{
		const auto normalize = false;
		glVertexAttribPointer(
			Cint_to_gluint(location),
			a_count,
			GL_FLOAT,
			normalize ? GL_TRUE : GL_FALSE,
			Csizet_to_glsizei(stride),
			reinterpret_cast<void*>(offset)
		);
		glEnableVertexAttribArray(Cint_to_gluint(location));

		location += 1;
		offset += Cint_to_sizet(a_count) * sizeof(float);
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
	, pimpl(std::make_unique<RendererPimpl>(set))
{
}

Renderer::~Renderer() = default;

struct TransparentMesh
{
	std::shared_ptr<MeshInstance> mesh;
	float squared_distance_to_camera;
};

void Renderer::render(const glm::ivec2& window_size, const World& world, const Camera& camera)
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
		.stencil_op(StencilAction::keep, StencilAction::replace, StencilAction::replace)
		.blend_mode(Blend::src_alpha, Blend::one_minus_src_alpha);

	glViewport(0, 0, window_size.x, window_size.y);
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	const auto compiled_camera = compile(camera, window_size);

	const auto calc_mesh_transform = [](std::shared_ptr<MeshInstance> m)
	{
		const auto translation = glm::translate(glm::mat4(1.0f), m->position);
		const auto rotation = glm::yawPitchRoll(m->rotation.x, m->rotation.y, m->rotation.z);
		return translation * rotation;
	};
	const auto render_geom = [](std::shared_ptr<CompiledGeom> geom)
	{
		ASSERT(is_bound_for_shader(geom->debug_types));
		glBindVertexArray(geom->vao);
		glDrawElements(GL_TRIANGLES, geom->number_of_triangles * 3, GL_UNSIGNED_INT, 0);
	};

	std::vector<TransparentMesh> transparent_meshes;

	for (auto& m: world.meshes)
	{
		if (m->material->is_transparent())
		{
			transparent_meshes.emplace_back(TransparentMesh{
				m, glm::length2(camera.position - m->position)});
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
		m->material->use_shader();
		m->material->set_uniforms(compiled_camera, calc_mesh_transform(m));
		m->material->bind_textures(&pimpl->states, &assets);
		m->material->apply_lights(world.lights, settings, &pimpl->states, &assets);

		render_geom(m->geom);
	}

	std::sort(
		transparent_meshes.begin(),
		transparent_meshes.end(),
		[](const auto& lhs, const auto& rhs)
		{ return lhs.squared_distance_to_camera > rhs.squared_distance_to_camera; }
	);

	for (auto& tm: transparent_meshes)
	{
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
		m->material->use_shader();
		m->material->set_uniforms(compiled_camera, calc_mesh_transform(m));
		m->material->bind_textures(&pimpl->states, &assets);
		m->material->apply_lights(world.lights, settings, &pimpl->states, &assets);

		render_geom(m->geom);
	}

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
				set_model_projection_view(
					shader.program,
					shader.model,
					shader.projection,
					shader.view,
					compiled_camera,
					calc_mesh_transform(m) * small_scale_mat
				);

				render_geom(m->geom);
			}
		}
	}
}


}  //  namespace klotter
