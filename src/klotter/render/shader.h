#pragma once

#include "klotter/render/uniform.h"

#include <unordered_set>


namespace klotter
{

struct ShaderProgram
{
    ShaderProgram
    (
        const std::string& vertex_source,
        const std::string& fragment_source
    );
    ShaderProgram
    (
        std::string_view vertex_source,
        std::string_view fragment_source
    );

    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    void operator=(const ShaderProgram&) = delete;

    ShaderProgram(ShaderProgram&&);
    void operator=(ShaderProgram&&);

    void clear();
    bool is_loaded() const;

    void
    use() const;

    Uniform
    get_uniform(const std::string& name) const;

    // shader needs to be bound
    void
    set_float(const Uniform& uniform, float value) const;

    void
    set_vec3(const Uniform& uniform, float x, float y, float z) const;

    void
    set_vec3(const Uniform& uniform, const glm::vec3& v) const;

    void
    set_vec4(const Uniform& uniform, float x, float y, float z, float w) const;

    void
    set_vec4(const Uniform& uniform, const glm::vec4& v) const;

    void
    set_texture(const Uniform& uniform) const;

    void
    set_mat(const Uniform& uniform, const glm::mat4& mat) const;

    void
    set_mat(const Uniform& uniform, const glm::mat3& mat) const;


    unsigned int shader_program;
};


void
setup_textures(ShaderProgram* shader, std::vector<Uniform*> uniform_list);

bool
is_shader_bound(unsigned int program);

void
clear_shader_program();

}
