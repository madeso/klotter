#include "klotter/render/shader.source.h"

#include "unlit.vert.glsl.h"
#include "unlit.frag.glsl.h"

#include "default.vert.glsl.h"
#include "default.frag.glsl.h"

namespace klotter
{

ShaderSource load_unlit_shader_source()
{
	return {
		{{VertexType::position3, "a_position"},
		 {VertexType::color3, "a_color"},
		 {VertexType::texture2, "a_tex_coord"}},
		UNLIT_VERT_GLSL,
		UNLIT_FRAG_GLSL};
}

ShaderSource load_default_shader_source()
{
	return {
		{{VertexType::position3, "a_position"},
		 {VertexType::normal3, "a_normal"},
		 {VertexType::color3, "a_color"},
		 {VertexType::texture2, "a_tex_coord"}},
		DEFAULT_VERT_GLSL,
		DEFAULT_FRAG_GLSL};
}

}  //  namespace klotter
