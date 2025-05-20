#include "klotter/render/render.h"

#include "klotter/log.h"

#include "klotter/render/camera.h"
#include "klotter/render/fullscreeninfo.h"
#include "klotter/render/geom.builder.h"
#include "klotter/render/geom.h"
#include "klotter/render/opengl_utils.h"
#include "klotter/render/render.pimpl.h"
#include "klotter/render/statechanger.h"
#include "klotter/render/constants.h"

namespace klotter
{

Renderer::Renderer(const RenderSettings& set)
	: settings(set)
	, pimpl(std::make_unique<RendererPimpl>(set, FullScreenInfo{}))
{
}

Renderer::~Renderer() = default;

Skybox Renderer::make_skybox(std::shared_ptr<TextureCubemap> texture) const
{
	constexpr float size = 1.0f;
	constexpr bool invert = true;

	const auto layout = pimpl->shaders.skybox_shader.geom_layout;

	const auto triangle = geom::create_box(size, size, size, invert, colors::white).to_geom();
	auto geom = compile_geom(triangle, layout);

	LOG_INFO("Creating skybox");
	return {geom, texture};
}

struct TransparentMesh
{
	std::shared_ptr<MeshInstance> mesh;
	float squared_distance_to_camera;
};

std::shared_ptr<UnlitMaterial> Renderer::make_unlit_material()
{
	return std::make_shared<UnlitMaterial>(pimpl->shaders);
}

std::shared_ptr<DefaultMaterial> Renderer::make_default_material()
{
	return std::make_shared<DefaultMaterial>(pimpl->shaders);
}

CompiledGeomVertexAttributes Renderer::unlit_geom_layout() const
{
	return pimpl->shaders.unlit_shader.geom_layout;
}

CompiledGeomVertexAttributes Renderer::default_geom_layout() const
{
	return pimpl->shaders.default_shader.geom_layout;
}


bool Renderer::is_loaded() const
{
	return pimpl->shaders.is_loaded() && pimpl->debug.is_loaded();
}

glm::mat4 rot_from_basis(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	return glm::mat4{glm::vec4{a, 0}, glm::vec4{b, 0}, glm::vec4{c, 0}, glm::vec4{0, 0, 0, 1}};
}

glm::mat4 calc_mesh_transform(std::shared_ptr<MeshInstance> m, const CompiledCamera& cc)
{
	const auto calc_fixed_right = [](const glm::vec3& normal, const glm::vec3& up)
	{
		const auto right = glm::normalize(glm::cross(normal, up));
		const auto new_up = glm::normalize(glm::cross(right, normal));
		return rot_from_basis(right, new_up, normal);
	};

	const auto calc_fixed_up = [](const glm::vec3& normal, const glm::vec3& up)
	{
		const auto right = glm::normalize(glm::cross(normal, up));
		const auto new_normal = glm::normalize(glm::cross(right, up));
		return rot_from_basis(right, up, new_normal);
	};

	const auto translation = glm::translate(glm::mat4(1.0f), m->position);

	// todo(Gustav): verify that the billboards are oriented correctly, grass in example 3 is twosided...
	switch (m->billboarding)
	{
	case Billboarding::screen:
		{
			const auto rotation
				= calc_fixed_right(glm::normalize(m->position - cc.position), glm::vec3{0, 1, 0});
			return translation * rotation;
		}
	case Billboarding::screen_fast:
		{
			// todo(Gustav): move to precalculated or remove?
			const auto rotation = calc_fixed_right(cc.in, glm::vec3{0, 1, 0});
			return translation * rotation;
		}
	case Billboarding::axial_y:
		{
			const auto rotation
				= calc_fixed_up(glm::normalize(m->position - cc.position), glm::vec3{0, 1, 0});
			return translation * rotation;
		}
	case Billboarding::axial_y_fast:
		{
			// todo(Gustav): move to precalculated or remove?
			const auto rotation = calc_fixed_up(cc.in, glm::vec3{0, 1, 0});
			return translation * rotation;
		}
	default:
		{
			const auto rotation = get_mesh_rotation_matrix(m->rotation);
			return translation * rotation;
		}
	}
};

void render_debug_lines(
	Renderer* r, const CompiledCamera& compiled_camera, const glm::ivec2& window_size
)
{
	int line_state = -1;
	constexpr int line_state_solid = 1;
	constexpr int line_state_dash = 0;

	// todo(Gustav): draw solid lines here
	r->pimpl->debug.line_shader.use();
	r->pimpl->debug.line_shader.set_mat(
		r->pimpl->debug.line_projection, compiled_camera.projection
	);
	r->pimpl->debug.line_shader.set_mat(r->pimpl->debug.line_view, compiled_camera.view);
	for (const auto& line: r->debug.debug_lines)
	{
		if (line.style == LineStyle::always_visible) continue;
		StateChanger{&r->pimpl->states}.depth_func(Compare::less).depth_test(true);
		if (line_state != line_state_solid)
		{
			r->pimpl->debug.set_line_line();
			line_state = line_state_solid;
		}
		r->pimpl->debug.line_batch.line(line.from, line.to, line.color);
	}
	r->pimpl->debug.line_batch.submit();

	for (const auto& line: r->debug.debug_lines)
	{
		if (line.style != LineStyle::always_visible) continue;
		StateChanger{&r->pimpl->states}.depth_test(false);
		if (line_state != line_state_solid)
		{
			r->pimpl->debug.set_line_line();
			line_state = line_state_solid;
		}
		r->pimpl->debug.line_batch.line(line.from, line.to, line.color);
	}
	r->pimpl->debug.line_batch.submit();

	// todo(Gustav): start drawing dashed lines here
	for (const auto& line: r->debug.debug_lines)
	{
		if (line.style != LineStyle::dashed_when_hidden) continue;
		StateChanger{&r->pimpl->states}.depth_func(Compare::greater).depth_test(false);
		if (line_state != line_state_dash)
		{
			r->pimpl->debug.set_line_dash({window_size.x, window_size.y}, 20.0f, 20.0f);
			line_state = line_state_dash;
		}
		r->pimpl->debug.line_batch.line(line.from, line.to, line.color);
	}
	r->pimpl->debug.line_batch.submit();
	r->debug.debug_lines.clear();
}

void Renderer::render_world(const glm::ivec2& window_size, const World& world, const Camera& camera)
{
	const auto has_outlined_meshes = std::any_of(
		world.meshes.begin(),
		world.meshes.end(),
		[](const auto& mesh) { return mesh->outline.has_value(); }
	);
	StateChanger{&pimpl->states}
		.cull_face(true)
		.cull_face_mode(CullFace::back)
		.stencil_mask(0xFF)
		.stencil_test(has_outlined_meshes)
		.depth_test(true)
		.depth_mask(true)
		.stencil_op(StencilAction::keep, StencilAction::replace, StencilAction::replace)
		.blend_mode(Blend::src_alpha, Blend::one_minus_src_alpha);

	glClearColor(0, 0, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	const auto compiled_camera = compile(camera, window_size);

	auto bound_camera_buffer = BoundUniformBuffer{pimpl->camera_uniform_buffer.buffer.get()};
	pimpl->camera_uniform_buffer.set_props(compiled_camera);

	std::vector<TransparentMesh> transparent_meshes;

	for (auto& m: world.meshes)
	{
		const auto not_transparent = RenderContext{ModelSource::Uniform, UseTransparency::no};

		if (m->material->is_transparent())
		{
			transparent_meshes.emplace_back(
				TransparentMesh{m, glm::length2(camera.position - m->position)}
			);
			continue;
		}
		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(true)
			.depth_func(Compare::less)
			.blending(false)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);

		if (m->outline)
		{
			StateChanger{&pimpl->states}.stencil_func(Compare::always, 1, 0xFF).stencil_mask(0xFF);
		}
		m->material->use_shader(not_transparent);
		m->material->set_uniforms(
			not_transparent, compiled_camera, calc_mesh_transform(m, compiled_camera)
		);
		m->material->bind_textures(not_transparent, &pimpl->states, &assets);
		m->material->apply_lights(not_transparent, world.lights, settings, &pimpl->states, &assets);

		render_geom(*m->geom);
	}

	for (auto& m: world.instances)
	{
		const auto not_transparent = RenderContext{ModelSource::Instanced_mat4, UseTransparency::no};

		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(true)
			.depth_func(Compare::less)
			.blending(false)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);
		m->material->use_shader(not_transparent);
		m->material->set_uniforms(
			// todo(Gustav): should we really set the model matrix for instanced meshes?
			not_transparent, compiled_camera, std::nullopt
		);
		m->material->bind_textures(not_transparent, &pimpl->states, &assets);
		m->material->apply_lights(not_transparent, world.lights, settings, &pimpl->states, &assets);

		render_geom_instanced(*m);
	}

