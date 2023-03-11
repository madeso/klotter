#pragma once

#include <memory>

#include "klotter/colors.h"

#include "klotter/render/camera.h"
#include "klotter/render/shader.h"
#include "klotter/render/texture.h"
#include "klotter/render/assets.h"
#include "klotter/render/vertex_layout.h"

namespace klotter
{

struct Lights;

struct LoadedShaderData
{
    std::shared_ptr<ShaderProgram> program;
    CompiledGeomVertexAttributes mesh_layout;
};


struct ShaderResource
{
    ShaderResource();
    ~ShaderResource();

    LoadedShaderData basic_shader;
    LoadedShaderData light_shader;

    bool is_loaded() const;
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
    LoadedShaderData shader;

    explicit Material(LoadedShaderData data);
    virtual ~Material() = default;

    Material(const Material&) = delete;
    Material(Material&&) = delete;
    void operator=(const Material&) = delete;
    void operator=(Material&&) = delete;

    virtual std::vector<float> compile_mesh_data(const Mesh& mesh) = 0;

    virtual void set_uniforms(const CompiledCamera&, const glm::mat4&) = 0;
    virtual void bind_textures(Assets* assets) = 0;
    virtual void apply_lights(const Lights& lights) = 0;
};
using MaterialPtr = std::shared_ptr<Material>;


struct BasicMaterial : Material
{
    glm::vec3 color;
    float alpha;
    std::shared_ptr<Texture> texture;

    BasicMaterial();
    std::vector<float> compile_mesh_data(const Mesh& mesh) override;
    void set_uniforms(const CompiledCamera&, const glm::mat4&) override;
    void bind_textures(Assets* assets) override;
    void apply_lights(const Lights& lights) override;
};


struct LightMaterial : Material
{
    glm::vec3 color;
    float alpha;
    std::shared_ptr<Texture> texture;

    LightMaterial();
    std::vector<float> compile_mesh_data(const Mesh& mesh) override;
    void set_uniforms(const CompiledCamera&, const glm::mat4&) override;
    void bind_textures(Assets* assets) override;
    void apply_lights(const Lights& lights) override;
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

    void render(Assets*, const CompiledCamera&, const glm::mat4&, const Lights&);
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

struct PointLight
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 color = colors::white;
};

struct Lights
{
    PointLight point_light;
};

struct World
{
    std::vector<MeshInstancePtr> meshes;
    Lights lights;
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