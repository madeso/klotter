#include "klotter/render/mesh.builder.h"

#include "klotter/cint.h"

#include "klotter/render/render.h"

#include <map>


namespace klotter::mesh
{



Builder create_box(float x, float y, float z, bool face_out)
{
    Builder b;

    const auto v = [&]
    (
        glm::vec3 color,
        glm::vec3 p0, glm::vec2 t0,
        glm::vec3 p1, glm::vec2 t1,
        glm::vec3 p2, glm::vec2 t2,
        glm::vec3 p3, glm::vec2 t3
    )
    {
        constexpr float pd = 0.1f;
        constexpr float td = 0.01f;
        const auto ci = b.foa_color({ color, 1.0f }, 0.001f);
        const auto v0 = Vertex{ b.foa_position(p0, pd), 0, b.foa_text_coord(t0, td), ci };
        const auto v1 = Vertex{ b.foa_position(p1, pd), 0, b.foa_text_coord(t1, td), ci };
        const auto v2 = Vertex{ b.foa_position(p2, pd), 0, b.foa_text_coord(t2, td), ci };
        const auto v3 = Vertex{ b.foa_position(p3, pd), 0, b.foa_text_coord(t3, td), ci };

        b.add_quad(face_out, v0, v1, v2, v3);
    };

    // texel scale
    const float ts = 1.0f;
    
    // half sizes
    const float hx = x * 0.5f;
    const float hy = y * 0.5f;
    const float hz = z * 0.5f;

    // front
    v(colors::white,
        {-hx, -hy, -hz}, { 0.0f, 0.0f},
        { hx, -hy, -hz}, { x*ts, 0.0f},
        { hx,  hy, -hz}, { x*ts, y*ts},
        {-hx,  hy, -hz}, { 0.0f, y*ts});

    // back
    v(colors::white,
        {-hx, -hy,  hz}, { 0.0f, 0.0f},
        {-hx,  hy,  hz}, { 0.0f, y*ts},
        { hx,  hy,  hz}, { x*ts, y*ts},
        { hx, -hy,  hz}, { x*ts, 0.0f});

    // left
    v(colors::white,
        {-hx,  hy, -hz}, { y*ts, 0.0f},
        {-hx,  hy,  hz}, { y*ts, z*ts},
        {-hx, -hy,  hz}, { 0.0f, z*ts},
        {-hx, -hy, -hz}, { 0.0f, 0.0f});

    // right
    v(colors::white,
        { hx,  hy,  hz}, { y*ts, z*ts},
        { hx,  hy, -hz}, { 0.0f, z*ts},
        { hx, -hy, -hz}, { 0.0f, 0.0f},
        { hx, -hy,  hz}, { y*ts, 0.0f});

    // bottom
    v(colors::white,
        {-hx, -hy, -hz}, { 0.0f, 0.0f},
        {-hx, -hy,  hz}, { 0.0f, z*ts},
        { hx, -hy,  hz}, { x*ts, z*ts},
        { hx, -hy, -hz}, { x*ts, 0.0f});

    // top
    v(colors::white,
        {-hx,  hy, -hz}, { 0.0f, 0.0f},
        { hx,  hy, -hz}, { x*ts, 0.0f},
        { hx,  hy,  hz}, { x*ts, z*ts},
        {-hx,  hy,  hz}, { 0.0f, z*ts});

    return b;
}

// ==================================================================================================================================

Vertex::Vertex
(
    Index a_position,
    Index a_normal,
    Index a_texture,
    Index a_color
)
    : position(a_position)
    , normal(a_normal)
    , texture(a_texture)
    , color(a_color)
{
}

Triangle::Triangle(Vertex a, Vertex b, Vertex c) : v0(a), v1(b), v2(c)
{
}

// ==================================================================================================================================

namespace
{
    template<typename T>
    Index add_vec(std::vector<T>* v, T t)
    {
        v->emplace_back(t);
        return v->size() - 1;
    }

    template<typename T>
    std::optional<Index> find_vec(const std::vector<T>& v, T t, float diff)
    {
        const auto d2 = diff * diff;
        Index index = 0;
        for (const auto& r : v)
        {
            if (glm::distance2(r, t) < d2)
            {
                return index;
            }
            index += 1;
        }
        
        return std::nullopt;
    }

