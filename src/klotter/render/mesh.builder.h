#pragma once

namespace klotter
{
    struct Mesh;
}


namespace klotter::mesh
{
using Index = u64;


struct Vertex
{
    Index position;
    Index texture;
    Index normal;

    Vertex();
    Vertex(Index vertex, Index normal, Index textureCoordinate);

    static Vertex Create_VN_T(Index vertexAndNormal, Index text);
};


struct Triangle
{
    Vertex v0;
    Vertex v1;
    Vertex v2;

    Triangle(Vertex v0, Vertex v1, Vertex v2);
    Triangle();

    Vertex& operator[](Index index);
    const Vertex& operator[](Index index) const;
};

struct Builder
{
    Index addTextCoord(const glm::vec2& v);
    Index addPosition(const glm::vec3& pos);
    Index addNormal(const glm::vec3& norm);
    void addTriangle(const Triangle& t);

    void addQuad(bool reverse, const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3);
    void addFace(const std::vector<Vertex>& vertices);

    void buildNormals();

    Mesh asMesh() const;

    std::vector<Triangle> triangles;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
};


Builder CreateBox(float w, float h, float d, bool faceOut);

}
