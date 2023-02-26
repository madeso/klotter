#pragma once

#include <memory>

#include "klotter/render/camera.h"
#include "klotter/render/shader.h"

#include "klotter/types.h"
#include "klotter/colors.h"

namespace klotter
{

struct ShaderResource
{
    ShaderResource();
    ~ShaderResource();
    std::shared_ptr<ShaderProgram> shader;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;

    explicit Vertex(glm::vec3 p, glm::vec3 c = white);
};

struct Face
{
    u32 a;
    u32 b;
    u32 c;
};

struct Mesh
{
    Mesh(std::vector<Vertex>, std::vector<Face> faces);

    std::vector<Vertex> vertices;
    std::vector<Face> faces;
};



struct Material
{
    std::shared_ptr<ShaderProgram> shader;

    Material(std::shared_ptr<ShaderProgram>);
    virtual ~Material() = default;

    Material(const Material&) = delete;
    Material(Material&&) = delete;
    void operator=(const Material&) = delete;
    void operator=(Material&&) = delete;

    virtual std::vector<float> compile_mesh_data(const Mesh& mesh) = 0;

    virtual void setUniforms() = 0;
};
using MaterialPtr = std::shared_ptr<Material>;


struct BasicMaterial : Material
{
    glm::vec4 color;

    BasicMaterial();
    std::vector<float> compile_mesh_data(const Mesh& mesh) override;
    void setUniforms() override;
};



struct CompiledMesh
{
    u32 vbo;
    u32 vao;
    u32 ebo;
    MaterialPtr material;
    i32 number_of_triangles;

    explicit CompiledMesh(u32, u32, u32, MaterialPtr, i32);
    ~CompiledMesh();

    CompiledMesh(const CompiledMesh&) = delete;
    CompiledMesh(CompiledMesh&&) = delete;
    void operator=(const CompiledMesh&) = delete;
    void operator=(CompiledMesh&&) = delete;

    void render();
};



using CompiledMeshPtr = std::shared_ptr<CompiledMesh>;
CompiledMeshPtr compile_Mesh(const Mesh& mesh, MaterialPtr material);



struct MeshInstance
{
    CompiledMeshPtr geom;

    glm::vec3 position;
    glm::quat rotation;
};
using MeshInstancePtr = std::shared_ptr<MeshInstance>;
MeshInstancePtr make_MeshInstance(CompiledMeshPtr geom);
        

struct Scene
{
    std::vector<MeshInstancePtr> meshes;
};


void render(const Scene& scene, const Camera& camera);



}