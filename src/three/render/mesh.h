#pragma once

#include "three/hash.string.h"
#include "three/render/material.property.h"


namespace render
{


struct MaterialDescription
{
    std::string shader;

    explicit MaterialDescription(const std::string& shader_path);

    // custom properties
    // example: the "diffuse" property for all "box" meshes is "dirt.png"
    std::unordered_map<HashedString, MaterialPropertyReference> name_to_array;
    std::vector<float> floats;
    std::vector<glm::vec3> vec3s;
    std::vector<glm::vec4> vec4s;
    std::vector<std::string> textures;
    RenderMode render_mode = RenderMode::fill;

    #define ADD_OP(FUNC_NAME, TYPE)\
    MaterialDescription& FUNC_NAME(const HashedString& name, const TYPE& v)
    ADD_OP(with_float, float);
    ADD_OP(with_vec3, glm::vec3);
    ADD_OP(with_vec4, glm::vec4);
    ADD_OP(with_texture, std::string);
    #undef ADD_OP

    MaterialDescription& with_render_mode(RenderMode rm);
};


/** A unique combination of position/normal/texturecoord/etc. in a Geom.
 */
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture;
    glm::vec4 color;

    Vertex
    (
        const glm::vec3& p,
        const glm::vec3& n,
        const glm::vec2& t,
        const glm::vec4& c = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}
    );
};

using Triangle = glm::ivec3;

struct Geom
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> triangles; // %3 == 0

    Geom(const std::vector<Vertex>& verts, const std::vector<unsigned int>& tris);
};


struct Mesh
{
    MaterialDescription material;
    Geom geom;
};


Geom
create_plane_mesh();


Geom
create_plane_mesh(float size, float uv);


Geom
create_box_mesh(float size);


Geom
create_box_mesh(float x, float y, float z);


}

