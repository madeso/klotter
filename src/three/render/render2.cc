#include "three/render/render2.h"

#include "three/dependency_opengl.h"
#include "three/assert.h"
#include "three/rect.h"

#include "three/render/shader.h"
#include "three/render/texture.h"
#include "three/render/opengl_utils.h"

#define ASSERTX(X,...) ASSERT(X)

using namespace std::literals;


namespace render
{

SpriteBatch::SpriteBatch(ShaderProgram* quad_shader, Render2* r)
    : render(r)
    , white_texture
    (
        load_image_from_color
        (
            0xffffffff,
            TextureEdge::clamp,
            TextureRenderStyle::pixel,
            Transparency::include
        )
    )
{
    quad_shader->use();

    glGenVertexArrays(1, &va);
    glBindVertexArray(va);

    constexpr auto vertex_size = 9 * sizeof(float);
    constexpr auto max_vertices = 4 * max_quads;
    constexpr auto max_indices = 6 * max_quads;

    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, vertex_size * max_vertices, nullptr, GL_DYNAMIC_DRAW);

    auto relative_offset = [](unsigned int i)
    {
        return reinterpret_cast<void*>(i * sizeof(float));
    };

    unsigned int offset = 0;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
    offset += 3;

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
    offset += 4;

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
    offset += 2;


    std::vector<u32> indices;
    indices.reserve(max_indices);

    for(auto quad_index=0; quad_index<max_quads; quad_index+=1)
    {
        const auto base = quad_index * 4;
        indices.emplace_back(base + 0);
        indices.emplace_back(base + 1);
        indices.emplace_back(base + 2);

        indices.emplace_back(base + 2);
        indices.emplace_back(base + 3);
        indices.emplace_back(base + 0);
    }

    ASSERT(max_indices == indices.size());

    glGenBuffers(1, &ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(u32), indices.data(), GL_STATIC_DRAW);
}


SpriteBatch::~SpriteBatch()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vb);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &va);
}


void add_vertex(SpriteBatch* batch, const Vertex2& v)
{
    batch->data.push_back(v.position.x);
    batch->data.push_back(v.position.y);
    batch->data.push_back(v.position.z);

    batch->data.push_back(v.color.x);
    batch->data.push_back(v.color.y);
    batch->data.push_back(v.color.z);
    batch->data.push_back(v.color.w);

    batch->data.push_back(v.texturecoord.x);
    batch->data.push_back(v.texturecoord.y);
}

Rectf get_sprite(const Texture& texture, const Recti& ri)
{
    const auto r = Cint_to_float(ri);
    const auto w = 1.0f/static_cast<float>(texture.width);
    const auto h = 1.0f/static_cast<float>(texture.height);
    return {r.left * w, 1-r.top * h, r.right * w, 1-r.bottom * h};
}

void SpriteBatch::quad(std::optional<Texture*> texture_argument, const Vertex2& v0, const Vertex2& v1, const Vertex2& v2, const Vertex2& v3)
{
    Texture* texture = texture_argument.value_or(&white_texture);

    if(quads == max_quads)
    {
        submit();
    }

    if(current_texture == nullptr)
    {
        current_texture = texture;
    }
    else if (current_texture != texture)
    {
        submit();
        current_texture = texture;
    }

    quads += 1;

    add_vertex(this, v0);
    add_vertex(this, v1);
    add_vertex(this, v2);
    add_vertex(this, v3);
}

void SpriteBatch::quad(std::optional<Texture*> texture, const Rectf& scr, const std::optional<Rectf>& texturecoord, const glm::vec4& tint)
{
    const auto tc = texturecoord.value_or(Rectf{1.0f, 1.0f});
    quad
    (
        texture,
        {{scr.left, scr.bottom, 0.0f}, tint, {tc.left, tc.bottom}},
        {{scr.right, scr.bottom, 0.0f}, tint, {tc.right, tc.bottom}},
        {{scr.right, scr.top, 0.0f}, tint, {tc.right, tc.top}},
        {{scr.left, scr.top, 0.0f}, tint, {tc.left, tc.top}}
    );
}

void SpriteBatch::quad(std::optional<Texture*> texture_argument, const Rectf& scr, const Recti& texturecoord, const glm::vec4& tint)
{
    Texture* texture = texture_argument.value_or(&white_texture);
    quad(texture, scr, get_sprite(*texture, texturecoord), tint);
}

