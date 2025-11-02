#pragma once

#include "klotter/render/space.h"

namespace klotter
{

/** \addtogroup render
 *  @{
*/

// todo(Gustav): rename to PerspectiveCamera? or PerspCamera?
/// The current perspective camera representation.
struct Camera
{
	float fov = 45.0f;
	float near = 0.1f;
	float far = 100.0f;

	glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};

	float yaw = 0.0f;
	float pitch = 0.0f;
};

/// A orthographic camera representation.
struct OrthoCamera
{
	float size = 100.0f;

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
	/// also known as projection matrix
	glm::mat4 clip_from_view;

	/// also known as view matrix
	glm::mat4 view_from_world;
	glm::vec3 position;
	glm::vec3 in;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

/// Capture the current local space of the camera.
CameraVectors create_vectors(const Camera& camera);

// todo(Gustav): the meaning of this function is unclear... move closer to usage or remove?
glm::mat4 create_view_from_world_mat(const glm::vec3& pos, const CameraVectors& cv);

/// "Compile" a camera to a perspective so it can be used in rendering.
CompiledCamera compile(const Camera&, const glm::ivec2& window_size);

/// "Compile" a camera to an orthographic view so it can be used in rendering.
CompiledCamera compile(const OrthoCamera&, const glm::ivec2& window_size);

// todo(Gustav): also include depth, what happens if the position is behind?
/// Calculate the screen coordinate of a 3d world position.
glm::vec2 screen_from_world(const CompiledCamera& cam, const glm::vec3& world_pos, const glm::vec2& resolution);

/**
@}
*/
}  //  namespace klotter
