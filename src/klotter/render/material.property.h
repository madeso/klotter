#pragma once

namespace klotter
{

enum class MaterialPropertyType
{
    float_type, vec3_type, vec4_type, texture_type
};


enum class RenderMode
{
    fill, line, point
};



// reference a certain property for example "diffuse texture"
struct MaterialPropertyReference
{
    MaterialPropertyType type;
    int index;
};

bool operator<(const MaterialPropertyReference& lhs, const MaterialPropertyReference& rhs);

}

