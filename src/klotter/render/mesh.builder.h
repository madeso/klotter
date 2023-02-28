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
    Index normal;
    Index texture;

    Vertex(Index vertex, Index normal, Index texture);
};


struct Triangle
{
    Vertex v0;
    Vertex v1;
    Vertex v2;

    Triangle(Vertex v0, Vertex v1, Vertex v2);
};

struct Builder
{
    Index add_text_coord(const glm::vec2& v);
    Index add_position(const glm::vec3& pos);
    Index add_normal(const glm::vec3& norm);
    
    Builder& add_triangle(const Triangle& t);
    Builder& add_quad(bool reverse, const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3);
    Builder& add_face(const std::vector<Vertex>&);

    Builder& replace_with_smooth_normals();
    Builder& move(const glm::vec3& dir);
    Builder& scale(float scale);
    Builder& invert_normals();

    Mesh to_mesh() const;

    std::vector<Triangle> triangles;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
};


Builder create_box(float w, float h, float d, bool face_out);

}
