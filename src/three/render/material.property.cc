#include "three/render/material.property.h"

#include "three/stdutils.h"


namespace render
{

bool operator<(const MaterialPropertyReference& lhs, const MaterialPropertyReference& rhs)
{
    if(lhs.type == rhs.type)
    {
        return lhs.index < rhs.index;
    }
    else
    {
        return base_cast(lhs.type) < base_cast(rhs.type);
    }
}

}

