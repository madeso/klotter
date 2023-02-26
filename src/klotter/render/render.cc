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

u32 create_vao()
{
    u32 vao;
    glGenVertexArrays(1, &vao);
    return vao;
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
        const auto push = [&vertices](const glm::vec3& v)
        {
            vertices.emplace_back(v.x);
            vertices.emplace_back(v.y);
            vertices.emplace_back(v.z);
        };
        for (const auto& f : mesh.faces)
        {
            push(f.a); push(f.b); push(f.c);
        }
        return vertices;
    }
};

MaterialPtr make_BasicMaterial()
{
    return std::make_shared<BasicMaterial>();
}

CompiledMeshPtr compile_Mesh(const Mesh& mesh, MaterialPtr material)
{
    const auto vertices = material->compile_mesh_data(mesh);
    const auto vbo = create_vbo();
    const auto vao = create_vao();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    shaders().shader->use();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return std::make_shared<CompiledMesh>(vbo, vao, material);
}

void CompiledMesh::render()
{
    material->shader->use();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

CompiledMesh::CompiledMesh(u32 b, u32 a, MaterialPtr m)
    : vbo(b)
    , vao(a)
    , material(m)
{
}

CompiledMesh::~CompiledMesh()
{
    // tdoo(Gustav): implement destructor
}

MeshInstancePtr make_MeshInstance(CompiledMeshPtr geom)
{
    auto mesh = std::make_shared<MeshInstance>();
    mesh->geom = geom;
    return mesh;
}


// move to member function of app to access current window size
void render(const Scene& scene, const Camera&)
{
    for (auto& m : scene.meshes)
    {
        m->geom->render();
    }
}


}