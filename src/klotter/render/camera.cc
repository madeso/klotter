#include "klotter/render/camera.h"

#include <cmath>

namespace klotter
{


CameraVectors create_vectors(const Camera& cam)
{
	const auto direction = glm::vec3{
		std::cos(glm::radians(cam.yaw)) * std::cos(glm::radians(cam.pitch)),
		std::sin(glm::radians(cam.pitch)),
		std::sin(glm::radians(cam.yaw)) * std::cos(glm::radians(cam.pitch))};
	const auto front = glm::normalize(direction);
	const auto right = glm::normalize(glm::cross(front, UP));
	const auto up = glm::normalize(glm::cross(right, front));

	return {front, right, up};
}

CompiledCamera compile(const Camera& cam, const glm::ivec2 window_size)
{
	const float aspect = static_cast<float>(window_size.x) / static_cast<float>(window_size.y);
	const auto projection = glm::perspective(glm::radians(cam.fov), aspect, 0.1f, 100.0f);

	const auto camera_space = create_vectors(cam);
	const auto view
		= glm::lookAt(cam.position, cam.position + camera_space.front, {0.0f, 1.0f, 0.0f});

	return CompiledCamera{projection, view, cam.position};
}

}  //  namespace klotter
