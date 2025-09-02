#pragma once

#include "klotter/render/texture.h"

#include "embed/types.h"

namespace klotter
{

[[nodiscard]]
Texture2d load_image_from_embedded(
	DEBUG_LABEL_ARG_MANY 
	const embedded_binary& image_binary, TextureEdge te, TextureRenderStyle trs, Transparency t, ColorData cd
);

[[nodiscard]]
TextureCubemap load_cubemap_from_embedded(
	DEBUG_LABEL_ARG_MANY 
	const embedded_binary& image_right,
	const embedded_binary& image_left,
	const embedded_binary& image_top,
	const embedded_binary& image_bottom,
	const embedded_binary& image_back,
	const embedded_binary& image_front,
	ColorData cd
);


}  //  namespace klotter
