#include "klotter/render/render.h"

#include <string_view>

#include "klotter/assert.h"
#include "klotter/render/opengl_utils.h"


using namespace std::literals::string_view_literals;

namespace klotter
{

template<typename T>
bool should_change(std::optional<T>* current_state, T new_state)
{
    // if there is a value, and that is the same... then don't update opengl
    if (*current_state && *current_state == new_state)
    { return false; }

    *current_state = new_state;
    return true;
}


void apply(std::optional<bool>* current_state, bool new_state, GLenum gl_type)
{
    if(should_change(current_state, new_state))
    {
        if(new_state)
        {
            glEnable(gl_type);
        }
        else
        {
            glDisable(gl_type);
        }
    }
}

struct States
{
    OpenglStates* states;

    explicit States(OpenglStates* s)
        : states(s)
    {
    }

    States& cull_face(bool new_state)
    {
        apply(&states->cull_face, new_state, GL_CULL_FACE);
        return *this;
    }

    States& blending(bool new_state)
    {
        apply(&states->blending, new_state, GL_BLEND);
        return *this;
    }

    States& depth_test(bool new_state)
    {
        apply(&states->depth_test, new_state, GL_DEPTH_TEST);
        return *this;
    }

    States& render_mode(RenderMode new_state)
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
    States{ states }
        .depth_test(false)
        .blending(true)
        ;
}


ShaderResource*& shader_resource()
{
    static ShaderResource* s = nullptr;
    return s;
}

ShaderResource& shaders()
{
    assert(shader_resource());
    return *shader_resource();
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

Vertex::Vertex(glm::vec3 p, glm::vec2 u, glm::vec3 c)
    : position(std::move(p))
    , uv(std::move(u))
    , color(std::move(c))
{
}

Mesh::Mesh(std::vector<Vertex> v, std::vector<Face> f)
    : vertices(std::move(v))
    , faces(std::move(f))
{
}

struct ShaderSource
{
    ShaderVertexAttributes layout;
    std::string_view vertex;
    std::string_view fragment;
};

using BaseShaderData = std::vector<VertexType>;

LoadedShaderData load_shader(const BaseShaderData& base_layout, const ShaderSource& source)
{
    auto layout_compiler = compile_attribute_layouts(base_layout, { source.layout });
    const auto mesh_layout = get_mesh_layout(layout_compiler);
    const auto compiled_layout = compile_shader_layout(layout_compiler, source.layout);

    auto program = std::make_shared<ShaderProgram>(source.vertex, source.fragment, compiled_layout);

    return { program, mesh_layout };
}

ShaderSource basic_shader_source();
ShaderSource light_shader_source();

ShaderResource::ShaderResource()
{
    assert(shader_resource() == nullptr);
    shader_resource() = this;

    auto global_shader_data = BaseShaderData{};
    basic_shader = load_shader(global_shader_data, basic_shader_source());
    light_shader = load_shader(global_shader_data, light_shader_source());
}

ShaderResource::~ShaderResource()
{
    shader_resource() = nullptr;
}

bool ShaderResource::is_loaded() const
{
    return
        basic_shader.program->is_loaded() &&
        light_shader.program->is_loaded()
        ;
}

Material::Material(LoadedShaderData s)
    : shader(std::move(s))
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderSource basic_shader_source() {
    return
    {
        {
            {VertexType::position3, "a_position"},
            {VertexType::color3, "a_color"},
            {VertexType::texture2, "a_tex_coord"}
        },
        R"glsl(
        #version 330 core

        in vec3 a_position;
        in vec3 a_color;
        in vec2 a_tex_coord;

        uniform mat4 u_projection;
        uniform mat4 u_view;
        uniform mat4 u_model;

        out vec3 v_color;
        out vec2 v_tex_coord;

        void main()
        {
            gl_Position = u_projection * u_view * u_model * vec4(a_position.xyz, 1.0);
            v_color = a_color;
            v_tex_coord = a_tex_coord;
        }
    )glsl"sv,
     R"glsl(
        #version 330 core

        uniform vec4 u_tint_color;
        uniform sampler2D u_tex_diffuse;

        in vec3 v_color;
        in vec2 v_tex_coord;

        out vec4 o_frag_color;

        void main()
        {
            o_frag_color = texture(u_tex_diffuse, v_tex_coord) * u_tint_color.rgba * vec4(v_color.rgb, 1.0);
        }
    )glsl"sv
    };
}

BasicMaterial::BasicMaterial()
    : Material(shaders().basic_shader)
    , color(colors::white)
    , alpha(1.0f)
{
}

std::vector<float> BasicMaterial::compile_mesh_data(const Mesh& mesh)
{
    std::vector<float> vertices;

    auto push3 = [&vertices](const glm::vec3& p)
    {
        vertices.emplace_back(p.x);
        vertices.emplace_back(p.y);
        vertices.emplace_back(p.z);
    };
    auto push2 = [&vertices](const glm::vec2& p)
    {
        vertices.emplace_back(p.x);
        vertices.emplace_back(p.y);
    };

    for (const auto& v : mesh.vertices)
    {
        push3(v.position);
        push3(v.color);
        push2(v.uv);
    }
    return vertices;
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
    if (t == nullptr) { t = assets->get_white(); }
    glBindTexture(GL_TEXTURE_2D, t->id);
}

