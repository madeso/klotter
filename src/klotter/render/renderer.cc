#include "klotter/render/renderer.h"

#include "klotter/log.h"

#include "klotter/render/camera.h"
#include "klotter/render/fullscreen.h"
#include "klotter/render/geom.builder.h"
#include "klotter/render/geom.h"
#include "klotter/render/opengl_utils.h"
#include "klotter/render/renderer.pimpl.h"
#include "klotter/render/statechanger.h"
#include "klotter/render/constants.h"

namespace klotter
{

Renderer::Renderer(const RenderSettings& set)
	: settings(set)
	, pimpl(std::make_unique<RendererPimpl>(set, FullScreenGeom{}))
{
}

Renderer::~Renderer() = default;

Skybox Renderer::make_skybox(std::shared_ptr<TextureCubemap> texture) const
{
	constexpr float size = 1.0f;

	const auto layout = pimpl->shaders_resources.skybox_shader.geom_layout;

	const auto triangle = geom::create_box(size, size, size, geom::NormalsFacing::In, colors::white).to_geom();
	auto geom = compile_geom(USE_DEBUG_LABEL_MANY("skybox") triangle, layout);

	LOG_INFO("Created skybox");
	return {geom, std::move(texture)};
}

struct TransparentMesh
{
	std::shared_ptr<MeshInstance> mesh;
	float squared_distance_to_camera;
};

std::shared_ptr<UnlitMaterial> Renderer::make_unlit_material() const
{
	return std::make_shared<UnlitMaterial>(pimpl->shaders_resources);
}

std::shared_ptr<DefaultMaterial> Renderer::make_default_material() const
{
	return std::make_shared<DefaultMaterial>(pimpl->shaders_resources);
}

CompiledGeomVertexAttributes Renderer::unlit_geom_layout() const
{
	return pimpl->shaders_resources.unlit_shader_container.geom_layout;
}

CompiledGeomVertexAttributes Renderer::default_geom_layout() const
{
	return pimpl->shaders_resources.default_shader_container.geom_layout;
}

bool Renderer::is_loaded() const
{
	return pimpl->shaders_resources.is_loaded() && pimpl->debug_drawer.is_loaded();
}

glm::mat4 rot_from_basis(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	return glm::mat4{glm::vec4{a, 0}, glm::vec4{b, 0}, glm::vec4{c, 0}, glm::vec4{0, 0, 0, 1}};
}

// todo(Gustav): should functions take shared_ptr?
glm::mat4 calc_world_from_local(std::shared_ptr<MeshInstance> m, const CompiledCamera& cc)
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

	const auto translation = glm::translate(glm::mat4(1.0f), m->world_position);

