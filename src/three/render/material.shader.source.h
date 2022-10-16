#pragma once

#include "three/hash.string.h"
#include "three/render/shader.source.h"
#include "three/render/material.property.h"

namespace render
{

struct Vfs;


struct MaterialSourceProperty
{
    HashedString display_name; // used in ui and material references, example: "diffuse texture"
    std::string shader_uniform_ident; // glsl variable identifier, example: "uDiffuse"
    MaterialPropertyReference index;
};

struct MaterialShaderSource
{
    // todo(Gustav): merge material shader source with custom frontmatter parsing so both shader and material shader can use the same frontmatter as base
    shader::ShaderSource source;
    bool apply_lightning;

    // todo(Gustav): specify sort order
    // todo(Gustav): specify blend mode
    // todo(Gustav): stencil test/write action

    std::vector<MaterialSourceProperty> properties;

    MaterialShaderSource(const shader::ShaderSource& src, bool al);

    static MaterialShaderSource create_with_lights(const shader::ShaderSource& src);
    static MaterialShaderSource create_unlit(const shader::ShaderSource& src);

    // default values
    // example: this material exposes "diffuse" and unless specified it is white
    std::vector<float> floats;
    std::vector<glm::vec3> vec3s;
    std::vector<glm::vec4> vec4s;
    std::vector<std::string> textures;

    #define ADD_OP(FUNC_NAME, TYPE)\
    MaterialShaderSource& FUNC_NAME(const HashedString& name, const std::string& uniform_name, const TYPE& v)
    ADD_OP(with_float, float);
    ADD_OP(with_vec3, glm::vec3);
    ADD_OP(with_vec4, glm::vec4);
    ADD_OP(with_texture, std::string);
    #undef ADD_OP
};

std::optional<MaterialShaderSource> load_material_shader_source(const Vfs& vfs, const std::string& path);


}
