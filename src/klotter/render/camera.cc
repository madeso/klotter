#include "klotter/render/camera.h"

#include <cmath>

namespace klotter
{

// todo(Gustav): replace create_view_mat when we have test rendering

// todo(Gustav): is this wrapper needed?
CameraVectors create_vectors(const Camera& cam)
{
	return create_vectors(cam.yaw, cam.pitch);
}

glm::mat4 create_view_from_world_mat(const glm::vec3& pos, const CameraVectors& cv)
{
	return glm::lookAt(pos, pos + cv.front, {0.0f, 1.0f, 0.0f});
}

CompiledCamera compile(const Camera& cam, const glm::ivec2& window_size)
{
	// todo(Gustav): use the near and far from the camera struct
	const float aspect = static_cast<float>(window_size.x) / static_cast<float>(window_size.y);
	const auto clip_from_view = glm::perspective(glm::radians(cam.fov), aspect, 0.1f, 100.0f);

	const auto camera_space = create_vectors(cam);
	const auto view_from_world = create_view_from_world_mat(cam.position, camera_space);

	return CompiledCamera{clip_from_view, view_from_world, cam.position, camera_space.front};
}

CompiledCamera compile(const OrthoCamera& cam, const glm::ivec2& window_size)
{
	const float aspect = static_cast<float>(window_size.x) / static_cast<float>(window_size.y);

	const float width = cam.size * aspect;
	const float height = cam.size;

	const auto clip_from_view = glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, cam.near, cam.far);

	const auto camera_space = create_vectors(cam.yaw, cam.pitch);
	const auto view_from_world = create_view_from_world_mat(cam.position, camera_space);

	return CompiledCamera{clip_from_view, view_from_world, cam.position, camera_space.front};
}

/** Convert from -1..1 to 0..1
 */
float r01_from_r11(float f)
{
	return (f + 1.0f) / 2.0f;
}

glm::vec2 screen_from_clip(const glm::vec2& resolution, const glm::vec4& clip_pos)
{
	const auto ndc = glm::vec3(clip_pos) / clip_pos.w;
	return glm::vec2{r01_from_r11(ndc.x), r01_from_r11(ndc.y)} * resolution;
}

glm::vec2 screen_from_world(const CompiledCamera& cam, const glm::vec3& world_pos, const glm::vec2& resolution)
{
	const auto clip_pos = cam.clip_from_view * cam.view_from_world * glm::vec4(world_pos, 1.0f);
	return screen_from_clip(resolution, clip_pos);
}

}  //  namespace klotter
