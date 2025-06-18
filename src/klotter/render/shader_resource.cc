#include "klotter/render/shader_resource.h"

#include "constants.h"
#include "shader.source.h"
#include "klotter/str.h"
#include "klotter/cpp.h"

#include "klotter/render/shader.h"
#include "klotter/render/camera.h"
#include "klotter/render/render_settings.h"
#include "klotter/render/fullscreen.h"

#include "pp.vert.glsl.h"
#include "pp.invert.frag.glsl.h"
#include "pp.grayscale.frag.glsl.h"
#include "pp.damage.frag.glsl.h"
#include "pp.blur.frag.glsl.h"
#include "pp.always.frag.glsl.h"

namespace klotter
{

void CameraUniformBuffer::set_props(const CompiledCamera& cc)
{
	buffer->set_mat4(projection_prop, cc.projection);
	buffer->set_mat4(view_prop, cc.view);
}

LoadedShader_SingleColor::LoadedShader_SingleColor(
	std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, geom_layout(std::move(l))
	, tint_color(program->get_uniform("u_material.diffuse_tint"))
	, model(program->get_uniform("u_model"))
{
	program->setup_uniform_block(desc.setup);
}

LoadedShader_Skybox::LoadedShader_Skybox(
	std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l, const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, geom_layout(std::move(l))
	, tex_skybox(program->get_uniform("u_skybox_tex"))
{
	setup_textures(program.get(), {&tex_skybox});
	program->setup_uniform_block(desc.setup);
}

LoadedShader_Unlit::LoadedShader_Unlit(
	TransformSource model_source,
	std::shared_ptr<ShaderProgram> p,
	const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, tint_color(program->get_uniform("u_material.diffuse_tint"))
	, tex_diffuse(program->get_uniform("u_material.diffuse_tex"))
	, model(
		  model_source == TransformSource::Uniform ? std::optional<Uniform>{program->get_uniform("u_model")} : std::nullopt
	  )
{
	setup_textures(program.get(), {&tex_diffuse});
	program->setup_uniform_block(desc.setup);
}

DirectionalLightUniforms::DirectionalLightUniforms(ShaderProgram* program, const std::string& base)
	: light_diffuse_color(program->get_uniform(base + "diffuse"))
	, light_specular_color(program->get_uniform(base + "specular"))
	, dir(program->get_uniform(base + "dir"))
{
}

PointLightUniforms::PointLightUniforms(ShaderProgram* program, const std::string& base)
	: light_diffuse_color(program->get_uniform(base + "diffuse"))
	, light_specular_color(program->get_uniform(base + "specular"))
	, light_attenuation(program->get_uniform(base + "attenuation"))
	, light_world(program->get_uniform(base + "world_pos"))
{
}

FrustumLightUniforms::FrustumLightUniforms(ShaderProgram* program, const std::string& base)
	: diffuse(program->get_uniform(base + "diffuse"))
	, specular(program->get_uniform(base + "specular"))
	, attenuation(program->get_uniform(base + "attenuation"))
	, world_to_clip(program->get_uniform(base + "world_to_clip"))
	, world_pos(program->get_uniform(base + "world_pos"))
	, cookie(program->get_uniform(base + "cookie"))
{
}

PostProcSetup operator|(PostProcSetup lhs, PostProcSetup rhs)
{
	return static_cast<PostProcSetup>(base_cast(lhs) | base_cast(rhs));
}

std::optional<Uniform> get_uniform(
	ShaderProgram& prog, const std::string& name, PostProcSetup setup, PostProcSetup flag
)
{
	if (is_flag_set(setup, flag))
	{
		return prog.get_uniform(name);
	}
	else
	{
		return std::nullopt;
	}
}

LoadedPostProcShader::LoadedPostProcShader(std::shared_ptr<ShaderProgram> s, PostProcSetup setup)
	: program(std::move(s))
	, texture(program->get_uniform("u_texture"))
	, factor(get_uniform(*program, "u_factor", setup, PostProcSetup::factor))
	, resolution(get_uniform(*program, "u_resolution", setup, PostProcSetup::resolution))
	, time(get_uniform(*program, "u_time", setup, PostProcSetup::time))
{
	setup_textures(program.get(), {&texture});
}

LoadedShader_Default::LoadedShader_Default(
	TransformSource model_source,
	std::shared_ptr<ShaderProgram> p,
	const RenderSettings& settings,
	const CameraUniformBuffer& desc
)
	: program(std::move(p))
	, tint_color(program->get_uniform("u_material.diffuse_tint"))
	, tex_diffuse(program->get_uniform("u_material.diffuse_tex"))
	, tex_specular(program->get_uniform("u_material.specular_tex"))
	, tex_emissive(program->get_uniform("u_material.emissive_tex"))
	, ambient_tint(program->get_uniform("u_material.ambient_tint"))
	, specular_color(program->get_uniform("u_material.specular_tint"))
	, shininess(program->get_uniform("u_material.shininess"))
	, emissive_factor(program->get_uniform("u_material.emissive_factor"))
	, model(
		  model_source == TransformSource::Uniform ? std::optional<Uniform>{program->get_uniform("u_model")} : std::nullopt
	  )
	, view_position(program->get_uniform("u_view_position"))
	, light_ambient_color(program->get_uniform("u_ambient_light"))
{
	for (int i = 0; i < settings.number_of_directional_lights; i += 1)
	{
		const std::string base = Str{} << "u_directional_lights[" << i << "].";
		directional_lights.emplace_back(program.get(), base);
	}

	for (int i = 0; i < settings.number_of_point_lights; i += 1)
	{
		const std::string base = Str{} << "u_point_lights[" << i << "].";
		point_lights.emplace_back(program.get(), base);
	}

	for (int i = 0; i < settings.number_of_frustum_lights; i += 1)
	{
		const std::string base = Str{} << "u_frustum_lights[" << i << "].";
		frustum_lights.emplace_back(program.get(), base);
	}

	std::vector<Uniform*> textures = {&tex_diffuse, &tex_specular, &tex_emissive};
	for (auto& fl: frustum_lights)
	{
		textures.emplace_back(&fl.cookie);
	}

	setup_textures(program.get(), textures);
	program->setup_uniform_block(desc.setup);
}

const LoadedShader_Unlit& shader_from_container(const LoadedShader_Unlit_Container& container, const RenderContext& rc)
{
	return rc.use_transparency == UseTransparency::yes ? container.transparency_shader : container.default_shader;
}

bool LoadedShader_Unlit_Container::is_loaded() const
{
	return default_shader.program->is_loaded() && transparency_shader.program->is_loaded();
}

const LoadedShader_Default& shader_from_container(const LoadedShader_Default_Container& container, const RenderContext& rc)
{
	switch (rc.model_source)
	{
	case TransformSource::Uniform:
		return rc.use_transparency == UseTransparency::yes ? container.transparency_shader : container.default_shader;
	case TransformSource::Instanced_mat4:
		assert(rc.use_transparency == UseTransparency::no);	 // not currently supporting instanced transparency
		return container.default_shader_instance;
	default: assert(false && "unhandled"); return container.default_shader;
	}
}

bool LoadedShader_Default_Container::is_loaded() const
{
	return default_shader.program->is_loaded() && transparency_shader.program->is_loaded();
}

bool ShaderResource::is_loaded() const
{
	return single_color_shader.program->is_loaded() && skybox_shader.program->is_loaded() && unlit_shader_container.is_loaded()
		&& default_shader_container.is_loaded() && pp_invert->program->is_loaded() && pp_grayscale->program->is_loaded()
		&& pp_blurv->program->is_loaded() && pp_blurh->program->is_loaded() && pp_always->program->is_loaded();
}

using BaseShaderData = std::vector<VertexType>;

BaseShaderData get_vertex_types(const ShaderVertexAttributes& va)
{
	auto ret = BaseShaderData{};
	for (const auto& v: va)
	{
		ret.emplace_back(v.type);
	}
	return ret;
}


// todo(Gustav): should this be a tuple instead? this way the members are named
struct LoadedShader
{
	LoadedShader(std::shared_ptr<ShaderProgram> p, CompiledGeomVertexAttributes l)
		: program(std::move(p))
		, geom_layout(std::move(l))
	{}

	std::shared_ptr<ShaderProgram> program;
	CompiledGeomVertexAttributes geom_layout;
};

LoadedShader load_shader(DEBUG_LABEL_ARG_MANY const BaseShaderData& base_layout, const VertexShaderSource& source, TransformSource model_source)
{
	auto layout_compiler = compile_attribute_layouts(base_layout, {source.layout});
	const auto geom_layout = get_geom_layout(layout_compiler);

	std::optional<InstanceProp> instance_prop;
	switch (model_source)
	{
	case TransformSource::Instanced_mat4: instance_prop = InstanceProp{VertexType::instance_transform, "u_model"}; break;
	case TransformSource::Uniform: break;
	default: assert(false && "unhandled ModelSource");
	}

	const auto compiled_layout = compile_shader_layout(layout_compiler, source.layout, instance_prop);

	auto program = std::make_shared<ShaderProgram>(USE_DEBUG_LABEL_MANY(debug_label) source.vertex, source.fragment, compiled_layout);

	return {program, geom_layout};
}

ShaderResource load_shaders(const CameraUniformBuffer& desc, const RenderSettings& settings, const FullScreenGeom& full_screen)
{
	const auto single_color_shader = load_shader_source({}, desc.setup.source);

	// todo(Gustav): fix skybox shader
	const auto skybox_source = load_skybox_source(desc.setup.source);
	const auto skybox_shader = VertexShaderSource{
		ShaderVertexAttributes{{VertexType::position3, "a_position"}}, skybox_source.vertex, skybox_source.fragment
	};

	BaseShaderData global_shader_data = get_vertex_types(single_color_shader.layout);

	ShaderOptions unlit_shader_options;
	unlit_shader_options.use_texture = true;

	ShaderOptions default_shader_options;
	default_shader_options.use_lights = true;
	default_shader_options.use_texture = true;
	default_shader_options.number_of_directional_lights = settings.number_of_directional_lights;
	default_shader_options.number_of_point_lights = settings.number_of_point_lights;
	default_shader_options.number_of_frustum_lights = settings.number_of_frustum_lights;

	auto loaded_unlit = load_shader(
		USE_PROGRAM_LABEL_MANY("unlit")
		global_shader_data,
		load_shader_source(unlit_shader_options.with_transparent_cutoff(), desc.setup.source),
		TransformSource::Uniform
	);
	auto loaded_default = load_shader(
		USE_PROGRAM_LABEL_MANY("default")
		global_shader_data,
		load_shader_source(default_shader_options.with_transparent_cutoff(), desc.setup.source),
		TransformSource::Uniform
	);
	auto loaded_default_instanced = load_shader(
		USE_PROGRAM_LABEL_MANY("default instanced")
		global_shader_data,
		load_shader_source(default_shader_options.with_transparent_cutoff().with_instanced_mat4(), desc.setup.source),
		TransformSource::Instanced_mat4
	);

	auto loaded_unlit_transparency = load_shader(
		USE_PROGRAM_LABEL_MANY("unlit transparency")
		global_shader_data, load_shader_source(unlit_shader_options, desc.setup.source), TransformSource::Uniform
	);
	auto loaded_default_transparency = load_shader(
		USE_PROGRAM_LABEL_MANY("default transparency")
		global_shader_data, load_shader_source(default_shader_options, desc.setup.source), TransformSource::Uniform
	);

	// todo(Gustav): should the asserts here be runtime errors? currently all setups are compile-time...
	assert(loaded_unlit.geom_layout.debug_types == loaded_unlit_transparency.geom_layout.debug_types);
	assert(loaded_default.geom_layout.debug_types == loaded_default_transparency.geom_layout.debug_types);
	assert(
		loaded_default.geom_layout.debug_types == loaded_default_instanced.geom_layout.debug_types
	);	// is this valid? should this fail?

	auto pp_invert = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_PROGRAM_LABEL_MANY("pp invert")
			std::string{PP_VERT_GLSL}, std::string{PP_INVERT_FRAG_GLSL}, full_screen.layout
		),
		PostProcSetup::factor
	);
	auto pp_grayscale = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_PROGRAM_LABEL_MANY("pp grayscale")
			std::string{PP_VERT_GLSL}, std::string{PP_GRAYSCALE_FRAG_GLSL}, full_screen.layout
		),
		PostProcSetup::factor
	);
	auto pp_damage = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_PROGRAM_LABEL_MANY("pp damage")
			std::string{PP_VERT_GLSL}, std::string{PP_DAMAGE_FRAG_GLSL}, full_screen.layout
		),
		PostProcSetup::factor | PostProcSetup::resolution | PostProcSetup::time
	);

	constexpr IsGauss use_gauss =
