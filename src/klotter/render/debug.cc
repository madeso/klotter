#include "klotter/render/debug.h"

#include "klotter/render/camera.h"

namespace klotter
{

void DebugRender::add_line(const glm::vec3& from, const glm::vec3& to, const Rgb& color)
{
	lines.emplace_back(DebugLine{from, to, color});
}

glm::vec3 world_from_ndc(const CompiledCamera& camera, const glm::vec3& ndc)
{
	const auto clip_space_pos = glm::vec4{ndc.x, ndc.y, ndc.z, 1.0f};
	const auto view_from_clip = glm::inverse(camera.clip_from_view);
	const auto view_space_pos = view_from_clip * clip_space_pos;
	const auto view_space_pos_corrected = view_space_pos / view_space_pos.w;
	const auto world_from_view = glm::inverse(camera.view_from_world);
	const auto world_space_pos = world_from_view * view_space_pos_corrected;
	const auto out_world = glm::vec3{world_space_pos.x, world_space_pos.y, world_space_pos.z};
	return out_world;
}

void draw_frustum(DebugRender* debug, const CompiledCamera& camera, const Rgb& color)
{
	const auto near_bot_left = world_from_ndc(camera, {-1.0f, -1.0f, -1.0f});
	const auto near_bot_right = world_from_ndc(camera, {+1.0f, -1.0f, -1.0f});
	const auto near_top_left = world_from_ndc(camera, {-1.0f, +1.0f, -1.0f});
	const auto near_top_right = world_from_ndc(camera, {+1.0f, +1.0f, -1.0f});
	const auto far_bot_left = world_from_ndc(camera, {-1.0f, -1.0f, +1.0f});
	const auto far_bot_right = world_from_ndc(camera, {+1.0f, -1.0f, +1.0f});
	const auto far_top_left = world_from_ndc(camera, {-1.0f, +1.0f, +1.0f});
	const auto far_top_right = world_from_ndc(camera, {+1.0f, +1.0f, +1.0f});

	debug->add_line(camera.position, near_bot_left, color);
	debug->add_line(camera.position, near_top_left, color);
	debug->add_line(camera.position, near_bot_right, color);
	debug->add_line(camera.position, near_top_right, color);
	debug->add_line(near_bot_left, near_bot_right, color);
	debug->add_line(near_bot_right, near_top_right, color);
	debug->add_line(near_top_right, near_top_left, color);
	debug->add_line(near_top_left, near_bot_left, color);
	debug->add_line(far_bot_left, far_bot_right, color);
	debug->add_line(far_bot_right, far_top_right, color);
	debug->add_line(far_top_right, far_top_left, color);
	debug->add_line(far_top_left, far_bot_left, color);
	debug->add_line(near_bot_left, far_bot_left, color);
	debug->add_line(near_bot_right, far_bot_right, color);
	debug->add_line(near_top_left, far_top_left, color);
	debug->add_line(near_top_right, far_top_right, color);
}


}
