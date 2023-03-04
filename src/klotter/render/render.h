#pragma once

#include <memory>

#include "klotter/colors.h"

#include "klotter/render/camera.h"
#include "klotter/render/shader.h"
#include "klotter/render/texture.h"
#include "klotter/render/assets.h"

namespace klotter
{

struct ShaderResource
{
    ShaderResource();
    ~ShaderResource();

    std::shared_ptr<ShaderProgram> basic_shader;
    std::shared_ptr<ShaderProgram> light_shader;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 color;

    explicit Vertex(glm::vec3 p, glm::vec2, glm::vec3 c = colors::white);
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

    virtual void set_uniforms(const CompiledCamera&, const glm::mat4&) = 0;
    virtual void bind_textures(Assets* assets) = 0;
};
using MaterialPtr = std::shared_ptr<Material>;


struct BasicMaterial : Material
{
    glm::vec4 color;
    std::shared_ptr<Texture> texture;

    BasicMaterial();
    std::vector<float> compile_mesh_data(const Mesh& mesh) override;
    void set_uniforms(const CompiledCamera&, const glm::mat4&) override;
    void bind_textures(Assets* assets) override;
};


struct LightMaterial : Material
{
    glm::vec4 color;
    std::shared_ptr<Texture> texture;

    LightMaterial();
    std::vector<float> compile_mesh_data(const Mesh& mesh) override;
    void set_uniforms(const CompiledCamera&, const glm::mat4&) override;
    void bind_textures(Assets* assets) override;
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

    void render(Assets*, const CompiledCamera&, const glm::mat4&);
};



using CompiledMeshPtr = std::shared_ptr<CompiledMesh>;
CompiledMeshPtr compile_Mesh(const Mesh&, MaterialPtr);



struct MeshInstance
{
    CompiledMeshPtr geom;

    glm::vec3 position;
    glm::vec3 rotation; // yaw pitch roll
};
using MeshInstancePtr = std::shared_ptr<MeshInstance>;
MeshInstancePtr make_MeshInstance(CompiledMeshPtr geom);
        

struct World
{
    std::vector<MeshInstancePtr> meshes;
};

enum class RenderMode {fill, line, point};

struct OpenglStates
{
    std::optional<bool> cull_face;
    std::optional<bool> blending;
    std::optional<bool> depth_test;
    std::optional<RenderMode> render_mode;
};

struct Renderer
{
    Renderer();

    ShaderResource shaders;
    OpenglStates states;
    Assets assets;
    glm::ivec2 window_size;

    void render(const World&, const Camera&);
};


}