#include "klotter/render/material.h"

#include "klotter/assert.h"
#include "klotter/cint.h"

#include "klotter/render/assets.h"
#include "klotter/render/camera.h"
#include "klotter/render/render_settings.h"
#include "klotter/render/shader.h"
#include "klotter/render/shader_resource.h"
#include "klotter/render/world.h"
#include "klotter/render/statechanger.h"

#include "klotter/render/constants.h"

namespace klotter
{

UnlitMaterial::UnlitMaterial(const ShaderResource& resource)
	: shader_container(&resource.unlit_shader_container)
{
}

void UnlitMaterial::use_shader(const RenderContext& rc)
{
	shader_from_container(*shader_container, rc).program->use();
}

void set_optional_mat(
	ShaderProgram* program, const std::optional<Uniform>& uniform, const std::optional<glm::mat4>& transform
)
{
	if (uniform && transform)
	{
		program->set_mat(*uniform, *transform);
	}
	else
	{
		assert(uniform.has_value() == false);
		assert(transform.has_value() == false);
	}
}

void UnlitMaterial::set_uniforms(
	const RenderContext& rc, const CompiledCamera&, const std::optional<glm::mat4>& world_from_local
)
{
	const auto& shader = shader_from_container(*shader_container, rc);
	shader.program->set_vec4(shader.tint_color_uni, {color, alpha});
	set_optional_mat(shader.program.get(), shader.world_from_local_uni, world_from_local);
}

void UnlitMaterial::bind_textures(const RenderContext& rc, OpenglStates* states, Assets* assets)
{
	std::shared_ptr<Texture2d> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}

	bind_texture_2d(states, shader_from_container(*shader_container, rc).tex_diffuse_uni, *t);
}

void UnlitMaterial::apply_lights(const RenderContext&, const Lights&, const RenderSettings&, OpenglStates*, Assets*)
{
	// no lights for unlit material
}

bool UnlitMaterial::is_transparent() const
{
	// todo(Gustav): improve transparency
	return alpha < ALPHA_TRANSPARENCY_LIMIT;
}

DefaultMaterial::DefaultMaterial(const ShaderResource& resource)
	: shader_container(&resource.default_shader_container)
{
}

void DefaultMaterial::use_shader(const RenderContext& rc)
{
	shader_from_container(*shader_container, rc).program->use();
}

void DefaultMaterial::set_uniforms(
	const RenderContext& rc, const CompiledCamera& cc, const std::optional<glm::mat4>& world_from_local
)
{
	const auto& shader = shader_from_container(*shader_container, rc);

	shader.program->set_vec4(shader.tint_color_uni, {color, alpha});
	shader.program->set_vec3(shader.ambient_tint_uni, ambient_tint);
	shader.program->set_vec3(shader.specular_color_uni, specular_color);
	shader.program->set_float(shader.shininess_uni, shininess);
	shader.program->set_float(shader.emissive_factor_uni, emissive_factor);

	set_optional_mat(shader.program.get(), shader.world_from_local_uni, world_from_local);
	shader.program->set_vec3(shader.view_position_uni, cc.position);
}

std::shared_ptr<Texture2d> get_or_white(Assets* assets, std::shared_ptr<Texture2d> t)
{
	if (t == nullptr)
	{
		return assets->get_white();
	}
	else
	{
		return t;
	}
};

std::shared_ptr<Texture2d> get_or_black(Assets* assets, std::shared_ptr<Texture2d> t)
{
	if (t == nullptr)
	{
		return assets->get_black();
	}
	else
	{
		return t;
	}
};

void DefaultMaterial::bind_textures(const RenderContext& rc, OpenglStates* states, Assets* assets)
{
	const auto& shader = shader_from_container(*shader_container, rc);
	bind_texture_2d(states, shader.tex_diffuse_uni, *get_or_white(assets, diffuse));
	bind_texture_2d(states, shader.tex_specular_uni, *get_or_white(assets, specular));
	bind_texture_2d(states, shader.tex_emissive_uni, *get_or_black(assets, emissive));
}

void DefaultMaterial::apply_lights(
	const RenderContext& rc, const Lights& lights, const RenderSettings& settings, OpenglStates* states, Assets* assets
)
{
	const auto& shader = shader_from_container(*shader_container, rc);
	shader.program->set_vec3(shader.light_ambient_color_uni, lights.color * lights.ambient);

	constexpr auto no_directional_light = ([]() {
		DirectionalLight p;
		p.color = colors::black;
		p.diffuse = 0.0f;
		p.specular = 0.0f;
		return p;
	})();
	constexpr auto no_point_light = ([]() {
		PointLight p;
		p.color = colors::black;
		p.diffuse = 0.0f;
		p.specular = 0.0f;
		return p;
	})();
	auto no_frustum_light = ([]() {
		FrustumLight p;
		p.color = colors::black;
		p.diffuse = 0.0f;
		p.specular = 0.0f;
		return p;
	})();

	// todo(Gustav): graph the most influential lights instead of the first N lights
	for (int index = 0; index < settings.number_of_directional_lights; index += 1)
	{
		const auto& p = Cint_to_sizet(index) < lights.directional_lights.size()
						  ? lights.directional_lights[Cint_to_sizet(index)]
						  : no_directional_light;
		const auto& u = shader.directional_lights[Cint_to_sizet(index)];
		shader.program->set_vec3(u.light_diffuse_color_uni, p.color * p.diffuse);
		shader.program->set_vec3(u.light_specular_color_uni, p.color * p.specular);
		shader.program->set_vec3(u.dir_uni, p.direction);
	}

	for (int index = 0; index < settings.number_of_point_lights; index += 1)
	{
		const auto& p = Cint_to_sizet(index) < lights.point_lights.size()
			? lights.point_lights[Cint_to_sizet(index)]
			: no_point_light
		;
		const auto& u = shader.point_lights[Cint_to_sizet(index)];
		shader.program->set_vec3(u.light_diffuse_color_uni, p.color * p.diffuse);
		shader.program->set_vec3(u.light_specular_color_uni, p.color * p.specular);
		shader.program->set_vec3(u.light_world_uni, p.position);
		shader.program->set_vec4(u.light_attenuation_uni, {p.min_range, p.max_range, p.curve.slope, p.curve.threshold});
	}

	for (int index = 0; index < settings.number_of_frustum_lights; index += 1)
	{
		const auto& p = Cint_to_sizet(index) < lights.frustum_lights.size() ? lights.frustum_lights[Cint_to_sizet(index)]
																		: no_frustum_light;
		const auto& u = shader.frustum_lights[Cint_to_sizet(index)];
		shader.program->set_vec3(u.diffuse_uni, p.color * p.diffuse);
		shader.program->set_vec3(u.specular_uni, p.color * p.specular);
		shader.program->set_vec3(u.world_pos_uni, p.position);
		shader.program->set_vec4(u.attenuation_uni, {p.min_range, p.max_range, p.curve.slope, p.curve.threshold});

		const auto view = create_view_mat(p.position, create_vectors(p.yaw, p.pitch));
		const auto projection = glm::perspective(glm::radians(p.fov), p.aspect, 0.1f, p.max_range);
		shader.program->set_mat(u.world_to_clip_uni, projection * view);

		bind_texture_2d(states, u.cookie_uni, *get_or_white(assets, p.cookie));
	}
}

bool DefaultMaterial::is_transparent() const
{
	// todo(Gustav): improve transparency
	return alpha < ALPHA_TRANSPARENCY_LIMIT;
}

}  //  namespace klotter
