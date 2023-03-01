#include "klotter/render/assets.h"

#include "klotter/render/texture.io.h"


// assets
#include "light_01.png.h"
#include "dark_01.png.h"


namespace klotter
{

std::shared_ptr<Texture> get_or_load(std::shared_ptr<Texture>* texture, const embedded_binary& bin)
{
	if (*texture == nullptr)
	{
		*texture = std::make_shared<Texture>(load_image_from_embedded(bin,
			TextureEdge::repeat, TextureRenderStyle::smooth, Transparency::exclude));
	}
	return *texture;
}

std::shared_ptr<Texture> Assets::get_dark_grid()
{
	return get_or_load(&dark_grid, DARK_01_PNG);
}

std::shared_ptr<Texture> Assets::get_light_grid()
{
	return get_or_load(&light_grid, LIGHT_01_PNG);
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

