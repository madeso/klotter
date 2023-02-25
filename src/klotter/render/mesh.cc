#include "klotter/render/mesh.h"

#include "klotter/assert.h"


namespace klotter
{


Vertex::Vertex
(
    const glm::vec3& p,
    const glm::vec3& n,
    const glm::vec2& t,
    const glm::vec4& c
)
    : position(p)
    , normal(n)
    , texture(t)
    , color(c)
{
}


Geom::Geom(const std::vector<Vertex>& verts, const std::vector<unsigned int>& tris)
    : vertices(verts)
    , triangles(tris)
{
    ASSERT(triangles.size() % 3 == 0);
}


Geom
create_plane_mesh()
{
    // todo(Gustav): remove
    return
    {
        {
            {{ 0.5f, -0.5f, 0.0f}, {0, 1, 0}, {1, 0}},
            {{ 0.5f,  0.5f, 0.0f}, {0, 1, 0}, {1, 1}, {0.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.0f}, {0, 1, 0}, {0, 0}},
            {{-0.5f,  0.5f, 0.0f}, {0, 1, 0}, {0, 1}}
        },
        {
            0, 1, 3,
            1, 2, 3
        }
    };
}


Geom
create_plane_mesh(float size, float uv)
{
    // todo(Gustav): optimize vertices

    const auto h = size / 2.0f;
    return
    {
        {
            {{-h,  0.0f, -h}, { 0.0f,  1.0f,  0.0f}, { 0.0f,   uv}},
            {{ h,  0.0f,  h}, { 0.0f,  1.0f,  0.0f}, {   uv, 0.0f}},
            {{ h,  0.0f, -h}, { 0.0f,  1.0f,  0.0f}, {   uv,   uv}},

            {{ h,  0.0f,  h}, { 0.0f,  1.0f,  0.0f}, {   uv, 0.0f}},
            {{-h,  0.0f, -h}, { 0.0f,  1.0f,  0.0f}, { 0.0f,   uv}},
            {{-h,  0.0f,  h}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 0.0f}}
        },
        {
             0,  1,  2,    3,  4,  5
        }
    };
}


Geom
create_box_mesh(float size)
{
    return create_box_mesh(size, size, size);
}


Geom
create_box_mesh(float x, float y, float z)
{
    const auto hx = x / 2.0f;
    const auto hy = y / 2.0f;
    const auto hz = z / 2.0f;

    // vertex indices
    enum
    {
        v00, v01, v02, v03,
        v10, v11, v12, v13,
        v20, v21, v22, v23,
        v30, v31, v32, v33,
        v40, v41, v42, v43,
        v50, v51, v52, v53
    };

    return
    {
        {
            {{-hx,  hy, -hz}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 1.0f}},
            {{ hx,  hy, -hz}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 1.0f}},
            {{ hx, -hy, -hz}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 0.0f}},
            {{-hx, -hy, -hz}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 0.0f}},

            {{-hx, -hy,  hz}, { 0.0f,  0.0f, 1.0f}, { 0.0f, 0.0f}},
            {{ hx, -hy,  hz}, { 0.0f,  0.0f, 1.0f}, { 1.0f, 0.0f}},
            {{ hx,  hy,  hz}, { 0.0f,  0.0f, 1.0f}, { 1.0f, 1.0f}},
            {{-hx,  hy,  hz}, { 0.0f,  0.0f, 1.0f}, { 0.0f, 1.0f}},

            {{-hx,  hy,  hz}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}},
            {{-hx,  hy, -hz}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f}},
            {{-hx, -hy, -hz}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}},
            {{-hx, -hy,  hz}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f}},

            {{ hx, -hy,  hz}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f}},
            {{ hx, -hy, -hz}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}},
            {{ hx,  hy, -hz}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f}},
            {{ hx,  hy,  hz}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}},

            {{-hx, -hy, -hz}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 1.0f}},
            {{ hx, -hy, -hz}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 1.0f}},
            {{ hx, -hy,  hz}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 0.0f}},
            {{-hx, -hy,  hz}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 0.0f}},

            {{-hx,  hy,  hz}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 0.0f}},
            {{ hx,  hy,  hz}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 0.0f}},
            {{ hx,  hy, -hz}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 1.0f}},
            {{-hx,  hy, -hz}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 1.0f}},
        },
        {
            v00, v01, v02,    v02, v03, v00,
            v10, v11, v12,    v12, v13, v10,
            v20, v21, v22,    v22, v23, v20,
            v30, v31, v32,    v32, v33, v30,
            v40, v41, v42,    v42, v43, v40,
            v50, v51, v52,    v52, v53, v50
        }
    };
}

}