void BasicMaterial::apply_lights(const Lights&)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ShaderSource light_shader_source()
{
    return 
    {
        {
            {VertexType::position3, "a_position"},
            {VertexType::color3, "a_color"},
            {VertexType::texture2, "a_tex_coord"}
        },
        R"glsl(
        #version 330 core

        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_color;
        layout (location = 2) in vec2 a_tex_coord;

        uniform mat4 u_projection;
        uniform mat4 u_view;
        uniform mat4 u_model;

        out vec3 v_color;
        out vec2 v_tex_coord;

        void main()
        {
            gl_Position = u_projection * u_view * u_model * vec4(a_position.xyz, 1.0);
            v_color = a_color;
            v_tex_coord = a_tex_coord;
        }
    )glsl"sv,
        R"glsl(
        #version 330 core

        uniform vec4 u_tint_color;
        uniform sampler2D u_tex_diffuse;
        uniform vec3 u_light_color;

        in vec3 v_color;
        in vec2 v_tex_coord;

        out vec4 o_frag_color;

        void main()
        {
            float ambient_strength = 0.1;
            vec3 ambient_color = ambient_strength * u_light_color;
            vec3 light_color = ambient_color;

            vec4 object_color = texture(u_tex_diffuse, v_tex_coord) * u_tint_color.rgba * vec4(v_color.rgb, 1.0);

            o_frag_color = vec4(light_color.rgb * object_color.rgb, object_color.a);
        }
    )glsl"sv
};
}

LightMaterial::LightMaterial()
    : Material(shaders().light_shader)
    , color(colors::white)
    , alpha(1.0f)
{
}

std::vector<float> LightMaterial::compile_mesh_data(const Mesh& mesh)
{
    std::vector<float> vertices;

    auto push3 = [&vertices](const glm::vec3& p)
    {
        vertices.emplace_back(p.x);
        vertices.emplace_back(p.y);
        vertices.emplace_back(p.z);
    };
    auto push2 = [&vertices](const glm::vec2& p)
    {
        vertices.emplace_back(p.x);
        vertices.emplace_back(p.y);
    };

    for (const auto& v : mesh.vertices)
    {
        push3(v.position);
        push3(v.color);
        push2(v.uv);
    }
    return vertices;
}

void LightMaterial::set_uniforms(const CompiledCamera& cc, const glm::mat4& transform)
{
    shader.program->set_vec4(shader.program->get_uniform("u_tint_color"), {color, alpha});
    shader.program->set_mat(shader.program->get_uniform("u_model"), transform);
    shader.program->set_mat(shader.program->get_uniform("u_projection"), cc.projection);
    shader.program->set_mat(shader.program->get_uniform("u_view"), cc.view);
}

void LightMaterial::bind_textures(Assets* assets)
{
    glActiveTexture(GL_TEXTURE0);
    std::shared_ptr<Texture> t = texture;
    if (t == nullptr) { t = assets->get_white(); }
    glBindTexture(GL_TEXTURE_2D, t->id);
}

void LightMaterial::apply_lights(const Lights& lights)
{
    shader.program->set_vec3(shader.program->get_uniform("u_light_color"), lights.point_light.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<u32> compile_indices(const Mesh& mesh)
{
    std::vector<u32> indices;
    const auto push = [&indices](const u32 i)
    {
        indices.emplace_back(i);
    };
    for (const auto& f : mesh.faces)
    {
        push(f.a); push(f.b); push(f.c);
    }
    return indices;
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

CompiledMeshPtr compile_Mesh(const Mesh& mesh, MaterialPtr material)
{
    const auto indices = compile_indices(mesh);
    const auto vertices = material->compile_mesh_data(mesh);

    material->shader.program->use();

    const auto vbo = create_buffer();
    const auto vao = create_vertex_array();
    glBindVertexArray(vao);

    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, Csizet_to_glsizeiptr(sizeof(float) * vertices.size()), vertices.data(), GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(float);
    const auto float_offset = [](std::size_t number_of_floats) -> void*
    {
        return reinterpret_cast<void*>(number_of_floats * sizeof(float));
    };
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, float_offset(0)); // next offset 3
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, float_offset(3)); // next offset 6
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, float_offset(6)); // next offset 8
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);


    const auto ebo = create_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Csizet_to_glsizeiptr(sizeof(u32) * indices.size()), indices.data(), GL_STATIC_DRAW);

    // todo(Gustav): disable binding
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // ebo
    // glBindVertexArray(0); // vao
    // glBindBuffer(GL_ARRAY_BUFFER, 0); // vbo


    return std::make_shared<CompiledMesh>(vbo, vao, ebo, material, static_cast<i32>(mesh.faces.size()));
}

void CompiledMesh::render(Assets* assets, const CompiledCamera& cc, const glm::mat4& transform, const Lights& lights)
{
    material->shader.program->use();
    material->set_uniforms(cc, transform);
    material->bind_textures(assets);
    material->apply_lights(lights);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, number_of_triangles * 3, GL_UNSIGNED_INT, 0);
}

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


Renderer::Renderer()
    : window_size{0, 0}
{
    glClearColor(0, 0, 0, 1.0f);

    States{ &states }.cull_face(true);

    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::render(const World& world, const Camera& camera)
{
    glViewport(0, 0, window_size.x, window_size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    States{ &states }
        .depth_test(true)
        .blending(false)
        ;

    const auto compiled_camera = compile(camera, window_size);

    for (auto& m : world.meshes)
    {
        const auto translation = glm::translate(glm::mat4(1.0f), m->position);
        const auto rotation = glm::yawPitchRoll(m->rotation.x, m->rotation.y, m->rotation.z);
        m->geom->render(&assets, compiled_camera, translation * rotation, world.lights);
    }
}


}
