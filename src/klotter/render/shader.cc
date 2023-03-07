#include "klotter/render/shader.h"

#include "klotter/assert.h"
#include "klotter/log.h"
#include "klotter/cint.h"

#include "klotter/render/opengl_utils.h"


namespace klotter
{

void set_shader_program(unsigned int new_program);

bool check_shader_compilation_error(const char* name, unsigned int shader)
{
    int  success = 0;
    char log[512] = {0,};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, log);
        LOG_ERROR("ERROR: %s shader compilation failed\n%s\n", name, log);
        return false;
    }

    return true;
}


bool check_shader_link_error(unsigned int program)
{
    int  success = 0;
    char log[512] = {0,};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        LOG_ERROR("ERROR: shader linking failed\n%s\n", log);
        return false;
    }

    return true;
}


void upload_shader_source(unsigned int shader, std::string_view source)
{
    const char* const s = &source[0];
    const int length = Csizet_to_int(source.length()); // should be GLint
    glShaderSource(shader, 1, &s, &length);
}


template<typename T>
void load_shader_source
(
    ShaderProgram* self,
    const T& vertex_source,
    const T& fragment_source
)
{
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    upload_shader_source(vertex_shader, vertex_source);
    glCompileShader(vertex_shader);
    const auto vertex_ok = check_shader_compilation_error("vertex", vertex_shader);

    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    upload_shader_source(fragment_shader, fragment_source);
    glCompileShader(fragment_shader);
    const auto fragment_ok = check_shader_compilation_error("fragment", fragment_shader);

    glAttachShader(self->shader_program, vertex_shader);
    glAttachShader(self->shader_program, fragment_shader);
    glLinkProgram(self->shader_program);
    const auto link_ok = check_shader_link_error(self->shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    clear_shader_program();

    if(vertex_ok && fragment_ok && link_ok)
    {
        // nop
    }
    else
    {
        clear_shader_program();
        glDeleteProgram(self->shader_program);
        self->shader_program = 0;
    }
}


ShaderProgram::ShaderProgram
(
    const std::string& vertex_source,
    const std::string& fragment_source
)
    : shader_program(glCreateProgram())
{
    load_shader_source(this, vertex_source, fragment_source);
}


ShaderProgram::ShaderProgram
(
    std::string_view vertex_source,
    std::string_view fragment_source
)
    : shader_program(glCreateProgram())
{
    load_shader_source(this, vertex_source, fragment_source);
}


void ShaderProgram::use() const
{
    set_shader_program(shader_program);
}


ShaderProgram::ShaderProgram(ShaderProgram&& rhs)
    : shader_program(rhs.shader_program)
{
    rhs.shader_program = 0;
}


void ShaderProgram::operator=(ShaderProgram&& rhs)
{
    clear();

    shader_program = rhs.shader_program;

    rhs.shader_program = 0;
}


ShaderProgram::~ShaderProgram()
{
    clear();
}


bool ShaderProgram::is_loaded() const
{
    return shader_program != 0;
}


void ShaderProgram::clear()
{
    clear_shader_program();
    glDeleteProgram(shader_program);
    shader_program = 0;
}


Uniform ShaderProgram::get_uniform(const std::string& name) const
{
    const auto uni = Uniform{name, glGetUniformLocation(shader_program, name.c_str()), shader_program};
    if(uni.is_valid() == false)
    {
        LOG_ERROR("Uniform %s not found", name.c_str());
    }
    return uni;
}


void ShaderProgram::set_float(const Uniform& uniform, float value) const
{
    ASSERT(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    ASSERT(uniform.debug_shader_program == shader_program);

    ASSERT(uniform.texture == -1 && "uniform is a texture not a float");
    glUniform1f(uniform.location, value);
}


void ShaderProgram::set_vec3(const Uniform& uniform, float x, float y, float z) const
{
    ASSERT(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    ASSERT(uniform.debug_shader_program == shader_program);

    ASSERT(uniform.texture == -1 && "uniform is a texture not a vec3");
    glUniform3f(uniform.location, x, y, z);
}


void ShaderProgram::set_vec3(const Uniform& uniform, const glm::vec3& v) const
{
    set_vec3(uniform, v.x, v.y, v.z);
}


void ShaderProgram::set_vec4(const Uniform& uniform, float x, float y, float z, float w) const
{
    ASSERT(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    ASSERT(uniform.debug_shader_program == shader_program);

    ASSERT(uniform.texture == -1 && "uniform is a texture not a vec4");
    glUniform4f(uniform.location, x, y, z, w);
}


void ShaderProgram::set_vec4(const Uniform& uniform, const glm::vec4& v) const
{
    set_vec4(uniform, v.x, v.y, v.z, v.w);
}


void ShaderProgram::set_texture(const Uniform& uniform) const
{
    ASSERT(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    ASSERT(uniform.debug_shader_program == shader_program);

    ASSERT(uniform.texture >= 0 && "uniform needs to be a texture");
    glUniform1i(uniform.location, uniform.texture);
}


void ShaderProgram::set_mat(const Uniform& uniform, const glm::mat4& mat) const
{
    ASSERT(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    ASSERT(uniform.debug_shader_program == shader_program);

    ASSERT(uniform.texture == -1 && "uniform is a texture not a matrix4");
    glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
}


void ShaderProgram::set_mat(const Uniform& uniform, const glm::mat3& mat) const
{
    ASSERT(is_shader_bound(shader_program));
    if(uniform.is_valid() == false) { return; }
    ASSERT(uniform.debug_shader_program == shader_program);

    ASSERT(uniform.texture == -1 && "uniform is a texture not a matrix3");
    glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(mat));
}


void setup_textures(ShaderProgram* shader, std::vector<Uniform*> uniform_list)
{
    // OpenGL should support atleast 16 textures
    ASSERT(uniform_list.size() <= 16);

    shader->use();

    int index = 0;
    for(const auto& uniform: uniform_list)
    {
        uniform->texture = index;
        index += 1;
        shader->set_texture(*uniform);
    }
}


namespace
{
    unsigned int debug_current_shader_program = 0;
}


void set_shader_program(unsigned int new_program)
{
    debug_current_shader_program = new_program;
    glUseProgram(new_program);
}


bool is_shader_bound(unsigned int program)
{
    return debug_current_shader_program == program;
}


void clear_shader_program()
{
    set_shader_program(0);
}




}
