#include "three/render/camera.h"

#include <cmath>

namespace three
{

Camera::Camera(float a_fov, float a_near, float a_far)
    : fov(a_fov)
    , near(a_near)
    , far(a_far)
{
}

CameraVectors
create_vectors(const Camera& cam)
{
    const auto direction = glm::vec3
    {
        std::cos(glm::radians(cam.yaw)) * std::cos(glm::radians(cam.pitch)),
        std::sin(glm::radians(cam.pitch)),
        std::sin(glm::radians(cam.yaw)) * std::cos(glm::radians(cam.pitch))
    };
    const auto front = glm::normalize(direction);
    const auto right = glm::normalize(glm::cross(front, UP));
    const auto up = glm::normalize(glm::cross(right, front));

    return {front, right, up};
}

}
