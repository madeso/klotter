#pragma once

namespace klotter
{

constexpr auto UP = glm::vec3(0.0f, 1.0f, 0.0f);

struct CameraVectors
{
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
};


struct Camera
{
    float fov = 45.0f;
    float near = 0.1f;
    float far = 100.0f;

    glm::vec3 position = glm::vec3{0.0f, 0.0f,  0.0f};

    float yaw = 0.0f;
    float pitch = 0.0f;
};


struct CompiledCamera
{
    glm::mat4 projection;
    glm::mat4 view;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

CameraVectors create_vectors(const Camera& camera);
CompiledCamera compile(const Camera&, const glm::ivec2 window_size);

}
