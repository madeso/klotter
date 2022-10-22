#pragma once

namespace three
{

struct OpenglStates
{
    std::optional<bool> cull_face;
    std::optional<bool> blending;
    std::optional<bool> depth_test;
    std::optional<unsigned int> render_mode;
};

void opengl_setup(OpenglStates* states);
void opengl_set2d(OpenglStates* states);
void opengl_set3d(OpenglStates* states);

void opengl_set_render_mode_to_fill(OpenglStates* states);
void opengl_set_render_mode_to_line(OpenglStates* states);
void opengl_set_render_mode_to_point(OpenglStates* states);

}
