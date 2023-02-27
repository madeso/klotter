#include "klotter/render/render.h"

#include <string_view>

#include "klotter/assert.h"
#include "klotter/render/opengl_utils.h"


using namespace std::literals::string_view_literals;

namespace klotter
{

template<typename T, typename TChange>
void apply_generic_state(std::optional<T>* current_state, std::optional<T> new_state, TChange change_function)
{
    if
    (
        new_state.has_value() == false
        ||
        (current_state->has_value() && **current_state == *new_state)
    )
    {
        return;
    }

    ASSERT(new_state.has_value());

    change_function(*new_state);
    *current_state = new_state;
}

void apply_state(std::optional<bool>* current_state, std::optional<bool> new_state, GLenum gl_type)
{
    apply_generic_state<bool>
    (
        current_state, new_state,
        [gl_type](bool enable)
        {
            if(enable)
            {
                glEnable(gl_type);
            }
            else
            {
                glDisable(gl_type);
            }
        }
    );
}


void apply(OpenglStates* current_states, const OpenglStates& new_states)
{
    #define APPLY_STATE(name, gl) apply_state(&current_states->name, new_states.name, gl)
    APPLY_STATE(cull_face, GL_CULL_FACE);
    APPLY_STATE(blending, GL_BLEND);
    APPLY_STATE(depth_test, GL_DEPTH_TEST);
    #undef APPLY_STATE

    apply_generic_state(&current_states->render_mode, new_states.render_mode, [](unsigned int rm)
    {
        glPolygonMode(GL_FRONT_AND_BACK, rm);
    });
}



void opengl_setup(OpenglStates* state)
{
    {
        OpenglStates new_states;
        new_states.cull_face = false;
        apply(state, new_states);
    }
    
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void opengl_set2d(OpenglStates* states)
{
    OpenglStates new_states;
    new_states.depth_test = false;
    new_states.blending = true;

    apply(states, new_states);
}


void opengl_set3d(OpenglStates* states)
{
    OpenglStates new_states;
    new_states.depth_test = true;
    new_states.blending = false;

    apply(states, new_states);
}


void opengl_set_render_mode_to_fill(OpenglStates* states)
{
    OpenglStates new_states;
    new_states.render_mode = GL_FILL;

    apply(states, new_states);
}


void opengl_set_render_mode_to_line(OpenglStates* states)
{
    OpenglStates new_states;
    new_states.render_mode = GL_LINE;

    apply(states, new_states);
}


void opengl_set_render_mode_to_point(OpenglStates* states)
{
    OpenglStates new_states;
    new_states.render_mode = GL_POINT;

    apply(states, new_states);
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

u32 create_vbo()
{
    u32 vbo;
    glGenBuffers(1, &vbo);
    return vbo;
}

void destroy_vbo(u32 vbo)
{
    glDeleteBuffers(1, &vbo);
}

u32 create_vao()
{
    u32 vao;
    glGenVertexArrays(1, &vao);
    return vao;
}

void destroy_vao(u32 vao)
{
    glDeleteVertexArrays(1, &vao);
}

u32 create_ebo()
{
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    return ebo;
}

void destroy_ebo(u32 ebo)
{
    glDeleteBuffers(1, &ebo);
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


ShaderResource::ShaderResource()
{
    assert(shader_resource() == nullptr);
    shader_resource() = this;

    shader = std::make_shared<ShaderProgram>(
    R"glsl(
        #version 330 core

        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec2 aTexCoord;

        uniform mat4 uTransform;

        out vec3 vColor;
        out vec2 vTexCoord;

        void main()
        {
            gl_Position = uTransform * vec4(aPos.xyz, 1.0);
            vColor = aColor;
            vTexCoord = aTexCoord;
        }
    )glsl"sv,
     R"glsl(
        #version 330 core

        uniform vec4 TintColor;

        in vec3 vColor;
        in vec2 vTexCoord;

        uniform sampler2D uTexture;

        out vec4 FragColor;

        void main()
        {
            FragColor = texture(uTexture, vTexCoord) * TintColor.rgba * vec4(vColor.rgb, 1.0);
        }
    )glsl"sv
    );
}
ShaderResource::~ShaderResource()
{
    shader_resource() = nullptr;
}


Material::Material(std::shared_ptr<ShaderProgram> s)
    : shader(s)
{
}


BasicMaterial::BasicMaterial()
    : Material(shaders().shader)
    , color(white, 1.0f)
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

void BasicMaterial::setUniforms(const glm::mat4& transform)
{
    shader->set_vec4(shader->get_uniform("TintColor"), color);
    shader->set_mat(shader->get_uniform("uTransform"), transform);
}

void BasicMaterial::bind_textures(Assets* assets)
{
    glActiveTexture(GL_TEXTURE0);
    std::shared_ptr<Texture> t = texture;
    if (t == nullptr) { t = assets->get_white(); }
    glBindTexture(GL_TEXTURE_2D, t->id);
}

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

CompiledMeshPtr compile_Mesh(const Mesh& mesh, MaterialPtr material)
{
    const auto indices = compile_indices(mesh);
    const auto vertices = material->compile_mesh_data(mesh);

    const auto vbo = create_vbo();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    shaders().shader->use();

    const auto vao = create_vao();
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

    const auto ebo = create_ebo();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Csizet_to_glsizeiptr(sizeof(u32) * indices.size()), indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    return std::make_shared<CompiledMesh>(vbo, vao, ebo, material, static_cast<i32>(mesh.faces.size()));
}

CompiledMesh::~CompiledMesh()
{
    destroy_ebo(ebo);
    destroy_vao(vao);
    destroy_vbo(vbo);
}

void CompiledMesh::render(Assets* assets, const glm::mat4& transform)
{
    material->shader->use();
    material->setUniforms(transform);
    material->bind_textures(assets);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, number_of_triangles * 3, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);
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
    opengl_setup(&states);
}

void Renderer::render(const Scene& scene, const Camera&)
{
    glViewport(0, 0, window_size.x, window_size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    opengl_set3d(&states);

    for (auto& m : scene.meshes)
    {
        const auto translation = glm::translate(glm::mat4(1.0f), m->position);
        const auto rotation = glm::yawPitchRoll(m->rotation.x, m->rotation.y, m->rotation.z);
        m->geom->render(&assets, translation * rotation);
    }
}


}
