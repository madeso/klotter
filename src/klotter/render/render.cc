#include "klotter/render/render.h"

#include <string_view>
#include <numeric>

#include "klotter/cint.h"
#include "klotter/assert.h"
#include "klotter/render/opengl_utils.h"
#include "klotter/render/shader.source.h"

// todo(Gustav): split to more files (mesh/material/renderer/states seems okay)
// todo(Gustav): apply more states
// todo(Gustav): rename shaders to unlit and lit

namespace klotter
{

// ------------------------------------------------------------------------------------------------
// mesh & glue code

CompiledMesh::CompiledMesh(u32 b, u32 a, u32 e, MaterialPtr m, i32 tc)
	: vbo(b)
	, vao(a)
	, ebo(e)
	, material(m)
	, number_of_triangles(tc)
{
}

MeshInstancePtr make_MeshInstance(CompiledMeshPtr geom)
{
	auto mesh = std::make_shared<MeshInstance>();
	mesh->geom = geom;
	return mesh;
}

std::shared_ptr<BasicMaterial> Renderer::make_basic_material()
{
	return std::make_shared<BasicMaterial>(shaders);
}

std::shared_ptr<LightMaterial> Renderer::make_light_material()
{
	return std::make_shared<LightMaterial>(shaders);
}

Vertex::Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 u, glm::vec3 c)
	: position(std::move(p))
	, normal(std::move(n))
	, uv(std::move(u))
	, color(std::move(c))
{
}

Mesh::Mesh(std::vector<Vertex> v, std::vector<Face> f)
	: vertices(std::move(v))
	, faces(std::move(f))
{
}

// ------------------------------------------------------------------------------------------------
// shader loading

using BaseShaderData = std::vector<VertexType>;

LoadedShaderData load_shader(const BaseShaderData& base_layout, const ShaderSource& source)
{
	auto layout_compiler = compile_attribute_layouts(base_layout, {source.layout});
	const auto mesh_layout = get_mesh_layout(layout_compiler);
	const auto compiled_layout = compile_shader_layout(layout_compiler, source.layout);

	auto program = std::make_shared<ShaderProgram>(source.vertex, source.fragment, compiled_layout);

	return {program, mesh_layout};
}

ShaderResource::ShaderResource()
{
	auto global_shader_data = BaseShaderData{};
	basic_shader = load_shader(global_shader_data, basic_shader_source());
	light_shader = load_shader(global_shader_data, light_shader_source());
}

// todo(Gustav): remove this member function?
bool ShaderResource::is_loaded() const
{
	return basic_shader.program->is_loaded() && light_shader.program->is_loaded();
}

// ------------------------------------------------------------------------------------------------
// material, part of rendering?

Material::Material(LoadedShaderData s)
	: shader(std::move(s))
{
}

BasicMaterial::BasicMaterial(const ShaderResource& resource)
	: Material(resource.basic_shader)
	, color(colors::white)
	, alpha(1.0f)
{
}

void BasicMaterial::set_uniforms(const CompiledCamera& cc, const glm::mat4& transform)
{
	shader.program->set_vec4(shader.program->get_uniform("u_tint_color"), {color, alpha});
	shader.program->set_mat(shader.program->get_uniform("u_model"), transform);
	shader.program->set_mat(shader.program->get_uniform("u_projection"), cc.projection);
	shader.program->set_mat(shader.program->get_uniform("u_view"), cc.view);
}

void BasicMaterial::bind_textures(Assets* assets)
{
	glActiveTexture(GL_TEXTURE0);
	std::shared_ptr<Texture> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}
	glBindTexture(GL_TEXTURE_2D, t->id);
}

void BasicMaterial::apply_lights(const Lights&)
{
}

LightMaterial::LightMaterial(const ShaderResource& resource)
	: Material(resource.light_shader)
	, color(colors::white)
	, alpha(1.0f)
{
}

void LightMaterial::set_uniforms(const CompiledCamera& cc, const glm::mat4& transform)
{
	shader.program->set_vec4(shader.program->get_uniform("u_tint_color"), {color, alpha});
	shader.program->set_mat(shader.program->get_uniform("u_model"), transform);
	shader.program->set_mat(shader.program->get_uniform("u_projection"), cc.projection);
	shader.program->set_mat(shader.program->get_uniform("u_view"), cc.view);

	shader.program->set_vec3(shader.program->get_uniform("u_view_position"), cc.position);
}

void LightMaterial::bind_textures(Assets* assets)
{
	glActiveTexture(GL_TEXTURE0);
	std::shared_ptr<Texture> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}
	glBindTexture(GL_TEXTURE_2D, t->id);
}

void LightMaterial::apply_lights(const Lights& lights)
{
	shader.program->set_vec3(
		shader.program->get_uniform("u_light_color"), lights.point_light.color
	);
	shader.program->set_vec3(
		shader.program->get_uniform("u_light_world"), lights.point_light.position
	);
}

// ------------------------------------------------------------------------------------------------
// mesh building

std::vector<u32> compile_indices(const Mesh& mesh)
{
	std::vector<u32> indices;
	const auto push = [&indices](const u32 i)
	{
		indices.emplace_back(i);
	};
	for (const auto& f: mesh.faces)
	{
		push(f.a);
		push(f.b);
		push(f.c);
	}
	return indices;
}

