#include "klotter/render/mesh.builder.h"

#include "klotter/cint.h"

#include "klotter/render/render.h"

#include <map>


namespace klotter::mesh
{



Builder create_box(float w, float h, float d, bool face_out)
{
    Builder b;

    const Index t0 = b.add_text_coord(glm::vec2(0, 1));
    const Index t1 = b.add_text_coord(glm::vec2(1, 1));
    const Index t2 = b.add_text_coord(glm::vec2(0, 0));
    const Index t3 = b.add_text_coord(glm::vec2(1, 0));

    // front side
    const Index v0 = b.add_position(glm::vec3(0, 0, 0));
    const Index v1 = b.add_position(glm::vec3(w, 0, 0));
    const Index v2 = b.add_position(glm::vec3(0, h, 0));
    const Index v3 = b.add_position(glm::vec3(w, h, 0));

    // back side
    const Index v4 = b.add_position(glm::vec3(0, 0, d));
    const Index v5 = b.add_position(glm::vec3(w, 0, d));
    const Index v6 = b.add_position(glm::vec3(0, h, d));
    const Index v7 = b.add_position(glm::vec3(w, h, d));

    const auto v = [](Index p, Index t) -> Vertex { return { p, 0, t }; };
    b.add_quad(!face_out, v(v0, t2), v(v2, t0), v(v3, t1), v(v1, t3));  // front
    b.add_quad(!face_out, v(v1, t3), v(v3, t1), v(v7, t0), v(v5, t2));  // right
    b.add_quad(!face_out, v(v4, t3), v(v6, t1), v(v2, t0), v(v0, t2));  // left
    b.add_quad(!face_out, v(v5, t2), v(v7, t0), v(v6, t1), v(v4, t3));  // back
    b.add_quad(!face_out, v(v3, t1), v(v2, t3), v(v6, t2), v(v7, t0));  // up
    b.add_quad(!face_out, v(v4, t0), v(v0, t1), v(v1, t3), v(v5, t2));  // bottom
    return b;
}

// ==================================================================================================================================

Vertex::Vertex(
        Index a_position,
        Index a_normal,
        Index a_texture)
    : position(a_position)
    , texture(a_texture)
    , normal(a_normal)
{
}

Triangle::Triangle(Vertex a, Vertex b, Vertex c) : v0(a), v1(b), v2(c)
{
}

// ==================================================================================================================================

Index Builder::add_text_coord(const glm::vec2& tc)
{
    texcoords.push_back(tc);
    return static_cast<Index>(texcoords.size() - 1);
}

Index Builder::add_position(const glm::vec3& pos)
{
    positions.push_back(pos);
    return static_cast<Index>(positions.size() - 1);
}

Index Builder::add_normal(const glm::vec3& norm)
{
    normals.push_back(norm);
    return static_cast<Index>(normals.size() - 1);
}

Builder& Builder::add_triangle(const Triangle& t)
{
    triangles.emplace_back(t);
    return *this;
}

Builder& Builder::add_quad
(
    bool reverse,
    const Vertex& v0,
    const Vertex& v1,
    const Vertex& v2,
    const Vertex& v3
)
{
    if (reverse)
    {
        add_triangle(Triangle(v0, v1, v2));
        add_triangle(Triangle(v0, v2, v3));
    }
    else
    {
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
    bool added = false;
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

    Combo(const Vertex& v)
        : position(v.position)
        , texture(v.texture)
        , normal(v.normal)
    {
    }
};

bool operator<(const Combo& lhs, const Combo& rhs)
{
    if (lhs.position != rhs.position)
    {
        return lhs.position < rhs.position;
    }
    else if (lhs.texture != rhs.texture)
    {
        return lhs.texture < rhs.texture;
    }
    else if (lhs.normal != rhs.normal)
    {
        return lhs.normal < rhs.normal;
    }
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
            const glm::vec3 normal = normals.empty() == false
                ? normals[c.normal]
                : glm::vec3(0, 0, 0);
            const auto ind = vertices.size();
            vertices.emplace_back(pos, text);
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