	// todo(Gustav): verify that the billboards are oriented correctly, grass in example 3 is twosided...
	switch (m->billboarding)
	{
	case Billboarding::screen:
		{
			const auto rotation = calc_fixed_right(glm::normalize(m->world_position - cc.position), glm::vec3{0, 1, 0});
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
			const auto rotation = calc_fixed_up(glm::normalize(m->world_position - cc.position), glm::vec3{0, 1, 0});
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

void batch_lines(LineDrawer* drawer, const std::vector<DebugLine>& debug_lines, float gamma)
{
	for (const auto& line: debug_lines)
	{
		drawer->line_batch.line(line.from, line.to, linear_from_srgb(line.color, gamma));
	}
	drawer->line_batch.submit();
}

void render_debug_lines(const std::vector<DebugLine>& debug_lines, OpenglStates* states, LineDrawer* drawer, const CompiledCamera& compiled_camera, const glm::ivec2& window_size, float gamma)
{
	if (debug_lines.empty())
	{
		return;
	}

	drawer->shader.use();
	drawer->shader.set_mat(drawer->clip_from_view_uni, compiled_camera.clip_from_view);
	drawer->shader.set_mat(drawer->view_from_world_uni, compiled_camera.view_from_world);
	
	StateChanger{states}.depth_func(Compare::less_equal).depth_test(true);
	drawer->set_line_to_solid();
	batch_lines(drawer, debug_lines, gamma);

	StateChanger{states}.depth_func(Compare::greater).depth_test(true);
	drawer->set_line_to_dash({window_size.x, window_size.y}, 20.0f, 20.0f);
	batch_lines(drawer, debug_lines, gamma);
}

void Renderer::render_world(const glm::ivec2& window_size, const World& world, const Camera& camera)
{
	SCOPED_DEBUG_GROUP("render world call"sv);
	const auto has_outlined_meshes = std::any_of(
		world.meshes.begin(), world.meshes.end(), [](const auto& mesh) { return mesh->outline.has_value(); }
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

	if (world.meshes.empty() == false)
	{
		SCOPED_DEBUG_GROUP("render basic geom"sv);
		for (const auto& mesh: world.meshes)
		{
			const auto not_transparent_context = RenderContext{TransformSource::Uniform, UseTransparency::no, settings.gamma};

			if (mesh->material->is_transparent())
			{
				transparent_meshes.emplace_back(TransparentMesh{mesh, glm::length2(camera.position - mesh->world_position)});
				continue;
			}
			StateChanger{&pimpl->states}
				.depth_test(true)
				.depth_mask(true)
				.depth_func(Compare::less)
				.blending(false)
				.stencil_mask(0x0)
				.stencil_func(Compare::always, 1, 0xFF);

			if (mesh->outline)
			{
				StateChanger{&pimpl->states}.stencil_func(Compare::always, 1, 0xFF).stencil_mask(0xFF);
			}
			mesh->material->use_shader(not_transparent_context);
			mesh->material->set_uniforms(not_transparent_context, compiled_camera, calc_world_from_local(mesh, compiled_camera));
			mesh->material->bind_textures(not_transparent_context, &pimpl->states, &assets);
			mesh->material->apply_lights(not_transparent_context, world.lights, settings, &pimpl->states, &assets);

			render_geom(*mesh->geom);
		}
	}

	if (world.instances.empty() == false)
	{
		SCOPED_DEBUG_GROUP("render instances"sv);
		for (const auto& instance: world.instances)
		{
			const auto not_transparent_context = RenderContext{TransformSource::Instanced_mat4, UseTransparency::no, settings.gamma};

			StateChanger{&pimpl->states}
				.depth_test(true)
				.depth_mask(true)
				.depth_func(Compare::less)
				.blending(false)
				.stencil_mask(0x0)
				.stencil_func(Compare::always, 1, 0xFF);
			instance->material->use_shader(not_transparent_context);
			instance->material->set_uniforms(
				// todo(Gustav): should we really set the model matrix for instanced meshes?
				not_transparent_context,
				compiled_camera,
				std::nullopt
			);
			instance->material->bind_textures(not_transparent_context, &pimpl->states, &assets);
			instance->material->apply_lights(not_transparent_context, world.lights, settings, &pimpl->states, &assets);

			render_geom_instanced(*instance);
		}
	}

	if (debug.lines.empty() == false)
	{
		SCOPED_DEBUG_GROUP("render debug lines"sv);
		render_debug_lines(debug.lines, &pimpl->states, &pimpl->debug_drawer, compiled_camera, window_size, settings.gamma);
		debug.lines.clear();
	}

	// render skybox
	if (world.skybox && world.skybox->cubemap != nullptr && world.skybox->geom != nullptr)
	{
		SCOPED_DEBUG_GROUP("render skybox"sv);
		StateChanger{&pimpl->states}
			.depth_test(true)
			.depth_mask(false)
			.depth_func(Compare::less_equal)
			.blending(false)
			.stencil_mask(0x0)
			.stencil_func(Compare::always, 1, 0xFF);

		auto& shader = pimpl->shaders_resources.skybox_shader;

		shader.program->use();
		bind_texture_cubemap(&pimpl->states, shader.tex_skybox_uni, *world.skybox->cubemap);

		render_geom(*world.skybox->geom);
	}

	std::sort(
		transparent_meshes.begin(),
		transparent_meshes.end(),
		[](const auto& lhs, const auto& rhs) { return lhs.squared_distance_to_camera > rhs.squared_distance_to_camera; }
	);

	if (transparent_meshes.empty() == false)
	{
		SCOPED_DEBUG_GROUP("render transparent meshes"sv);
		for (auto& transparent_mesh: transparent_meshes)
		{
			const auto transparent_context = RenderContext{TransformSource::Uniform, UseTransparency::yes, settings.gamma};

			const auto& mesh = transparent_mesh.mesh;
			StateChanger{&pimpl->states}
				.depth_test(true)
				.depth_mask(true)
				.depth_func(Compare::less)
				.blending(true)
				.stencil_mask(0x0)
				.stencil_func(Compare::always, 1, 0xFF);

			if (mesh->outline)
			{
				StateChanger{&pimpl->states}.stencil_func(Compare::always, 1, 0xFF).stencil_mask(0xFF);
			}
			mesh->material->use_shader(transparent_context);
			mesh->material->set_uniforms(transparent_context, compiled_camera, calc_world_from_local(mesh, compiled_camera));
			mesh->material->bind_textures(transparent_context, &pimpl->states, &assets);
			mesh->material->apply_lights(transparent_context, world.lights, settings, &pimpl->states, &assets);

			render_geom(*mesh->geom);
		}
	}

	// render outline over all other meshes
	if (has_outlined_meshes)
	{
		SCOPED_DEBUG_GROUP("render outline meshes"sv);
		for (const auto& mesh: world.meshes)
		{
			if (const auto& mesh_outline = mesh->outline)
			{
				StateChanger{&pimpl->states}
					.stencil_func(Compare::not_equal, 1, 0xFF)
					.stencil_mask(0x00)
					.depth_test(false);
				const auto small_scale_mat = glm::scale(glm::mat4(1.0f), {OUTLINE_SCALE, OUTLINE_SCALE, OUTLINE_SCALE});

				auto& shader = pimpl->shaders_resources.single_color_shader;
				shader.program->use();
				shader.program->set_vec4(shader.tint_color_uni, {linear_from_srgb(*mesh_outline, settings.gamma), 1});

				shader.program->set_mat(shader.world_from_local_uni, calc_world_from_local(mesh, compiled_camera) * small_scale_mat);

				render_geom(*mesh->geom);
			}
		}
	}
}

}  //  namespace klotter
