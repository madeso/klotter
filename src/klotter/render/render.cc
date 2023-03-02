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

        uniform mat4 uProjection;
        uniform mat4 uView;
        uniform mat4 uModel;

        out vec3 vColor;
        out vec2 vTexCoord;

        void main()
        {
            gl_Position = uProjection * uView * uModel * vec4(aPos.xyz, 1.0);
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
    , color(colors::white, 1.0f)
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

void BasicMaterial::setUniforms(const CompiledCamera& cc, const glm::mat4& transform)
{
    shader->set_vec4(shader->get_uniform("TintColor"), color);
    shader->set_mat(shader->get_uniform("uModel"), transform);
    shader->set_mat(shader->get_uniform("uProjection"), cc.projection);
    shader->set_mat(shader->get_uniform("uView"), cc.view);
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

    shaders().shader->use();

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

void CompiledMesh::render(Assets* assets, const CompiledCamera& cc, const glm::mat4& transform)
{
    material->shader->use();
    material->setUniforms(cc, transform);
    material->bind_textures(assets);

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

void Renderer::render(const Scene& scene, const Camera& camera)
{
    glViewport(0, 0, window_size.x, window_size.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    States{ &states }
        .depth_test(true)
        .blending(false)
        ;

    const auto compiled_camera = compile(camera, window_size);

    for (auto& m : scene.meshes)
    {
        const auto translation = glm::translate(glm::mat4(1.0f), m->position);
        const auto rotation = glm::yawPitchRoll(m->rotation.x, m->rotation.y, m->rotation.z);
        m->geom->render(&assets, compiled_camera, translation * rotation);
    }
}


}