    template<typename T>
    Index find_or_add_vec(std::vector<T>* v, T t, float diff)
    {
        const auto found = find_vec(*v, t, diff);
        if (found) { return *found; }

        return add_vec(v, t);
    }
}

Index Builder::add_text_coord(const glm::vec2& tc)
    { return add_vec(&texcoords, tc); }
Index Builder::add_position(const glm::vec3& pos)
    { return add_vec(&positions, pos); }
Index Builder::add_normal(const glm::vec3& norm)
    { return add_vec(&normals, norm); }
Index Builder::add_color(const glm::vec4& c)
    { return add_vec(&colors, c); }

Index Builder::foa_text_coord(const glm::vec2& v, float max_diff)
    { return find_or_add_vec(&texcoords, v, max_diff); }
Index Builder::foa_position(const glm::vec3& pos, float max_diff)
    { return find_or_add_vec(&positions, pos, max_diff); }
Index Builder::foa_normal(const glm::vec3& norm, float max_diff)
    { return find_or_add_vec(&normals, norm, max_diff); }
Index Builder::foa_color(const glm::vec4& c, float max_diff)
    { return find_or_add_vec(&colors, c, max_diff); }


Builder& Builder::add_triangle(const Triangle& t)
{
    triangles.emplace_back(t);
    return *this;
}

Builder& Builder::add_quad
(
    bool ccw,
    const Vertex& v0,
    const Vertex& v1,
    const Vertex& v2,
    const Vertex& v3
)
{
    if (ccw)
    {
        // add counter clock wise
        add_triangle(Triangle(v0, v1, v2));
        add_triangle(Triangle(v0, v2, v3));
    }
    else
    {
        // add clock wise
        add_triangle(Triangle(v2, v1, v0));
        add_triangle(Triangle(v3, v2, v0));
    }

    return *this;
}

Builder& Builder::move(const glm::vec3& dir)
{
    for (glm::vec3& p : positions)
    {
        p += dir;
    }

    return *this;
}

Builder& Builder::scale(float scale)
{
    for (glm::vec3& p : positions)
    {
        p *= scale;
    }

    return *this;
}

Builder& Builder::invert_normals()
{
    for (glm::vec3& p : normals)
    {
        p = -p;
    }

    return *this;
}

Builder& Builder::add_face(const std::vector<Vertex>& vertices)
{
    // we currently doesnt support ton-triangular faces so - triangulate it
    const std::vector<Vertex>::size_type size = vertices.size();
    [[maybe_unused]] bool added = false;
    for (std::vector<Vertex>::size_type i = 2; i < size; ++i)
    {
        add_triangle(Triangle(vertices[0], vertices[i - 1], vertices[i]));
        added = true;
    }
    assert(added);

    return *this;
}

glm::vec3 FromTo(const glm::vec3& f, const glm::vec3& t)
{
    return t - f;
}

glm::vec3 crossNorm(const glm::vec3& lhs, const glm::vec3& rhs)
{
    return glm::normalize(glm::cross(lhs, rhs));
}


Builder& Builder::replace_with_smooth_normals()
{
    // start with empty sum, this will become the smooth normals
    std::vector<glm::vec3> vertexNormalsSum(positions.size(), glm::vec3(0, 0, 0));


    for(Triangle& t: triangles)
    {
        const glm::vec3 p0 = positions[t.v0.position];
        const glm::vec3 p1 = positions[t.v1.position];
        const glm::vec3 p2 = positions[t.v2.position];

        const glm::vec3 d0 = FromTo(p1, p0);
        const glm::vec3 d1 = FromTo(p1, p2);

        const glm::vec3 faceNormal = crossNorm(d1, d0);


        auto update_vert = [&](Vertex& vert)
        {
            // add face normal to the vertex normal
            vertexNormalsSum[vert.position] += faceNormal;

            // set the vertex normal to the same index as the position
            vert.normal = vert.position;
        };

        update_vert(t.v0);
        update_vert(t.v1);
        update_vert(t.v2);
    }

    //  normalize the sums
    normals.clear();
    for(const glm::vec3& normalSum: vertexNormalsSum)
    {
        normals.push_back(glm::normalize(normalSum));
    }
    assert(normals.size() == positions.size());

    return *this;
}


struct Combo
{
    Index position;
    Index texture;
    Index normal;
    Index color;

    Combo(const Vertex& v)
        : position(v.position)
        , texture(v.texture)
        , normal(v.normal)
        , color(v.color)
    {
    }
};

bool operator<(const Combo& lhs, const Combo& rhs)
{
#define SORT_ON(NAME) if (lhs.NAME != rhs.NAME) { return lhs.NAME < rhs.NAME; }
    SORT_ON(position)
    else SORT_ON(texture)
    else SORT_ON(normal)
    else SORT_ON(color)
#undef SORT_ON
    else
    {
        // all equal
        return false;
    }
}


Mesh Builder::to_mesh() const
{
    std::map<Combo, u32> combinations;

    // foreach triangle
    std::vector<klotter::Vertex> vertices;
    std::vector<klotter::Face> faces;

    auto convert_vert = [&](const Vertex& vert) -> u32
    {
        const Combo c(vert);
        auto result = combinations.find(c);
        if (result != combinations.end())
        {
            return result->second;
        }
        else
        {
            const glm::vec3 pos = positions[c.position];
            const glm::vec2 text = texcoords.empty()
                ? glm::vec2(0, 0)
                : texcoords[c.texture];
            const glm::vec4 col = colors.empty()
                ? glm::vec4{colors::white, 1.0f}
                : colors[c.color];
            // const glm::vec3 normal = normals.empty() == false
            //    ? normals[c.normal]
            //    : glm::vec3(0, 0, 0);
            const auto ind = vertices.size();
            vertices.emplace_back(pos, text, col);
            combinations.insert({ c,  Csizet_to_u32(ind) });
            return Csizet_to_u32(ind);
        }
    };

    for(const Triangle& tri: triangles)
    {
        const auto v0 = convert_vert(tri.v0);
        const auto v1 = convert_vert(tri.v1);
        const auto v2 = convert_vert(tri.v2);

        // add traingle to mesh
        faces.emplace_back(Face{v0, v1, v2});
    }

    return Mesh(std::move(vertices), std::move(faces));
}






}

