#include "klotter/render/compiled.mesh.h"

#include <functional>
#include <numeric>
#include <cmath>

#include "klotter/assert.h"
#include "klotter/dependency_opengl.h"
#include "klotter/render/material.property.h"
#include "klotter/render/material.shader.source.h"
#include "klotter/render/mesh.h"
#include "klotter/render/shader.h"
#include "klotter/render/texture.h"
#include "klotter/render/opengl_utils.h"
#include "klotter/str.h"


namespace klotter
{


void ActiveLights::clear()
{
    directional_lights.clear();
    point_lights.clear();
    spot_lights.clear();
}


void
set_attenuation_uniforms_in_shader(const AttenuationUniforms& uniforms, const ShaderProgram& shader, const Attenuation& att)
{
    shader.set_float(uniforms.constant, att.constant);
    shader.set_float(uniforms.linear, att.linear);
    shader.set_float(uniforms.quadratic, att.quadratic);
}

void
set_directional_light_uniforms_in_shader(const DirectionalLightUniforms& uniforms, const ShaderProgram& shader, const DirectionalLight& light)
{
    shader.set_vec3(uniforms.direction, light.direction);
    shader.set_vec3(uniforms.ambient, light.ambient * light.ambient_strength);
    shader.set_vec3(uniforms.diffuse, light.diffuse);
    shader.set_vec3(uniforms.specular, light.specular);
}

void
set_point_light_uniforms_in_shader(const PointLightUniforms& uniforms, const ShaderProgram& shader, const PointLight& light)
{
    set_attenuation_uniforms_in_shader(uniforms.attenuation, shader, light.attenuation);
    shader.set_vec3(uniforms.position, light.position);
    shader.set_vec3(uniforms.ambient, light.ambient * light.ambient_strength);
    shader.set_vec3(uniforms.diffuse, light.diffuse);
    shader.set_vec3(uniforms.specular, light.specular);
}

void
set_spot_light_uniforms_in_shader(const SpotLightUniforms& uniforms, const ShaderProgram& shader, const SpotLight& light)
{
    set_attenuation_uniforms_in_shader(uniforms.attenuation, shader, light.attenuation);
    shader.set_vec3(uniforms.position, light.position);
    shader.set_vec3(uniforms.direction, light.direction);
    shader.set_float(uniforms.cos_cutoff, std::cos(glm::radians(light.cutoff)));
    shader.set_float(uniforms.cos_outer_cutoff, std::cos(glm::radians(light.outer_cutoff)));
    shader.set_vec3(uniforms.ambient, light.ambient * light.ambient_strength);
    shader.set_vec3(uniforms.diffuse, light.diffuse);
    shader.set_vec3(uniforms.specular, light.specular);
}


constexpr Attenuation k_zero_attenuation = {0.0f, 0.0f, 0.0f};

constexpr DirectionalLight k_no_directional_light =
{
    // direction
    glm::vec3{0.0f, 1.0f, 0.0f},

    //ambient_strength
    0.0f,

    // ambient
    black3,
    // diffuse
    black3,
    // specular
    black3
};

constexpr PointLight k_no_point_light =
{
    k_zero_attenuation,

    // position
    glm::vec3{0.0f, 0.0f, 0.0f},

    // ambient_strength
    0.0f,

    // ambient
    black3,
    // diffuse
    black3,
    // specular
    black3
};


constexpr SpotLight k_no_spot_light =
{
    k_zero_attenuation,

    // position
    glm::vec3{0.0f, 0.0f, 0.0f},
    // direction
    glm::vec3{1.0f, 0.0f, 0.0f},

    // cutoff
    0.0f,
    // outer_cutoff
    0.0f,
    // ambient_strength
    0.0f,

    // ambient
    black3,
    // diffuse
    black3,
    // specular
    black3
};


void turn_on_light(const DirectionalLightUniforms& uniforms, const ShaderProgram& shader, const DirectionalLight& light)
{
    set_directional_light_uniforms_in_shader(uniforms, shader, light);
}


void turn_off_light(const DirectionalLightUniforms& uniforms, const ShaderProgram& shader)
{
    set_directional_light_uniforms_in_shader(uniforms, shader, k_no_directional_light);
}


void
turn_on_light(const PointLightUniforms& uniforms, const ShaderProgram& shader, const PointLight& light)
{
    set_point_light_uniforms_in_shader(uniforms, shader, light);
}


void
turn_off_light(const PointLightUniforms& uniforms, const ShaderProgram& shader)
{
    set_point_light_uniforms_in_shader(uniforms, shader, k_no_point_light);
}


void
turn_on_light(const SpotLightUniforms& uniforms, const ShaderProgram& shader, const SpotLight& light)
{
    set_spot_light_uniforms_in_shader(uniforms, shader, light);
}

void
turn_off_light(const SpotLightUniforms& uniforms, const ShaderProgram& shader)
{
    set_spot_light_uniforms_in_shader(uniforms, shader, k_no_spot_light);
}


DirectionalLightUniforms
get_directional_light_uniforms_from_shader
(
    const ShaderProgram& shader,
    const std::string& base_name
)
{
    DirectionalLightUniforms r;

    r.direction = shader.get_uniform(base_name + ".normalized_direction");
    r.ambient = shader.get_uniform(base_name + ".ambient");
    r.diffuse = shader.get_uniform(base_name + ".diffuse");
    r.specular = shader.get_uniform(base_name + ".specular");

    return r;
}


AttenuationUniforms
get_attenuation_uniforms_from_shader
(
    const ShaderProgram& shader,
    const std::string& base_name
)
{
    AttenuationUniforms r;

    r.constant = shader.get_uniform(base_name + ".constant");
    r.linear = shader.get_uniform(base_name + ".linear");
    r.quadratic = shader.get_uniform(base_name + ".quadratic");

    return r;
}



PointLightUniforms
get_point_light_uniforms_from_shader
(
    const ShaderProgram& shader,
    const std::string& base_name
)
{
    PointLightUniforms r;

    r.attenuation = get_attenuation_uniforms_from_shader(shader, base_name + ".attenuation");
    r.position = shader.get_uniform(base_name + ".position");
    r.ambient = shader.get_uniform(base_name + ".ambient");
    r.diffuse = shader.get_uniform(base_name + ".diffuse");
    r.specular = shader.get_uniform(base_name + ".specular");

    return r;
}



SpotLightUniforms
get_spot_light_uniforms_from_shader
(
    const ShaderProgram& shader,
    const std::string& base_name
)
{
    SpotLightUniforms r;
    
    r.attenuation = get_attenuation_uniforms_from_shader(shader, base_name + ".attenuation");
    r.position = shader.get_uniform(base_name + ".position");
    r.direction = shader.get_uniform(base_name + ".direction");
    r.cos_cutoff = shader.get_uniform(base_name + ".cos_cutoff");
    r.cos_outer_cutoff = shader.get_uniform(base_name + ".cos_outer_cutoff");
    r.ambient = shader.get_uniform(base_name + ".ambient");
    r.diffuse = shader.get_uniform(base_name + ".diffuse");
    r.specular = shader.get_uniform(base_name + ".specular");

    return r;
}





// returns false if there were too many lights in the scene
template<typename TUniform, typename TData>
void apply_data(const ShaderProgram& shader, const std::vector<TData>& src, const std::vector<TUniform>& dst, bool* applied)
{
    std::size_t index = 0;

    for(const TData& data: src)
    {
        if(index>= dst.size()) { *applied = false; return; }
        turn_on_light(dst[index], shader, data);
        index += 1;
    }

    for(;index<dst.size(); index+=1)
    {
        turn_off_light(dst[index], shader);
    }
}


LightUniforms
get_light_uniforms_from_shader
(
    const ShaderProgram& shader,
    const LightParams& lp
)
{
    LightUniforms r;

    assert(lp.number_of_directional_lights <= 1);
    if(lp.number_of_directional_lights == 1)
    {
        r.directional_lights.emplace_back(get_directional_light_uniforms_from_shader(shader, "uDirectionalLight"));
    }

    assert(lp.number_of_spot_lights <= 1);
    if(lp.number_of_spot_lights == 1)
    {
        r.spot_lights.emplace_back(get_spot_light_uniforms_from_shader(shader, "uSpotLight"));
    }

    for(int point_light_index=0; point_light_index<lp.number_of_point_lights; point_light_index += 1)
    {
        const std::string uniform_name = Str{} << "uPointLights[" << point_light_index << "]";
        r.point_lights.emplace_back(get_point_light_uniforms_from_shader(shader, uniform_name));
    }

    return r;
}

void
set_light_uniforms_in_shader
(
    const LightUniforms& uniforms,
    const ShaderProgram& prog,
    const ActiveLights& data,
    LightStatus* ls
)
{
    apply_data(prog, data.directional_lights, uniforms.directional_lights, &ls->applied_directional_lights);
    apply_data(prog, data.point_lights, uniforms.point_lights, &ls->applied_point_lights);
    apply_data(prog, data.spot_lights, uniforms.spot_lights, &ls->applied_spot_lights);
}


#define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
void CompiledProperties::FUNC_NAME(MaterialPropertyReference where, const TYPE& v)\
{\
    assert(where.type == ENUM);\
    MEMBER[Cint_to_sizet(where.index)].value = v;\
}
ADD_OP(set_float, floats, float, MaterialPropertyType::float_type)
ADD_OP(set_vec3, vec3s, glm::vec3, MaterialPropertyType::vec3_type)
ADD_OP(set_vec4, vec4s, glm::vec4, MaterialPropertyType::vec4_type)
ADD_OP(set_texture, textures, TextureId, MaterialPropertyType::texture_type)
#undef ADD_OP



CommonUniforms get_common_uniforms_from_shader(const ShaderProgram& shader)
{
    return
    {
        shader.get_uniform("uTransform"),
        shader.get_uniform("uModelTransform"),
        shader.get_uniform("uNormalMatrix"),
        shader.get_uniform("uViewPosition")
    };
}




CompiledMaterialShader::CompiledMaterialShader
(
    ShaderProgram&& prog,
    CompiledGeomVertexAttributes layout,
    const std::string& a_debug_name
)
    : debug_name(a_debug_name)
    , program(std::move(prog))
    , mesh_layout(layout)
    , common(get_common_uniforms_from_shader(program))
{
}


template<typename T> UniformIndexAnd<T> make_uniform_and(int index, T val)
{
    return UniformIndexAnd<T>{index, val};
}


std::optional<CompiledMaterialShader>
load_material_shader
(
    const std::vector<VertexType>& global_layout,
    Cache* cache,
    const Vfs& vfs,
    const std::string& shader_name,
    const LightParams& light_params
)
{
    auto loaded_shader_source = load_material_shader_source(vfs, shader_name);
    if(!loaded_shader_source) { return std::nullopt; }

    const MaterialShaderSource& shader_source = std::move(*loaded_shader_source);

    auto layout_compiler = compile_attribute_layouts(global_layout, {shader_source.source.layout});
    const auto mesh_layout = get_mesh_layout(layout_compiler);
    const auto compiled_layout = compile_shader_layout(layout_compiler, shader_source.source.layout);

    auto compile_defines = template_engine::Properties{};
    if(shader_source.apply_lightning)
    {
        compile_defines.insert({"NUMBER_OF_DIRECTIONAL_LIGHTS", std::to_string(light_params.number_of_directional_lights)});
        compile_defines.insert({"NUMBER_OF_POINT_LIGHTS", std::to_string(light_params.number_of_point_lights)});
        compile_defines.insert({"NUMBER_OF_SPOT_LIGHTS", std::to_string(light_params.number_of_spot_lights)});
    }
    
    auto ret = CompiledMaterialShader
    {
        {
            template_engine::generate(shader_source.source.vertex, compile_defines),
            template_engine::generate(shader_source.source.fragment, compile_defines),
            compiled_layout
        },
        mesh_layout,
        shader_name
    };
    
    auto texture_uniform_indices = std::vector<std::size_t>{};
    std::map<MaterialPropertyReference, int> array_to_uniform;

    for(const auto& prop: shader_source.properties)
    {
        if(prop.index.type == MaterialPropertyType::texture_type)
        {
            texture_uniform_indices.emplace_back(ret.uniforms.size());
        }
        const auto uniform_index = ret.uniforms.size();
        ret.uniforms.emplace_back(ret.program.get_uniform(prop.shader_uniform_ident));
        ret.name_to_array.insert({prop.display_name, prop.index});
        array_to_uniform.insert({prop.index, Csizet_to_int(uniform_index)});
    }

    // copy default values
    {
        auto copy = [&](auto* def_prop, const auto& source_prop, MaterialPropertyType propi)
        {
            int index = 0;
            for(const auto& d: source_prop)
            {
                auto found = array_to_uniform.find({propi, index});
                ASSERT(found != array_to_uniform.end());
                def_prop->emplace_back(make_uniform_and(found->second, d));
                index+=1;
            }
        };

        copy(&ret.default_values.floats, shader_source.floats, MaterialPropertyType::float_type);
        copy(&ret.default_values.vec3s, shader_source.vec3s, MaterialPropertyType::vec3_type);
        copy(&ret.default_values.vec4s, shader_source.vec4s, MaterialPropertyType::vec4_type);
    }

    // load textures
    {
        int index = 0;
        for(const auto& path: shader_source.textures)
        {
            auto found = array_to_uniform.find({MaterialPropertyType::texture_type, index});
            ASSERT(found != array_to_uniform.end()); 
            ret.default_values.textures.emplace_back
            (
                make_uniform_and(found->second, load_texture(cache, vfs, path))
            );
            index += 1;
        }
    }
    {
        auto textures = std::vector<Uniform*>{};
        for(auto index: texture_uniform_indices)
        {
            textures.emplace_back(&ret.uniforms[index]);
        }
        setup_textures(&ret.program, textures);
    }

    if(shader_source.apply_lightning)
    {
        ret.light = get_light_uniforms_from_shader(ret.program, light_params);
    }
    return ret;
}


void render_geom(const CompiledGeom& geom)
{
    ASSERT(is_bound_for_shader(geom.debug_shader_types));
    glBindVertexArray(geom.vao);
    glDrawElements(GL_TRIANGLES, geom.number_of_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void set_render_mode(OpenglStates* states, RenderMode render_mode)
{
    switch(render_mode)
    {
    case RenderMode::fill:  opengl_set_render_mode_to_fill(states); break;
    case RenderMode::line:  opengl_set_render_mode_to_line(states); break;
    case RenderMode::point: opengl_set_render_mode_to_point(states); break;
    default:
        DIE("unhandled render_mode");
        break;
    }
}


void use_material
(
    const CompiledMaterial& mat,
    OpenglStates* states,
    const Cache& cache,
    const CommonData& data,
    const ActiveLights& active_lights,
    LightStatus* ls
)
{
    set_render_mode(states, mat.render_mode);

    const auto& shader = get_compiled_material_shader(cache, mat.shader);
    
    shader.program.use();
    shader.program.set_vec3(shader.common.view_position, data.camera_position);
    shader.program.set_mat(shader.common.transform, data.pv * data.model);
    shader.program.set_mat(shader.common.model_transform, data.model);

    {
        const auto m = glm::mat3(glm::transpose(glm::inverse(data.model)));
        shader.program.set_mat(shader.common.normal_matrix, m);
    }

    if(shader.light)
    {
        set_light_uniforms_in_shader(*shader.light, shader.program, active_lights, ls);
    }

    // set material uniforms in shader
    for(const auto& d: mat.properties.floats)
    {
        shader.program.set_float(shader.uniforms[Cint_to_sizet(d.index)], d.value);
    }
    for(const auto& d: mat.properties.vec3s)
    {
        shader.program.set_vec3(shader.uniforms[Cint_to_sizet(d.index)], d.value);
    }
    for(const auto& d: mat.properties.vec4s)
    {
        shader.program.set_vec4(shader.uniforms[Cint_to_sizet(d.index)], d.value);
    }
    for(const auto& d: mat.properties.textures)
    {
        const auto& uniform = shader.uniforms[Cint_to_sizet(d.index)];
        bind_texture(uniform, get_texture(cache, d.value));
        shader.program.set_texture(uniform);
    }
}


CompiledMaterial
compile_material
(
    Engine* engine,
    Cache* cache,
    const Vfs& vfs,
    const MaterialDescription& mat
)
{
    auto shader_id = load_compiled_material_shader(engine, vfs, cache, mat.shader);
    const auto& shader = get_compiled_material_shader(*cache, shader_id);

    auto properties = shader.default_values;

    for(const auto& nta: mat.name_to_array)
    {
        const auto found = shader.name_to_array.find(nta.first);
        if(found == shader.name_to_array.end())
        {
            LOG_ERROR("Invalid property {} in shader {}", nta.first, shader.debug_name);
            continue;
        }

        const auto& shader_index = found->second;
        const auto mat_index = nta.second;
        if(shader_index.type != mat_index.type)
        {
            LOG_ERROR("Type mismatch for {} in shader {}: {} vs {}", nta.first, shader.debug_name, shader_index.type, mat_index.type);
            continue;
        }

        switch(mat_index.type)
        {
        case MaterialPropertyType::float_type:   properties.floats  [Cint_to_sizet(shader_index.index)].value = mat.floats  [Cint_to_sizet(mat_index.index)]; break;
        case MaterialPropertyType::vec3_type:    properties.vec3s   [Cint_to_sizet(shader_index.index)].value = mat.vec3s   [Cint_to_sizet(mat_index.index)]; break;
        case MaterialPropertyType::vec4_type:    properties.vec4s   [Cint_to_sizet(shader_index.index)].value = mat.vec4s   [Cint_to_sizet(mat_index.index)]; break;
        case MaterialPropertyType::texture_type: properties.textures[Cint_to_sizet(shader_index.index)].value = load_texture(cache, vfs, mat.textures[Cint_to_sizet(mat_index.index)]); break;
        default:
            DIE("Unhandled type");
            break;
        }
    }

    return {shader_id, properties, mat.render_mode};
}


unsigned int
create_buffer()
{
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    return buffer;
}


unsigned int
create_vertex_array()
{
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    return vao;
}


CompiledGeom::CompiledGeom
(
    unsigned int a_vbo,
    unsigned int a_vao,
    unsigned int a_ebo,
    int count,
    const VertexTypes& st
)
        : vbo(a_vbo)
        , vao(a_vao)
        , ebo(a_ebo)
        , number_of_indices(count)
        , debug_shader_types(st.begin(), st.end())
{
}

void
clear(CompiledGeom* geom)
{
    // todo(Gustav): figure out why we segfault if we unbind outside if...
    if(geom->ebo != 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &geom->ebo);
        geom->ebo = 0;
    }

    if(geom->vbo != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &geom->vbo);
        geom->vbo = 0;
    }

    if(geom->vao != 0)
    {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &geom->vao);
        geom->vao = 0;
    }
}