void SpriteBatch::submit()
{
    if(quads == 0)
    {
        return;
    }

    bind_texture(render->texture_uniform, *current_texture);
    glBindVertexArray(va);

    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferSubData
    (
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(sizeof(float) * data.size()),
        static_cast<const void*>(data.data())
    );
    glDrawElements(GL_TRIANGLES, 6 * quads, GL_UNSIGNED_INT, nullptr);

    data.resize(0);
    quads = 0;
    current_texture = nullptr;
}


Render2::Render2()
    // todo(Gustav): move quad_description and quad_layout to a seperate setup
    : quad_description
    (
        {
            {VertexType::position2, "position"},
            {VertexType::color4, "color"},
            {VertexType::texture2, "uv"}
        }
    )
    , quad_layout
    (
        compile_shader_layout(compile_attribute_layouts({quad_description}), quad_description)
    )
    , quad_shader
    (
        R"glsl(
            #version 450 core
            in vec3 position;
            in vec4 color;
            in vec2 uv;

            uniform mat4 view_projection;
            uniform mat4 transform;

            out vec4 varying_color;
            out vec2 varying_uv;

            void main()
            {
                varying_color = color;
                varying_uv = uv;
                gl_Position = view_projection * transform * vec4(position, 1.0);
            }
        )glsl"sv,
        R"glsl(
            #version 450 core

            in vec4 varying_color;
            in vec2 varying_uv;

            uniform sampler2D uniform_texture;

            out vec4 color;

            void main()
            {
                color = texture(uniform_texture, varying_uv) * varying_color;
            }
        )glsl"sv,
        quad_layout
    )
    , view_projection_uniform(quad_shader.get_uniform("view_projection"))
    , transform_uniform(quad_shader.get_uniform("transform"))
    , texture_uniform(quad_shader.get_uniform("uniform_texture"))
    , batch(&quad_shader, this)
{
    setup_textures(&quad_shader, {&texture_uniform});

    // todo(Gustav): verify mesh layout with SpriteBatch
}


struct ViewportDef
{
    Recti screen_rect;

    float virtual_width;
    float virtual_height;
};


ViewportDef
fit_with_black_bars
(
    float width,
    float height,
    int window_width,
    int window_height
)
{
    ASSERTX(width > 0, width);
    ASSERTX(height > 0, height);
    ASSERTX(window_width >= 0, window_width);
    ASSERTX(window_height >= 0, window_height);
    const float w = static_cast<float>(window_width) / width;
    const float h = static_cast<float>(window_height) / height;
    const float s = std::min(w, h);
    ASSERTX(s > 0, s, w, h);
    const float new_width = width * s;
    const float new_height = height * s;

    return ViewportDef
    {
        Recti(static_cast<int>(new_width), static_cast<int>(new_height)).set_bottom_left
        (
            static_cast<int>((static_cast<float>(window_width) - new_width) / 2.0f),
            static_cast<int>((static_cast<float>(window_height) - new_height) / 2.0f)
        ),
        width,
        height
    };
}


float
DetermineExtendScale(float scale, float height, int window_height)
{
    const auto scaled_height = height * scale;
    const auto s = static_cast<float>(window_height) / scaled_height;
    return s;
}


ViewportDef
extended_viewport
(
    float width,
    float height,
    int window_width,
    int window_height
)
{
    ASSERTX(width >= 0, width);
    ASSERTX(height >= 0, height);
    ASSERTX(window_width >= 0, window_width);
    ASSERTX(window_height >= 0, window_height);
    const auto w = static_cast<float>(window_width) / width;
    const auto h = static_cast<float>(window_height) / height;
    const auto r = Recti{window_width, window_height}.set_bottom_left(0, 0);
    if(w < h)
    {
        const auto s = DetermineExtendScale(w, height, window_height);
        return ViewportDef {r, width, height * s};
    }
    else
    {
        const auto s = DetermineExtendScale(h, width, window_width);
        return ViewportDef {r, width * s, height};
    }
}


namespace
{
    float lerp(float lhs, float t, float rhs)
    {
        return lhs + t * (rhs - lhs);
    }

    int lerp(int lhs, float t, int rhs)
    {
        return static_cast<int>
        (
            lerp
            (
                static_cast<float>(lhs),
                t,
                static_cast<float>(rhs)
            )
        );
    }

    template<typename T>
    Rect<T> lerp(const Rect<T>& lhs, float t, const Rect<T>& rhs)
    {
        #define V(x) lerp(lhs.x, t, rhs.x)
        return
        {
            V(left),
            V(bottom),
            V(right),
            V(top)
        };
        #undef V
    }
}