#if HAS(BLUR_USE_GAUSS)
		IsGauss::yes
#else
		IsGauss::no
#endif
		;

	auto pp_blurv = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_PROGRAM_LABEL_MANY("pp blur vert")
			std::string{PP_VERT_GLSL},
			generate_blur(PP_BLUR_FRAG_GLSL, {BlurType::vertical, BLUR_SAMPLES, use_gauss}),
			full_screen.layout
		),
		PostProcSetup::factor
	);
	auto pp_blurh = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_PROGRAM_LABEL_MANY("pp blur hor")
			std::string{PP_VERT_GLSL},
			generate_blur(PP_BLUR_FRAG_GLSL, {BlurType::horizontal, BLUR_SAMPLES, use_gauss}),
			full_screen.layout
		),
		PostProcSetup::factor | PostProcSetup::resolution
	);

	auto pp_always = std::make_shared<LoadedPostProcShader>(
		std::make_shared<ShaderProgram>(
			USE_PROGRAM_LABEL_MANY("pp always")
			std::string{PP_VERT_GLSL}, std::string{PP_ALWAYS_FRAG_GLSL}, full_screen.layout
		),
		PostProcSetup::none
	);

	auto loaded_single_color = load_shader(
		USE_PROGRAM_LABEL_MANY("single color") global_shader_data, single_color_shader, TransformSource::Uniform
	);
	auto loaded_skybox_shader
		= load_shader(USE_PROGRAM_LABEL_MANY("skybox"){}, skybox_shader, TransformSource::Uniform);

	return {
		// todo(Gustav): not really happy with sending "the same" argument twice
		LoadedShader_SingleColor{std::move(loaded_single_color.program), loaded_single_color.geom_layout, desc},
		LoadedShader_Skybox{std::move(loaded_skybox_shader.program), loaded_skybox_shader.geom_layout, desc},
		LoadedShader_Unlit_Container{
			loaded_unlit.geom_layout,
			LoadedShader_Unlit{TransformSource::Uniform, std::move(loaded_unlit.program), desc},
			LoadedShader_Unlit{TransformSource::Uniform, std::move(loaded_unlit_transparency.program), desc}
		},
		LoadedShader_Default_Container{
			loaded_default.geom_layout,
			LoadedShader_Default{TransformSource::Uniform, std::move(loaded_default.program), settings, desc},
			LoadedShader_Default{TransformSource::Uniform, std::move(loaded_default_transparency.program), settings, desc},
			LoadedShader_Default{TransformSource::Instanced_mat4, std::move(loaded_default_instanced.program), settings, desc}
		},
		pp_invert,
		pp_grayscale,
		pp_damage,
		pp_blurv,
		pp_blurh,
		pp_always
	};
}

}  //  namespace klotter
