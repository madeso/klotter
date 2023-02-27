#include "klotter/render/assets.h"

#include "klotter/render/texture.io.h"


// assets
#include "texture_01.png.h"


namespace klotter
{

std::shared_ptr<Texture> Assets::get_dark_grid()
{
	if(dark_grid == nullptr)
	{
		dark_grid = std::make_shared<Texture>(load_image_from_embedded(TEXTURE_01_PNG,
			TextureEdge::repeat, TextureRenderStyle::smooth, Transparency::exclude));
	}
	return dark_grid;
}

std::shared_ptr<Texture> Assets::get_white()
{
	if(white == nullptr)
	{
		white = std::make_shared<Texture>(load_image_from_color(0xFFFFFFFF,
			TextureEdge::repeat, TextureRenderStyle::pixel, Transparency::exclude));
	}

	return white;
}

}

