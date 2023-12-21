#include "klotter/render/assets.h"

#include "klotter/render/texture.io.h"


// assets
#include "light_01.png.h"
#include "dark_01.png.h"
#include "matrix.jpg.h"
#include "container_diffuse.png.h"
#include "container_specular.png.h"
#include "cookie_01.png.h"
#include "glass.png.h"
#include "grass.png.h"

namespace klotter
{

std::shared_ptr<Texture> get_or_load(
	std::shared_ptr<Texture>* texture,
	const embedded_binary& bin,
	TextureEdge texture_edge = TextureEdge::repeat,
	Transparency transparency = Transparency::exclude
)
{
	if (*texture == nullptr)
	{
		*texture = std::make_shared<Texture>(
			load_image_from_embedded(bin, texture_edge, TextureRenderStyle::mipmap, transparency)
		);
	}
	return *texture;
}

std::shared_ptr<Texture> get_or_create(std::shared_ptr<Texture>* texture, u32 pixel_color)
{
	if (*texture == nullptr)
	{
		*texture = std::make_shared<Texture>(load_image_from_color(
			pixel_color, TextureEdge::repeat, TextureRenderStyle::pixel, Transparency::exclude
		));
	}

	return *texture;
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture> Assets::get_black()
{
	return get_or_create(&black, 0x000000FF);
}

std::shared_ptr<Texture> Assets::get_white()
{
	return get_or_create(&white, 0xFFFFFFFF);
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture> Assets::get_cookie()
{
	return get_or_load(&cookie, COOKIE_01_PNG);
}

std::shared_ptr<Texture> Assets::get_dark_grid()
{
	return get_or_load(&dark_grid, DARK_01_PNG);
}

std::shared_ptr<Texture> Assets::get_light_grid()
{
	return get_or_load(&light_grid, LIGHT_01_PNG);
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture> Assets::get_container_diffuse()
{
	return get_or_load(&container_diffuse, CONTAINER_DIFFUSE_PNG);
}

std::shared_ptr<Texture> Assets::get_container_specular()
{
	return get_or_load(&container_specular, CONTAINER_SPECULAR_PNG);
}

std::shared_ptr<Texture> Assets::get_matrix()
{
	return get_or_load(&matrix, MATRIX_JPG);
}

std::shared_ptr<Texture> Assets::get_glass()
{
	return get_or_load(&glass, GLASS_PNG);
}

std::shared_ptr<Texture> Assets::get_grass()
{
	return get_or_load(&grass, GRASS_PNG, TextureEdge::clamp, Transparency::include);
}

}  //  namespace klotter