CompiledGeom::CompiledGeom(CompiledGeom&& rhs)
    : vbo(rhs.vbo)
    , vao(rhs.vao)
    , ebo(rhs.ebo)
    , number_of_indices(rhs.number_of_indices)
    , debug_shader_types(rhs.debug_shader_types)
{
    rhs.vbo = 0;
    rhs.vao = 0;
    rhs.ebo = 0;
    rhs.number_of_indices = 0;
    rhs.debug_shader_types = {};
}


void
CompiledGeom::operator=(CompiledGeom&& rhs)
{
    clear(this);

    vbo = rhs.vbo;
    vao = rhs.vao;
    ebo = rhs.ebo;
    number_of_indices = rhs.number_of_indices;
    debug_shader_types = rhs.debug_shader_types;

    rhs.vbo = 0;
    rhs.vao = 0;
    rhs.ebo = 0;
    rhs.number_of_indices = 0;
    rhs.debug_shader_types = {};
}


CompiledGeom::~CompiledGeom()
{
    clear(this);
}


struct BufferData
{
    using PerVertex = std::function<void (std::vector<float>*, const Vertex&)>;

    int count;
    PerVertex per_vertex;
    int start = 0;

    BufferData(int c, PerVertex pv)
        : count(c)
        , per_vertex(pv)
    {
    }
};


