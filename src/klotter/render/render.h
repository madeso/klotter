#pragma once

#include <memory>

#include "klotter/render/camera.h"
#include "klotter/render/shader.h"
#include "klotter/types.h"

namespace klotter
{

struct ShaderResource
{
    ShaderResource();
    ~ShaderResource();
    std::shared_ptr<ShaderProgram> shader;
};

struct Face
{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
};

struct Mesh
{
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
};
using MaterialPtr = std::shared_ptr<Material>;
MaterialPtr make_BasicMaterial();



struct CompiledMesh
{
    u32 vbo;
    u32 vao;
    MaterialPtr material;

    explicit CompiledMesh(u32, u32, MaterialPtr);
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