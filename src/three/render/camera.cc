#include "three/render/camera.h"

#include <cmath>

namespace render
{

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
