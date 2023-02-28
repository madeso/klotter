#include "klotter/render/mesh.builder.h"

#include "klotter/cint.h"

#include "klotter/render/render.h"

#include <map>


namespace klotter::mesh
{





Builder*
Move(Builder* mesh, const glm::vec3& dir)
{
    for(glm::vec3& p: mesh->positions)
    {
        p += dir;
    }

    return mesh;
}

Builder*
Scale(Builder* mesh, float scale)
{
    for(glm::vec3& p: mesh->positions)
    {
        p *= scale;
    }

    return mesh;
}

Builder*
InvertNormals(Builder* mesh)
{
    for(glm::vec3& p: mesh->normals)
    {
        p = -p;
    }

    return mesh;
}


namespace  // local
{
    Vertex
    v(Index p, Index t)
    {
        Vertex vt;
        vt.position = p;
        vt.texture = t;
        return vt;
    }
}

Builder
CreateBox(float w, float h, float d, bool faceOut)
{
    Builder b;

    const Index t0 = b.addTextCoord(glm::vec2(0, 1));
    const Index t1 = b.addTextCoord(glm::vec2(1, 1));
    const Index t2 = b.addTextCoord(glm::vec2(0, 0));
    const Index t3 = b.addTextCoord(glm::vec2(1, 0));

    // front side
    const Index v0 = b.addPosition(glm::vec3(0, 0, 0));
    const Index v1 = b.addPosition(glm::vec3(w, 0, 0));
    const Index v2 = b.addPosition(glm::vec3(0, h, 0));
    const Index v3 = b.addPosition(glm::vec3(w, h, 0));

    // back side
    const Index v4 = b.addPosition(glm::vec3(0, 0, d));
    const Index v5 = b.addPosition(glm::vec3(w, 0, d));
    const Index v6 = b.addPosition(glm::vec3(0, h, d));
    const Index v7 = b.addPosition(glm::vec3(w, h, d));

    b.addQuad(!faceOut, v(v0, t2), v(v2, t0), v(v3, t1), v(v1, t3));  // front
    b.addQuad(!faceOut, v(v1, t3), v(v3, t1), v(v7, t0), v(v5, t2));  // right
    b.addQuad(!faceOut, v(v4, t3), v(v6, t1), v(v2, t0), v(v0, t2));  // left
    b.addQuad(!faceOut, v(v5, t2), v(v7, t0), v(v6, t1), v(v4, t3));  // back
    b.addQuad(!faceOut, v(v3, t1), v(v2, t3), v(v6, t2), v(v7, t0));  // up
    b.addQuad(!faceOut, v(v4, t0), v(v0, t1), v(v1, t3), v(v5, t2));  // bottom
    return b;
}

// ==================================================================================================================================

Vertex::Vertex() : position(0), texture(0), normal(0)
{
}

Vertex::Vertex(
        Index vertex,
        Index anormal,
        Index textureCoordinate)
    : position(vertex)
    , texture(textureCoordinate)
    , normal(anormal)
{
}

Vertex
Vertex::Create_VN_T(Index vertexAndNormal, Index text)
{
    return Vertex(vertexAndNormal, vertexAndNormal, text);
}

Triangle::Triangle(Vertex a, Vertex b, Vertex c) : v0(a), v1(b), v2(c)
{
}

Triangle::Triangle()
{
}

namespace  // local
{
    Vertex Triangle::*const offsets[3] = {
            &Triangle::v0,
            &Triangle::v1,
            &Triangle::v2};
}

Vertex&
Triangle::operator[](Index index)
{
    return this->*offsets[index];
}

const Vertex&
Triangle::operator[](Index index) const
{
    return this->*offsets[index];
}

// ==================================================================================================================================

Index Builder::addTextCoord(const glm::vec2& tc)
{
    texcoords.push_back(tc);
    return static_cast<Index>(texcoords.size() - 1);
}

Index Builder::addPosition(const glm::vec3& pos)
{
    positions.push_back(pos);
    return static_cast<Index>(positions.size() - 1);
}

Index Builder::addNormal(const glm::vec3& norm)
{
    normals.push_back(norm);
    return static_cast<Index>(normals.size() - 1);
}

void Builder::addTriangle(const Triangle& t)
{
    triangles.emplace_back(t);
}

void Builder::addQuad
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
        addTriangle(Triangle(v0, v1, v2));
        addTriangle(Triangle(v0, v2, v3));
    }
    else
    {
        addTriangle(Triangle(v2, v1, v0));
        addTriangle(Triangle(v3, v2, v0));
    }
}

void Builder::addFace(const std::vector<Vertex>& vertices)
{
    // we currently doesnt support ton-triangular faces so - triangulate it
    const std::vector<Vertex>::size_type size = vertices.size();
    bool added = false;
    for (std::vector<Vertex>::size_type i = 2; i < size; ++i)
    {
        addTriangle(Triangle(vertices[0], vertices[i - 1], vertices[i]));
        added = true;
    }
    if (false == added)
    {
        throw "Unable to triangulate face";
    }
}

glm::vec3 FromTo(const glm::vec3& f, const glm::vec3& t)
{
    return t - f;
}

glm::vec3 crossNorm(const glm::vec3& lhs, const glm::vec3& rhs)
{
    return glm::normalize(glm::cross(lhs, rhs));
}


void Builder::buildNormals()
{
    std::vector<glm::vec3> vertexNormalsSum(positions.size(), glm::vec3(0, 0, 0));
    for(Triangle& t: triangles)
    {
        const glm::vec3 p0 = positions[t[0].position];
        const glm::vec3 p1 = positions[t[1].position];
        const glm::vec3 p2 = positions[t[2].position];

        const glm::vec3 d0 = FromTo(p1, p0);
        const glm::vec3 d1 = FromTo(p1, p2);

        const glm::vec3 faceNormal = crossNorm(d1, d0);

        for (int i = 0; i < 3; ++i)
        {
            vertexNormalsSum[t[i].position] += faceNormal;
            t[i].normal = t[i].position;
        }
    }

    normals.clear();
    for(const glm::vec3& normalSum: vertexNormalsSum)
    {
        normals.push_back(glm::normalize(normalSum));
    }

    assert(normals.size() == positions.size());
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


Mesh Builder::asMesh() const
{
    std::map<Combo, u32> combinations;

    // foreach triangle
    std::vector<klotter::Vertex> vertices;
    std::vector<klotter::Face> faces;
    
    for(const Triangle& sourceTriangle: triangles)
    {
        u32 triangle[3];

        // get combinations, add if not existing
        for (int i = 0; i < 3; ++i)
        {
            const Combo c(sourceTriangle[i]);
            auto result = combinations.find(c);
            if (result != combinations.end())
            {
                triangle[i] = result->second;
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
                vertices.emplace_back(pos ,text);
                combinations.insert({ c,  Csizet_to_u32(ind) });
                triangle[i] = Csizet_to_u32(ind);
            }
        }

        // add traingle to mesh
        faces.emplace_back(Face{ triangle[0], triangle[1], triangle[2] });
    }

    return Mesh(std::move(vertices), std::move(faces));
}






}