[[nodiscard]]
ViewportDef
lerp
(
    const ViewportDef& lhs,
    float t,
    const ViewportDef& rhs
)
{
    #define V(x) lerp(lhs.x, t, rhs.x)
    return
    {
        V(screen_rect),
        V(virtual_width),
        V(virtual_height)
    };
    #undef V
}



void set_gl_viewport(const Recti& r)
{
    glViewport(r.left, r.bottom, r.get_width(), r.get_height());
}


RenderLayer2::~RenderLayer2()
{
    batch->submit();
}


RenderLayer2::RenderLayer2(Layer&& l, SpriteBatch* b)
    : Layer(l)
    , batch(b)
{
}

RenderLayer3::RenderLayer3(Layer&& l)
    : Layer(l)
{
}


Layer create_layer(const ViewportDef& vp)
{
    return {{vp.virtual_width, vp.virtual_height}, vp.screen_rect};
}


RenderLayer2 create_layer2(const RenderCommand& rc, const ViewportDef& vp)
{
    set_gl_viewport(vp.screen_rect);
    opengl_set2d(rc.states);

    const auto camera = glm::mat4(1.0f);
    const auto projection = glm::ortho(0.0f, vp.virtual_width, 0.0f, vp.virtual_height);

    rc.render->quad_shader.use();
    rc.render->quad_shader.set_mat(rc.render->view_projection_uniform, projection);
    rc.render->quad_shader.set_mat(rc.render->transform_uniform, camera);

    // todo(Gustav): transform viewport according to the camera
    return RenderLayer2{create_layer(vp), &rc.render->batch};
}

RenderLayer3 create_layer3(const RenderCommand& rc, const ViewportDef& vp)
{
    set_gl_viewport(vp.screen_rect);
    opengl_set3d(rc.states);

    return RenderLayer3{create_layer(vp)};
}


glm::vec2 Layer::mouse_to_world(const glm::vec2& p) const
{
    // transform from mouse pixels to window 0-1
    const auto n = Cint_to_float(screen).to01(p);
    return viewport_aabb_in_worldspace.from01(n);
}


ViewportDef create_viewport(const LayoutData& ld, const glm::ivec2& size)
{
    if(ld.style==ViewportStyle::black_bars)
    {
        return fit_with_black_bars(ld.requested_width, ld.requested_height, size.x, size.y);
    }
    else
    {
        return extended_viewport(ld.requested_width, ld.requested_height, size.x, size.y);
    }
}


ViewportDef create_viewport(const LerpData& ld, const glm::ivec2& size)
{
    return lerp
    (
        create_viewport(ld.lhs, size),
        ld.t,
        create_viewport(ld.rhs, size)
    );
}


void
RenderCommand::clear(const glm::vec3& color, const LayoutData& ld) const
{
    if(ld.style == ViewportStyle::extended)
    {
        glClearColor(color.r, color.g, color.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        auto l = with_layer2(*this, ld);
        l.batch->quad({}, l.viewport_aabb_in_worldspace, {}, glm::vec4{color, 1.0f});
    }
}


bool
is_fullscreen(const LerpData& ld)
{
    if(ld.lhs.style == ld.rhs.style || (ld.t <= 0.0f || ld.t >= 1.0f))
    {
        const auto style = ld.t >= 1.0f ? ld.rhs.style : ld.lhs.style;
        return style == ViewportStyle::extended;
    }

    return false;
}


void
RenderCommand::clear(const glm::vec3& color, const LerpData& ld) const
{
    if(is_fullscreen(ld))
    {
        glClearColor(color.r, color.g, color.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        auto l = with_layer2(*this, ld);
        l.batch->quad({}, l.viewport_aabb_in_worldspace, {}, glm::vec4{color, 1.0f});
    }
}


RenderLayer2 with_layer2(const RenderCommand& rc, const LayoutData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer2(rc, vp);
}


RenderLayer3 with_layer3(const RenderCommand& rc, const LayoutData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer3(rc, vp);
}


Layer with_layer(const InputCommand& rc, const LayoutData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer(vp);
}





RenderLayer2 with_layer2(const RenderCommand& rc, const LerpData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer2(rc, vp);
}


RenderLayer3 with_layer3(const RenderCommand& rc, const LerpData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer3(rc, vp);
}


Layer with_layer(const InputCommand& rc, const LerpData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer(vp);
}

}
