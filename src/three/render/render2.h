#pragma once

#include "three/rect.h"
#include "three/types.h"

#include "three/render/shader.h"
#include "three/render/texture.h"


namespace three
{

struct SpriteBatch;
struct OpenglStates;
struct Texture;
struct ShaderProgram;
struct Render2;


enum class ViewportStyle { black_bars, extended};


struct Vertex2
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texturecoord;
};

struct SpriteBatch
{
    static constexpr int max_quads = 100;

    std::vector<float> data;
    int quads = 0;
    Texture* current_texture = nullptr;
    u32 va;
    u32 vb;
    u32 ib;
    Render2* render;
    Texture white_texture;

    SpriteBatch(ShaderProgram* shader, Render2* r);
    ~SpriteBatch();

    SpriteBatch(const SpriteBatch&) = delete;
    void operator=(const SpriteBatch&) = delete;
    SpriteBatch(SpriteBatch&&) = delete;
    void operator=(SpriteBatch&&) = delete;

    void quad(std::optional<Texture*> texture, const Vertex2& v0, const Vertex2& v1, const Vertex2& v2, const Vertex2& v3);
    void quad(std::optional<Texture*> texture, const Rectf& scr, const std::optional<Rectf>& texturecoord, const glm::vec4& tint = glm::vec4(1.0f));
    void quad(std::optional<Texture*> texture, const Rectf& scr, const Recti& texturecoord, const glm::vec4& tint = glm::vec4(1.0f));

    void submit();
};

struct Render2
{
    Render2();

    ShaderVertexAttributes quad_description;
    CompiledShaderVertexAttributes quad_layout;
    ShaderProgram quad_shader;
    Uniform view_projection_uniform;
    Uniform transform_uniform;
    Uniform texture_uniform;

    SpriteBatch batch;
};

struct Layer
{
    Rectf viewport_aabb_in_worldspace;
    Recti screen;

    glm::vec2 mouse_to_world(const glm::vec2& p) const;
};

struct RenderLayer2 : Layer
{
    SpriteBatch* batch;

    RenderLayer2(Layer&& l, SpriteBatch* batch);

    ~RenderLayer2();
};

struct RenderLayer3 : Layer
{
    RenderLayer3(Layer&& l);
};


struct LayoutData
{
    ViewportStyle style;
    float requested_width;
    float requested_height;
};

struct LerpData
{
    LayoutData lhs;
    LayoutData rhs;
    float t;
};


struct InputCommand
{
    glm::ivec2 size;
};

struct RenderCommand
{
    OpenglStates* states;
    Render2* render;
    glm::ivec2 size;

    // tood(Gustav): add clear to color function
    void clear(const glm::vec3& color, const LayoutData& ld) const;
    void clear(const glm::vec3& color, const LerpData& ld) const;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions


// todo(Gustav): pass camera to with_ functions
// todo(Gustav): move to member functions

RenderLayer2 with_layer2(const RenderCommand& rc, const LayoutData& ld);
RenderLayer3 with_layer3(const RenderCommand& rc, const LayoutData& ld);
Layer with_layer(const InputCommand& rc, const LayoutData& ld);

RenderLayer2 with_layer2(const RenderCommand& rc, const LerpData& ld);
RenderLayer3 with_layer3(const RenderCommand& rc, const LerpData& ld);
Layer with_layer(const InputCommand& rc, const LerpData& ld);

}
