#include "klotter/render/shader.source.h"

#include "basic.vert.glsl.h"
#include "basic.frag.glsl.h"

#include "light.vert.glsl.h"
#include "light.frag.glsl.h"

namespace klotter
{

ShaderSource basic_shader_source()
{
	return {
		{{VertexType::position3, "a_position"},
		 {VertexType::color3, "a_color"},
		 {VertexType::texture2, "a_tex_coord"}},
		BASIC_VERT_GLSL,
		BASIC_FRAG_GLSL};
}

ShaderSource light_shader_source()
{
	return {
		{{VertexType::position3, "a_position"},
		 {VertexType::normal3, "a_normal"},
		 {VertexType::color3, "a_color"},
		 {VertexType::texture2, "a_tex_coord"}},
		LIGHT_VERT_GLSL,
		LIGHT_FRAG_GLSL};
}

}  //  namespace klotter
