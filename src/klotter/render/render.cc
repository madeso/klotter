#include "klotter/render/render.h"

#include <string_view>

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
    layout(location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
    )glsl"sv,
        R"glsl(
        #version 330 core
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
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


struct BasicMaterial : Material
{
    BasicMaterial()
        : Material(shaders().shader)
    {
    }

    std::vector<float> compile_mesh_data(const Mesh& mesh)
    {
        std::vector<float> vertices;
        for (const auto& vv : mesh.vertices)
        {
            const auto& p = vv.position;
            vertices.emplace_back(p.x);
            vertices.emplace_back(p.y);
            vertices.emplace_back(p.z);
        }
        return vertices;
    }
};

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

MaterialPtr make_BasicMaterial()
{
    return std::make_shared<BasicMaterial>();
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    const auto ebo = create_ebo();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size(), indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    return std::make_shared<CompiledMesh>(vbo, vao, ebo, material, static_cast<i32>(mesh.faces.size()));
}

CompiledMesh::~CompiledMesh()
{
    destroy_ebo(ebo);
    destroy_vao(vao);
    destroy_vbo(vbo);
}

void CompiledMesh::render()
{
    material->shader->use();
    
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


void render(const Scene& scene, const Camera&)
{
    for (auto& m : scene.meshes)
    {
        m->geom->render();
    }
}


}