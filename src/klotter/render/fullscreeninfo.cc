#include "klotter/render/fullscreeninfo.h"

#include "klotter/render/geom.builder.h"
#include "klotter/render/geom.h"

namespace klotter
{

FullScreenInfo::FullScreenInfo()
{
	const auto layout_shader_material
		= ShaderVertexAttributes{{VertexType::position2xy, "a_position"}, {VertexType::texture2, "a_tex_coord"}};

	auto layout_compiler = compile_attribute_layouts({layout_shader_material});
	full_screen_layout = compile_shader_layout(layout_compiler, layout_shader_material, std::nullopt);
	const auto layout = get_geom_layout(layout_compiler);

	// ndc is [-1, 1], plane func generate a centered geom, so we set out plane to a size of 2
	full_screen_geom = compile_geom(geom::create_xy_plane(2.0f, 2.0f, geom::TwoSided::one_sided).to_geom(), layout);
}

}  //  namespace klotter
