#pragma once

#include "embed/types.h"
#include "klotter/render/texture.h"

namespace klotter
{


Texture2d load_image_from_embedded(
	const embedded_binary& image_binary, TextureEdge te, TextureRenderStyle trs, Transparency t
);

TextureCubemap load_cubemap_from_embedded(
	const embedded_binary& image_right,
	const embedded_binary& image_left,
	const embedded_binary& image_top,
	const embedded_binary& image_bottom,
	const embedded_binary& image_back,
	const embedded_binary& image_front
);


}  //  namespace klotter
