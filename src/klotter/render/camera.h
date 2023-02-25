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
    float fov;
    float near;
    float far;

    Camera(float a_fov, float a_near, float a_far);

    glm::vec3 position = glm::vec3{0.0f, 0.0f,  0.0f};

    float yaw = -90.0f;
    float pitch = 0.0f;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

CameraVectors create_vectors(const Camera& camera);

}
