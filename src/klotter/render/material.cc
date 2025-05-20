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
	: shader(&resource.unlit_shader)
{
}

void UnlitMaterial::use_shader(const RenderContext& rc)
{
	shader->base(rc).program->use();
}

void set_optional_mat(ShaderProgram* program, const std::optional<Uniform>& uniform, const std::optional<glm::mat4>& transform)
{
	if(transform) {
		program->set_mat(*uniform, *transform);
	}
	else {
		assert(uniform.has_value() == false);
	}
}

void UnlitMaterial::set_uniforms(
	const RenderContext& rc, const CompiledCamera&, const std::optional<glm::mat4>& transform
)
{
	const auto& base = shader->base(rc);
	base.program->set_vec4(base.tint_color, {color, alpha});
	set_optional_mat(base.program.get(), base.model, transform);
}

void UnlitMaterial::bind_textures(const RenderContext& rc, OpenglStates* states, Assets* assets)
{
	std::shared_ptr<Texture2d> t = texture;
	if (t == nullptr)
	{
		t = assets->get_white();
	}

	bind_texture_2d(states, shader->base(rc).tex_diffuse, *t);
}

void UnlitMaterial::
	apply_lights(const RenderContext&, const Lights&, const RenderSettings&, OpenglStates*, Assets*)
{
	// no lights for unlit material
}

bool UnlitMaterial::is_transparent() const
{
	// todo(Gustav): improve transparency
	return alpha < ALPHA_TRANSPARENCY_LIMIT;
}

DefaultMaterial::DefaultMaterial(const ShaderResource& resource)
	: shader(&resource.default_shader)
{
}

void DefaultMaterial::use_shader(const RenderContext& rc)
{
	shader->base(rc).program->use();
}

void DefaultMaterial::set_uniforms(
	const RenderContext& rc, const CompiledCamera& cc, const std::optional<glm::mat4>& transform
)
{
	const auto& base = shader->base(rc);

	base.program->set_vec4(base.tint_color, {color, alpha});
	base.program->set_vec3(base.ambient_tint, ambient_tint);
	base.program->set_vec3(base.specular_color, specular_color);
	base.program->set_float(base.shininess, shininess);
	base.program->set_float(base.emissive_factor, emissive_factor);

	set_optional_mat(base.program.get(), base.model, transform);
	base.program->set_vec3(base.view_position, cc.position);
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
	const auto& base = shader->base(rc);
	bind_texture_2d(states, base.tex_diffuse, *get_or_white(assets, diffuse));
	bind_texture_2d(states, base.tex_specular, *get_or_white(assets, specular));
	bind_texture_2d(states, base.tex_emissive, *get_or_black(assets, emissive));
}

void DefaultMaterial::apply_lights(
	const RenderContext& rc,
	const Lights& lights,
	const RenderSettings& settings,
	OpenglStates* states,
	Assets* assets
)
{
	const auto& base = shader->base(rc);
	base.program->set_vec3(base.light_ambient_color, lights.color * lights.ambient);

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
	for (int i = 0; i < settings.number_of_directional_lights; i += 1)
	{
		const auto& p = Cint_to_sizet(i) < lights.directional_lights.size()
						  ? lights.directional_lights[Cint_to_sizet(i)]
						  : no_directional_light;
		const auto& u = base.directional_lights[Cint_to_sizet(i)];
		base.program->set_vec3(u.light_diffuse_color, p.color * p.diffuse);
		base.program->set_vec3(u.light_specular_color, p.color * p.specular);
		base.program->set_vec3(u.dir, p.direction);
	}

	for (int i = 0; i < settings.number_of_point_lights; i += 1)
	{
		const auto& p = Cint_to_sizet(i) < lights.point_lights.size()
						  ? lights.point_lights[Cint_to_sizet(i)]
						  : no_point_light;
		const auto& u = base.point_lights[Cint_to_sizet(i)];
		base.program->set_vec3(u.light_diffuse_color, p.color * p.diffuse);
		base.program->set_vec3(u.light_specular_color, p.color * p.specular);
		base.program->set_vec3(u.light_world, p.position);
		base.program->set_vec4(
			u.light_attenuation, {p.min_range, p.max_range, p.curve.curve.s, p.curve.curve.t}
		);
	}

	for (int i = 0; i < settings.number_of_frustum_lights; i += 1)
	{
		const auto& p = Cint_to_sizet(i) < lights.frustum_lights.size()
						  ? lights.frustum_lights[Cint_to_sizet(i)]
						  : no_frustum_light;
		const auto& u = base.frustum_lights[Cint_to_sizet(i)];
		base.program->set_vec3(u.diffuse, p.color * p.diffuse);
		base.program->set_vec3(u.specular, p.color * p.specular);
		base.program->set_vec3(u.world_pos, p.position);
		base.program->set_vec4(
			u.attenuation, {p.min_range, p.max_range, p.curve.curve.s, p.curve.curve.t}
		);

		const auto view = create_view_mat(p.position, create_vectors(p.yaw, p.pitch));
		const auto projection = glm::perspective(glm::radians(p.fov), p.aspect, 0.1f, p.max_range);
		base.program->set_mat(u.world_to_clip, projection * view);

		bind_texture_2d(states, u.cookie, *get_or_white(assets, p.cookie));
	}
}

bool DefaultMaterial::is_transparent() const
{
	// todo(Gustav): improve transparency
	return alpha < ALPHA_TRANSPARENCY_LIMIT;
}

}  //  namespace klotter