using VertexVector = std::vector<float>;

/// Extracted data from mesh for OpenGL
struct ExtractedMesh
{
	VertexVector vertices;
	std::size_t floats_per_vertex;
	std::vector<int> attribute_counts;
	std::vector<u32> indices;
	i32 face_size;
};

void push2(VertexVector* vv, const glm::vec2& p)
{
	vv->emplace_back(p.x);
	vv->emplace_back(p.y);
}

void push3(VertexVector* vv, const glm::vec3& p)
{
	vv->emplace_back(p.x);
	vv->emplace_back(p.y);
	vv->emplace_back(p.z);
}

void push4(VertexVector* vv, const glm::vec4& p)
{
	vv->emplace_back(p.x);
	vv->emplace_back(p.y);
	vv->emplace_back(p.z);
	vv->emplace_back(p.w);
}

struct BufferData
{
	using PerVertex = void (*)(std::vector<float>*, const Vertex&);

	std::vector<int> count;
	std::vector<PerVertex> per_vertex;
};

ExtractedMesh extract_mesh(const Mesh& mesh, const CompiledGeomVertexAttributes& layout)
{
	const auto indices = compile_indices(mesh);

	const auto attribute_descriptions = [&]()
	{
		auto data = BufferData{};
		data.count.reserve(layout.elements.size());
		data.per_vertex.reserve(layout.elements.size());

		for (const auto& element: layout.elements)
		{
			switch (element.type)
			{
#define MAP(VT, PROP, COUNT) \
	case VT: \
		data.count.emplace_back(COUNT); \
		data.per_vertex.emplace_back([](VertexVector* vertices, const Vertex& vertex) \
									 { push##COUNT(vertices, PROP); }); \
		break
				MAP(VertexType::position3, vertex.position, 3);
				MAP(VertexType::normal3, vertex.normal, 3);
				MAP(VertexType::color3, vertex.color, 3);
				MAP(VertexType::color4, glm::vec4(vertex.color, 1.0f), 4);
				MAP(VertexType::texture2, vertex.uv, 2);
#undef MAP
			default: DIE("Invalid buffer type"); break;
			}
		}
		return data;
	}();

	const auto floats_per_vertex = Cint_to_sizet(std::accumulate(
		attribute_descriptions.count.begin(),
		attribute_descriptions.count.end(),
		0,
		[](auto s, const auto& d) { return s + d; }
	));
	const auto vertices = [&]()
	{
		auto verts = VertexVector{};
		verts.reserve(mesh.vertices.size() * floats_per_vertex);
		for (const auto& vertex: mesh.vertices)
		{
			for (const auto& per_vertex: attribute_descriptions.per_vertex)
			{
				per_vertex(&verts, vertex);
			}
		}
		return verts;
	}();

	const auto face_size = static_cast<i32>(mesh.faces.size());

	return {vertices, floats_per_vertex, attribute_descriptions.count, indices, face_size};
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
                    default: return GL_FILL;
                    }
                })();
			glPolygonMode(GL_FRONT_AND_BACK, mode);
		}
		return *this;
	}
};

void opengl_set2d(OpenglStates* states)
{
	StateChanger{states}.depth_test(false).blending(true);
}

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

CompiledMeshPtr compile_Mesh(const Mesh& mesh, MaterialPtr material)
{
	const auto ex = extract_mesh(mesh, material->shader.mesh_layout);

	material->shader.program->use();

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

	// todo(Gustav): disable binding
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // ebo
	// glBindVertexArray(0); // vao
	// glBindBuffer(GL_ARRAY_BUFFER, 0); // vbo


	return std::make_shared<CompiledMesh>(vbo, vao, ebo, material, ex.face_size);
}

CompiledMesh::~CompiledMesh()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	destroy_buffer(ebo);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	destroy_buffer(vbo);

	glBindVertexArray(0);
	destroy_vertex_array(vao);
}

Renderer::Renderer()
	: window_size{0, 0}
{
	glClearColor(0, 0, 0, 1.0f);

	StateChanger{&states}.cull_face(true);

	// todo(Gustav): move to states
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::render(const World& world, const Camera& camera)
{
	glViewport(0, 0, window_size.x, window_size.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	StateChanger{&states}.depth_test(true).blending(false);

	const auto compiled_camera = compile(camera, window_size);

	for (auto& m: world.meshes)
	{
		const auto translation = glm::translate(glm::mat4(1.0f), m->position);
		const auto rotation = glm::yawPitchRoll(m->rotation.x, m->rotation.y, m->rotation.z);
		const auto transform = translation * rotation;

		m->geom->material->shader.program->use();
		m->geom->material->set_uniforms(compiled_camera, transform);
		m->geom->material->bind_textures(&assets);
		m->geom->material->apply_lights(world.lights);

		glBindVertexArray(m->geom->vao);
		glDrawElements(GL_TRIANGLES, m->geom->number_of_triangles * 3, GL_UNSIGNED_INT, 0);
	}
}


}  //  namespace klotter
