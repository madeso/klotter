#include "three/render/material.shader.source.h"

#include "three/cint.h"
#include "three/render/vfs.h"


namespace render
{

MaterialShaderSource::MaterialShaderSource(const shader::ShaderSource& src, bool al)
    : source(src)
    , apply_lightning(al)
{
}

MaterialShaderSource MaterialShaderSource::create_with_lights(const shader::ShaderSource& src) { return {src, true}; }
MaterialShaderSource MaterialShaderSource::create_unlit(const shader::ShaderSource& src) { return {src, false}; }



#define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
MaterialShaderSource& MaterialShaderSource::FUNC_NAME(const HashedString& name, const std::string& uniform_name, const TYPE& v)\
{\
    const auto index = Csizet_to_int(MEMBER.size());\
    MEMBER.emplace_back(v);\
    properties.emplace_back(MaterialSourceProperty{name, uniform_name, {ENUM, index}});\
    return *this;\
}
ADD_OP(with_float, floats, float, MaterialPropertyType::float_type)
ADD_OP(with_vec3, vec3s, glm::vec3, MaterialPropertyType::vec3_type)
ADD_OP(with_vec4, vec4s, glm::vec4, MaterialPropertyType::vec4_type)
ADD_OP(with_texture, textures, std::string, MaterialPropertyType::texture_type)
#undef ADD_OP



std::optional<MaterialShaderSource> load_material_shader_source(const Vfs& vfs, const std::string& path)
{
    // todo(Gustav): replace with actual vfs call?
    return vfs.load_material_shader_source(path);
}



}