CompiledGeom
compile_geom(const Geom& mesh, const CompiledGeomVertexAttributes& layout)
{
    using VertexVector = std::vector<float>;

    auto data = std::vector<BufferData>{};
    data.reserve(layout.elements.size());

    for(const auto& element: layout.elements)
    {
        switch(element.type)
        {
        case VertexType::position3:
            data.emplace_back(3, [](VertexVector* vertices, const Vertex& vertex)
            {
                vertices->push_back(vertex.position.x);
                vertices->push_back(vertex.position.y);
                vertices->push_back(vertex.position.z);
            });
            break;
        case VertexType::normal3:
            data.emplace_back(3, [](VertexVector* vertices, const Vertex& vertex)
            {
                vertices->push_back(vertex.normal.x);
                vertices->push_back(vertex.normal.y);
                vertices->push_back(vertex.normal.z);
            });
            break;
        case VertexType::color4:
            data.emplace_back(4, [](VertexVector* vertices, const Vertex& vertex)
            {
                vertices->push_back(vertex.color.x);
                vertices->push_back(vertex.color.y);
                vertices->push_back(vertex.color.z);
                vertices->push_back(vertex.color.w);
            });
            break;
        case VertexType::texture2:
            data.emplace_back(2, [](VertexVector* vertices, const Vertex& vertex)
            {
                vertices->push_back(vertex.texture.x);
                vertices->push_back(vertex.texture.y);
            });
            break;
        default:
            DIE("Invalid buffer type");
            break;
        }
    }

    const auto floats_per_vertex = Cint_to_sizet
    (
        std::accumulate
        (
            data.begin(), data.end(),
            0, [](auto s, const auto& d)
            {
                return s + d.count;
            }
        )
    );
    auto vertices = VertexVector{};
    vertices.reserve(mesh.vertices.size() * floats_per_vertex);
    for(const auto& vertex: mesh.vertices)
    {
        for(const auto& d: data)
        {
            d.per_vertex(&vertices, vertex);
        }
    }

    const auto vbo = create_buffer();
    const auto vao = create_vertex_array();
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData
    (
        GL_ARRAY_BUFFER,
        Csizet_to_glsizeiptr(vertices.size() * sizeof(float)),
        &vertices[0],
        GL_STATIC_DRAW
    );

    const auto stride = floats_per_vertex * sizeof(float);
    int location = 0;
    std::size_t offset = 0;
    for(const auto& d: data)
    {
        const auto normalize = false;
        glVertexAttribPointer
        (
            Cint_to_gluint(location),
            d.count,
            GL_FLOAT,
            normalize ? GL_TRUE : GL_FALSE,
            Csizet_to_glsizei(stride),
            reinterpret_cast<void*>(offset)
        );
        glEnableVertexAttribArray(Cint_to_gluint(location));

        location += 1;
        offset += Cint_to_sizet(d.count) * sizeof(float);
    }

    // todo(Gustav): use IndexBuffer as it reflects the usage better than element buffer object?
    const auto ebo = create_buffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData
    (
        GL_ELEMENT_ARRAY_BUFFER,
        Csizet_to_glsizeiptr(mesh.triangles.size() * sizeof(unsigned int)),
        &mesh.triangles[0],
        GL_STATIC_DRAW
    );

    return CompiledGeom
    {
        vbo, vao, ebo,
        Csizet_to_int(mesh.triangles.size()),
        layout.debug_types
    };
}


CompiledMeshInstance
compile_mesh
(
    Engine* engine,
    Cache* cache,
    const Vfs& vfs,
    const MeshInstance& mesh
)
{
    auto material = compile_material(engine, cache, vfs, mesh.material);
    const auto& shader = get_compiled_material_shader(*cache, material.shader);
    auto geom = compile_geom(mesh.geom, shader.mesh_layout);

    return {std::move(geom), material};
}


}
