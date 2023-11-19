#include "klotter/render/render.h"

#include "klotter/cint.h"
#include "klotter/assert.h"

#include "klotter/render/opengl_utils.h"
#include "klotter/render/shader.source.h"
#include "klotter/render/geom.extract.h"

#include <string_view>

namespace klotter
{
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

struct LoadedShader_Unlit : LoadedShader
{
	LoadedShader_Unlit(LoadedShader s)
		: LoadedShader(std::move(s.program), s.geom_layout)
		, tint_color(program->get_uniform("u_tint_color"))
		, tex_diffuse(program->get_uniform("u_tex_diffuse"))
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

struct LoadedShader_Default : LoadedShader
{
	LoadedShader_Default(LoadedShader s)
		: LoadedShader(std::move(s.program), s.geom_layout)
		, tint_color(program->get_uniform("u_tint_color"))
		, tex_diffuse(program->get_uniform("u_tex_diffuse"))
		, model(program->get_uniform("u_model"))
		, projection(program->get_uniform("u_projection"))
		, view(program->get_uniform("u_view"))
		, view_position(program->get_uniform("u_view_position"))
		, light_color(program->get_uniform("u_light_color"))
		, light_world(program->get_uniform("u_light_world"))
	{
		setup_textures(program.get(), {&tex_diffuse});
	}

	Uniform tint_color;
	Uniform tex_diffuse;

	Uniform model;
	Uniform projection;
	Uniform view;

	Uniform view_position;
	Uniform light_color;
	Uniform light_world;
};

struct ShaderResource::ShaderResourcePimpl
{
	ShaderResourcePimpl(LoadedShader unlit, LoadedShader def)
		: unlit_shader(std::move(unlit))
		, default_shader(std::move(def))
	{
	}

	LoadedShader_Unlit unlit_shader;
	LoadedShader_Default default_shader;
};

ShaderResource::~ShaderResource()
{
}

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
	return std::make_shared<UnlitMaterial>(shaders);
}

std::shared_ptr<DefaultMaterial> Renderer::make_default_material()
{
	return std::make_shared<DefaultMaterial>(shaders);
}

CompiledGeomVertexAttributes Renderer::unlit_geom_layout()
{
	return shaders.r->unlit_shader.geom_layout;
}

CompiledGeomVertexAttributes Renderer::default_geom_layout()
{
	return shaders.r->default_shader.geom_layout;
}

bool ShaderResource::is_loaded() const
{
	return r->unlit_shader.program->is_loaded() && r->default_shader.program->is_loaded();
}

bool Renderer::is_loaded() const
{
	return shaders.is_loaded();
}

// ------------------------------------------------------------------------------------------------
// shader loading

using BaseShaderData = std::vector<VertexType>;

LoadedShader load_shader(const BaseShaderData& base_layout, const ShaderSource& source)
{
	auto layout_compiler = compile_attribute_layouts(base_layout, {source.layout});
	const auto geom_layout = get_geom_layout(layout_compiler);
	const auto compiled_layout = compile_shader_layout(layout_compiler, source.layout);

	auto program = std::make_shared<ShaderProgram>(source.vertex, source.fragment, compiled_layout);

	return {program, geom_layout};
}

ShaderResource::ShaderResource()
{
	// todo(Gustav): change so that there are common "base" shaders (example: single color) that
	// can be used for everything and specific shaders (example: pbr)
	auto global_shader_data = BaseShaderData{};


	r = std::make_unique<ShaderResourcePimpl>(
		load_shader(global_shader_data, load_unlit_shader_source()),
		load_shader(global_shader_data, load_default_shader_source())
	);
}

// ------------------------------------------------------------------------------------------------
// material

UnlitMaterial::UnlitMaterial(const ShaderResource& resource)
	: shader(&resource.r->unlit_shader)
	, color(colors::white)
	, alpha(1.0f)
{
}

void UnlitMaterial::use_shader()
{
	shader->program->use();
}

void UnlitMaterial::set_uniforms(const CompiledCamera& cc, const glm::mat4& transform)
{
	shader->program->set_vec4(shader->tint_color, {color, alpha});
	shader->program->set_mat(shader->model, transform);
	shader->program->set_mat(shader->projection, cc.projection);
	shader->program->set_mat(shader->view, cc.view);
}

void UnlitMaterial::bind_textures(Assets* assets)
{
	std::shared_ptr<Texture> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}

	bind_texture(shader->tex_diffuse, *t);
}

void UnlitMaterial::apply_lights(const Lights&)
{
}

DefaultMaterial::DefaultMaterial(const ShaderResource& resource)
	: shader(&resource.r->default_shader)
	, color(colors::white)
	, alpha(1.0f)
{
}

void DefaultMaterial::use_shader()
{
	shader->program->use();
}

void DefaultMaterial::set_uniforms(const CompiledCamera& cc, const glm::mat4& transform)
{
	shader->program->set_vec4(shader->tint_color, {color, alpha});
	shader->program->set_mat(shader->model, transform);
	shader->program->set_mat(shader->projection, cc.projection);
	shader->program->set_mat(shader->view, cc.view);
	shader->program->set_vec3(shader->view_position, cc.position);
}

void DefaultMaterial::bind_textures(Assets* assets)
{
	std::shared_ptr<Texture> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}

	bind_texture(shader->tex_diffuse, *t);
}

void DefaultMaterial::apply_lights(const Lights& lights)
{
	shader->program->set_vec3(shader->light_color, lights.point_light.color);
	shader->program->set_vec3(shader->light_world, lights.point_light.position);
}

// ------------------------------------------------------------------------------------------------
// renderer

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

	StateChanger& blend_mode(BlendMode new_state)
	{
		if (should_change(&states->blend_mode, new_state))
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
			const auto [src, dst] = new_state;
			glBlendFunc(convert(src), convert(dst));
		}
		return *this;
	}
};

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

void Renderer::render(const glm::ivec2& window_size, const World& world, const Camera& camera)
{
	StateChanger{&states}
		.cull_face(true)
		.cull_face_mode(CullFace::back)
		.blend_mode({Blend::src_alpha, Blend::one_minus_src_alpha});

	glViewport(0, 0, window_size.x, window_size.y);
	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	StateChanger{&states}.depth_test(true).blending(false);

	const auto compiled_camera = compile(camera, window_size);

	for (auto& m: world.meshes)
	{
		const auto translation = glm::translate(glm::mat4(1.0f), m->position);
		const auto rotation = glm::yawPitchRoll(m->rotation.x, m->rotation.y, m->rotation.z);
		const auto transform = translation * rotation;

		m->material->use_shader();
		m->material->set_uniforms(compiled_camera, transform);
		m->material->bind_textures(&assets);
		m->material->apply_lights(world.lights);

		ASSERT(is_bound_for_shader(m->geom->debug_types));
		glBindVertexArray(m->geom->vao);
		glDrawElements(GL_TRIANGLES, m->geom->number_of_triangles * 3, GL_UNSIGNED_INT, 0);
	}
}


}  //  namespace klotter
