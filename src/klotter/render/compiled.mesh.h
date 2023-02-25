#pragma once

#include <unordered_set>

#include "klotter/hash.string.h"

#include "klotter/render/handles.h"
#include "klotter/render/material.property.h"
#include "klotter/render/shader.h"
#include "klotter/render/uniform.h"
#include "klotter/render/vertex_layout.h"


#include "klotter/render/light.h"


namespace klotter
{



struct Attenuation;
struct Cache;
struct CommonData;
struct DirectionalLight;
struct Engine;
struct Geom;
struct MaterialDescription;
struct MaterialPropertyReference;
struct MeshInstance;
struct OpenglStates;
struct PointLight;
struct ShaderProgram;
struct SpotLight;
struct Uniform;
struct Vfs;


// "global" data on how many lights are supported
struct LightParams
{
    int number_of_directional_lights;
    int number_of_point_lights;
    int number_of_spot_lights;
};


struct ActiveLights
{
    std::vector<DirectionalLight> directional_lights;
    std::vector<PointLight> point_lights;
    std::vector<SpotLight> spot_lights;

    void clear();
};


struct LightStatus
{
    bool applied_directional_lights;
    bool applied_point_lights;
    bool applied_spot_lights;
};

constexpr LightStatus k_lightstatus_without_errors{true, true, true};


struct DirectionalLightUniforms
{
    Uniform direction;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;
};



struct AttenuationUniforms
{
    Uniform constant;
    Uniform linear;
    Uniform quadratic;
};


struct PointLightUniforms
{
    AttenuationUniforms attenuation;
    Uniform position;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;
};


struct SpotLightUniforms
{
    AttenuationUniforms attenuation;
    Uniform position;
    Uniform direction;
    Uniform cos_cutoff;
    Uniform cos_outer_cutoff;
    Uniform ambient;
    Uniform diffuse;
    Uniform specular;
};


struct LightUniforms
{
    std::vector<DirectionalLightUniforms> directional_lights;
    std::vector<PointLightUniforms> point_lights;
    std::vector<SpotLightUniforms> spot_lights;
};


template<typename T>
struct UniformIndexAnd
{
    int index;
    T value;
};


struct CompiledProperties
{
    std::vector<UniformIndexAnd<float>> floats;
    std::vector<UniformIndexAnd<glm::vec3>> vec3s;
    std::vector<UniformIndexAnd<glm::vec4>> vec4s;
    std::vector<UniformIndexAnd<TextureId>> textures;

    #define ADD_OP(FUNC_NAME, TYPE)\
    void FUNC_NAME(MaterialPropertyReference where, const TYPE& v)

    ADD_OP(set_float, float);
    ADD_OP(set_vec3, glm::vec3);
    ADD_OP(set_vec4, glm::vec4);
    ADD_OP(set_texture, TextureId);
    #undef ADD_OP
};


struct CommonData
{
    glm::vec3 camera_position;
    glm::mat4 pv; // projection * view
    glm::mat4 model;
};

struct CommonUniforms
{
    Uniform transform;
    Uniform model_transform;
    Uniform normal_matrix;
    Uniform view_position;
};

struct CompiledMaterialShader
{
    CompiledMaterialShader
    (
        ShaderProgram&& prog,
        CompiledGeomVertexAttributes layout,
        const std::string& a_debug_name
    );

    CompiledMaterialShader(CompiledMaterialShader&&) = default;
    CompiledMaterialShader& operator=(CompiledMaterialShader&&) = default;

    CompiledMaterialShader(const CompiledMaterialShader&) = delete;
    void operator=(const CompiledMaterialShader&) = delete;

    std::string debug_name;

    // acutal program
    ShaderProgram program;
    CompiledGeomVertexAttributes mesh_layout;

    // uniforms
    CommonUniforms common;
    std::optional<LightUniforms> light;
    std::vector<Uniform> uniforms;
    
    // stored properties
    std::unordered_map<HashedString, MaterialPropertyReference> name_to_array;
    CompiledProperties default_values;
};


struct CompiledMaterial
{
    CompiledMaterialShaderId shader;
    CompiledProperties properties;
    RenderMode render_mode;
};


struct CompiledGeom
{
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;
    int number_of_indices;
    std::unordered_set<VertexType> debug_shader_types;

    CompiledGeom
    (
        unsigned int a_vbo,
        unsigned int a_vao,
        unsigned int a_ebo,
        int count,
        const VertexTypes& st
    );
    ~CompiledGeom();

    CompiledGeom(CompiledGeom&& rhs);
    void operator=(CompiledGeom&&);

    CompiledGeom(const CompiledGeom&) = delete;
    void operator=(const CompiledGeom&) = delete;
};


struct CompiledMeshInstance
{
    CompiledGeom geom;
    CompiledMaterial material;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions


std::optional<CompiledMaterialShader>
load_material_shader
(
    const std::vector<VertexType>& global_layout,
    Cache* cache,
    const Vfs& vfs,
    const std::string& shader_name,
    const LightParams& light_params
);


void
use_material
(
    const CompiledMaterial& mat,
    OpenglStates* states,
    const Cache& cache,
    const CommonData& data,
    const ActiveLights& active_lights,
    LightStatus* ls
);

CompiledMaterial
compile_material
(
    Engine* engine,
    Cache* cache,
    const Vfs& vfs,
    const MaterialDescription& mat
);

CompiledMeshInstance
compile_mesh(Engine* engine, Cache* cache, const Vfs& vfs, const MeshInstance& mesh);

void
render_geom(const CompiledGeom& geom);

}
