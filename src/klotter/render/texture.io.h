#pragma once

#include "embed/types.h"
#include "klotter/render/texture.h"

namespace klotter
{


Texture2d load_image_from_embedded(
	const embedded_binary& image_binary, TextureEdge te, TextureRenderStyle trs, Transparency t
);


}
