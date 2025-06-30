#include "klotter/render/camera.h"

#include <cmath>

namespace klotter
{

// todo(Gustav): replace create_vectors and create_view_mat when we have test rendering

CameraVectors create_vectors(float yaw, float pitch)
{
	const auto direction = glm::vec3{
		std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
		std::sin(glm::radians(pitch)),
		std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
	};
	const auto front = glm::normalize(direction);
	const auto right = glm::normalize(glm::cross(front, UP));
	const auto up = glm::normalize(glm::cross(right, front));

	return {front, right, up};
}

CameraVectors create_vectors(const Camera& cam)
{
	return create_vectors(cam.yaw, cam.pitch);
}

glm::mat4 create_view_mat(const glm::vec3& pos, const CameraVectors& cv)
{
	return glm::lookAt(pos, pos + cv.front, {0.0f, 1.0f, 0.0f});
}

CompiledCamera compile(const Camera& cam, const glm::ivec2& window_size)
{
	const float aspect = static_cast<float>(window_size.x) / static_cast<float>(window_size.y);
	const auto projection = glm::perspective(glm::radians(cam.fov), aspect, 0.1f, 100.0f);

	const auto camera_space = create_vectors(cam);
	const auto view = create_view_mat(cam.position, camera_space);

	return CompiledCamera{projection, view, cam.position, camera_space.front};
}

glm::vec2 to_screen(const CompiledCamera& cam, const glm::vec3& pos, const glm::vec2& resolution)
{
	const auto clip = cam.projection * cam.view * glm::vec4(pos, 1.0f);
	const auto ndc = glm::vec3(clip) / clip.w;
	const auto c = [](float f)
	{
		return (f + 1.0f) / 2.0f;
	};
	return glm::vec2{c(ndc.x), c(ndc.y)} * resolution;
}

}  //  namespace klotter
