#include "klotter/render/render.h"

#include <string_view>

#include "klotter/dependency_opengl.h"


using namespace std::literals::string_view_literals;

namespace klotter
{

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

        out vec3 vColor;
        out vec2 vTexCoord;

        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
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

void BasicMaterial::setUniforms()
{
    const auto uni = shader->get_uniform("TintColor");
    shader->set_vec4(uni, color);
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
    const auto float_offset = [](int f) -> void*
    {
        return reinterpret_cast<void*>(f * sizeof(float));
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

void CompiledMesh::render(Assets* assets)
{
    material->shader->use();
    material->setUniforms();
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


void Renderer::render(const Scene& scene, const Camera&)
{
    for (auto& m : scene.meshes)
    {
        m->geom->render(&assets);
    }
}


}
