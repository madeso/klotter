#pragma once

#include "three/colors.h"

namespace three
{


struct Attenuation
{
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};


struct DirectionalLight
{
    glm::vec3 direction = glm::vec3{0.0f, 1.0f, 0.0f};

    float ambient_strength = 0.1f;
    
    glm::vec3 ambient = white3;
    glm::vec3 diffuse = white3;
    glm::vec3 specular = white3;
};


struct PointLight
{
    Attenuation attenuation;

    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  white3;
    glm::vec3 diffuse =  white3;
    glm::vec3 specular = white3;
};


struct SpotLight
{
    Attenuation attenuation;

    glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f};
    glm::vec3 direction = glm::vec3{1.0f, 0.0f, 0.0f};
    float cutoff = 12.5f;
    float outer_cutoff = 17.5;

    float ambient_strength = 0.1f;
    glm::vec3 ambient =  white3;
    glm::vec3 diffuse =  white3;
    glm::vec3 specular = white3;
};

}
