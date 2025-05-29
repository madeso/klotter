#pragma once

namespace klotter
{

/** \addtogroup render
 *  @{
*/

constexpr auto UP = glm::vec3(0.0f, 1.0f, 0.0f);

/// A temporary view of the local space of a camera.
struct CameraVectors
{
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
};

/// The current camera representation.
struct Camera
{
	float fov = 45.0f;
	float near = 0.1f;
	float far = 100.0f;

	glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};

	float yaw = 0.0f;
	float pitch = 0.0f;
};

/// A "compiled" camera for use in rendering
/// @see compile_camera
struct CompiledCamera
{
	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 position;
	glm::vec3 in;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

/// Capture the current local space of the cammera.
CameraVectors create_vectors(const Camera& camera);

/// Capture the local space of a "camera" us ing only the rotation.
CameraVectors create_vectors(float yaw, float pitch);

// todo(Gustav): the meaning of this function is unclear... move closer to usage or remove?
glm::mat4 create_view_mat(const glm::vec3& pos, const CameraVectors& cv);

/// "Compile" a camera so it can be used in rendering.
CompiledCamera compile(const Camera&, const glm::ivec2 window_size);

// todo(Gustav): also include depth, what happens if the position is behind?
/// Calculate the screen coordinate of a 3d world position.
glm::vec2 to_screen(const CompiledCamera& cam, const glm::vec3& pos, const glm::vec2& resolution);

/**
@}
*/
}  //  namespace klotter
