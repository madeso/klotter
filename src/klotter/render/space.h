#pragma once

namespace klotter
{

constexpr auto UP = glm::vec3(0.0f, 1.0f, 0.0f);

/// A temporary view of the local space of a camera.
struct CameraVectors
{
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
};

/// Capture the local space of a "camera" us ing only the rotation.
CameraVectors create_vectors(float yaw, float pitch);

}