	render_debug_lines(this, compiled_camera, window_size);

	// render skybox
	if (world.skybox.cubemap != nullptr && world.skybox.geom != nullptr)
	{
		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(false)
			.depth_func(Compare::less_equal)
			.blending(false)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);

		auto& shader = pimpl->shaders.skybox_shader;

		shader.program->use();
		bind_texture_cubemap(&pimpl->states, shader.tex_skybox, *world.skybox.cubemap);

		render_geom(*world.skybox.geom);
	}

	std::sort(
		transparent_meshes.begin(),
		transparent_meshes.end(),
		[](const auto& lhs, const auto& rhs)
		{ return lhs.squared_distance_to_camera > rhs.squared_distance_to_camera; }
	);

	for (auto& tm: transparent_meshes)
	{
		const auto transparent = RenderContext{ModelSource::Uniform, UseTransparency::yes};

		const auto& m = tm.mesh;
		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(true)
			.depth_func(Compare::less)
			.blending(true)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);

		if (m->outline)
		{
			StateChanger{&pimpl->states}.stencil_func(Compare::always, 1, 0xFF).stencil_mask(0xFF);
		}
		m->material->use_shader(transparent);
		m->material->set_uniforms(
			transparent, compiled_camera, calc_mesh_transform(m, compiled_camera)
		);
		m->material->bind_textures(transparent, &pimpl->states, &assets);
		m->material->apply_lights(transparent, world.lights, settings, &pimpl->states, &assets);

		render_geom(*m->geom);
	}

	// render outline over all other meshes
	if (has_outlined_meshes)
	{
		for (auto& m: world.meshes)
		{
			if (m->outline)
			{
				StateChanger{&pimpl->states}
					.stencil_func(Compare::not_equal, 1, 0xFF)
					.stencil_mask(0x00)
					.depth_test(false);
				const auto small_scale_mat = glm::scale(glm::mat4(1.0f), {OUTLINE_SCALE, OUTLINE_SCALE, OUTLINE_SCALE});

				auto& shader = pimpl->shaders.single_color_shader;
				shader.program->use();
				shader.program->set_vec4(shader.tint_color, {*m->outline, 1});

				shader.program->set_mat(
					shader.model, calc_mesh_transform(m, compiled_camera) * small_scale_mat
				);

				render_geom(*m->geom);
			}
		}
	}
}

}  //  namespace klotter
