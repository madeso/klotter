#include "klotter/render/shadow.h"

#include "klotter/render/world.h"
#include "klotter/render/camera.h"
#include "klotter/render/render_settings.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

namespace klotter
{
 
OrthoCamera shadow_cam_from_light(const DirectionalLight& light, const World& world, const Camera& camera)
{
	auto cam = OrthoCamera{};
	const auto light_local_space = create_vectors(light);
	const auto shadow_offset = light_local_space.front * world.lights.shadow_offset;
	cam.position = camera.position - shadow_offset;
	cam.pitch = light.pitch;
	cam.yaw = light.yaw;
	cam.near = world.lights.shadow_near;
	cam.far = world.lights.shadow_far;
	cam.size = world.lights.shadow_size;
	return cam;
}


using FrustumCorners = std::array<glm::vec3, 8>;
static FrustumCorners calculate_frustum_corners_in_world_space(const glm::mat4& world_from_clip)
{
	const auto calc_world_from_clip = [world_from_clip](float x, float y, float z)
	{
		const auto pt = world_from_clip * glm::vec4{x, y, z, 1.0f};
		return glm::vec3(pt) / pt.w;
	};

	constexpr float neg = -1.0f;
	constexpr float pos = 1.0f;

	return {
		calc_world_from_clip(neg, neg, neg),
		calc_world_from_clip(neg, neg, pos),
		calc_world_from_clip(neg, pos, neg),
		calc_world_from_clip(neg, pos, pos),
		calc_world_from_clip(pos, neg, neg),
		calc_world_from_clip(pos, neg, pos),
		calc_world_from_clip(pos, pos, neg),
		calc_world_from_clip(pos, pos, pos)
	};
}

static glm::vec3 calculate_frustum_center(const FrustumCorners& corners)
{
	glm::vec3 sum(0.0f);
	for (const auto& c: corners)
	{
		sum += c;
	}
	return sum / static_cast<float>(corners.size());
}

CompiledCamera calculate_tight_fitting_camera_around_perspective(
	const CompiledCamera& perspective, const glm::vec3& light_direction
)
{
	// algorithm inspired by learn open gl: https://learnopengl.com/Guest-Articles/2021/CSM

	const auto dir = glm::normalize(light_direction);

	const auto world_pos_frustum_corners = calculate_frustum_corners_in_world_space(
		glm::inverse(perspective.clip_from_view * perspective.view_from_world)
	);

	// calculate frustum center
	const auto world_pos_center = calculate_frustum_center(world_pos_frustum_corners);

	// calculate view matrix
	const auto world_pos_eye = world_pos_center - dir * 100.0f;
	constexpr auto y_up = glm::vec3(0, 1, 0);
	constexpr auto x_up = glm::vec3(1, 0, 0);
	const auto up = abs(glm::dot(y_up, dir)) > 0.99f ? x_up : y_up;
	const auto light_view_from_world = glm::lookAt(world_pos_eye, world_pos_center, up);

	// calculate aabb
	auto view_pos_min = glm::vec3{std::numeric_limits<float>::max()};
	auto view_pos_max = glm::vec3{std::numeric_limits<float>::lowest()}; // lowest() is the lowest negative, min() is the lowest positive
	for (const auto& world_pos: world_pos_frustum_corners)
	{
		const auto view_pos = glm::vec3(light_view_from_world * glm::vec4(world_pos, 1.0f));
		view_pos_min = glm::min(view_pos_min, view_pos);
		view_pos_max = glm::max(view_pos_max, view_pos);
	}

	// expand aabb (does this work???)
	constexpr float z_mult = 1.0f;	 // Tune this parameter according to the scene
	const auto temp_min_z = -view_pos_max.z;
	const auto temp_max_z = -view_pos_min.z;
	const auto min_z = temp_min_z < 0 ? temp_min_z * z_mult : temp_min_z / z_mult;
	const auto max_z = temp_max_z < 0 ? temp_max_z / z_mult : temp_max_z * z_mult;

	// calculate ortho projection
	const auto light_clip_from_view
		= glm::ortho(view_pos_min.x, view_pos_max.x, view_pos_min.y, view_pos_max.y, min_z, max_z);

	return {
		.clip_from_view = light_clip_from_view,
		.view_from_world = light_view_from_world,
		.position = world_pos_eye,
		.in = dir
	};
}


#if 0
struct Plane
{
	float a;
	float b;
	float c;
	float d;
};

Plane plane_from_point_normal(const glm::vec3& point, const glm::vec3& normal)
{
	const auto normalized_normal = glm::normalize(normal);
	return {
		.a = normalized_normal.x,
		.b = normalized_normal.y,
		.c = normalized_normal.z,
		.d = -glm::dot(point, normalized_normal)
	};
}

float signed_distance(const Plane& plane, const glm::vec3& Pt)
{
	return plane.a * Pt.x + plane.b * Pt.y + plane.c * Pt.z + plane.d;
}

CompiledCamera calculate_tight_fitting_camera_around_perspective(
	const CompiledCamera& perspective, const CameraVectors& light
)
{
	const auto world_pos_frustum_corners = calculate_frustum_corners_in_world_space(
		glm::inverse(perspective.clip_from_view * perspective.view_from_world)
	);

	const auto center = calculate_frustum_center(world_pos_frustum_corners);

	const auto near_and_far = [](const FrustumCorners& corners, const glm::vec3& p, const glm::vec3& dir)
	{
		const auto plane = plane_from_point_normal(p, dir);
		return std::ranges::minmax_element(
			corners,
			[plane](const auto& lhs, const auto& rhs)
			{ return signed_distance(plane, lhs) < signed_distance(plane, rhs); }
		);
	};

	// calculate nearest and fartest point based on distance from plane, using [0] and In
	const auto [near_point, far_point] = near_and_far(world_pos_frustum_corners, center, light.front);

	// calculate min/max aabb in "camera space" using distance and in/up
	const auto [left_point, right_point] = near_and_far(world_pos_frustum_corners, center, light.right);
	const auto [down_point, up_point] = near_and_far(world_pos_frustum_corners, center, light.up);

	
	// calculate ortho from planes
}
#endif

CompiledCamera compile_the_shadow_camera(
	const Camera& camera,
	const glm::ivec2& window_size,
	const DirectionalLight& light,
	const RenderSettings& settings,
	const World& world
)
{
	if (settings.use_tight_fit_shadows)
	{
		return calculate_tight_fitting_camera_around_perspective(
			compile(camera, window_size), create_vectors(light).front
		);
	}
	else
	{
		const auto cam = shadow_cam_from_light(light, world, camera);
		return compile(cam, settings.shadow_map_resolution);
	}
}




}
