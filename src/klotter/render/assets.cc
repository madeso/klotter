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

#include "skybox-right.jpg.h"
#include "skybox-left.jpg.h"
#include "skybox-top.jpg.h"
#include "skybox-bottom.jpg.h"
#include "skybox-back.jpg.h"
#include "skybox-front.jpg.h"

namespace klotter
{

std::shared_ptr<Texture2d> get_or_load(
	std::shared_ptr<Texture2d>* texture,
	const embedded_binary& bin,
	TextureEdge texture_edge = TextureEdge::repeat,
	Transparency transparency = Transparency::exclude
)
{
	if (*texture == nullptr)
	{
		*texture = std::make_shared<Texture2d>(
			load_image_from_embedded(bin, texture_edge, TextureRenderStyle::mipmap, transparency)
		);
	}
	return *texture;
}

std::shared_ptr<Texture2d> get_or_create(std::shared_ptr<Texture2d>* texture, u32 pixel_color)
{
	if (*texture == nullptr)
	{
		*texture = std::make_shared<Texture2d>(
			load_image_from_color(pixel_color, TextureEdge::repeat, TextureRenderStyle::pixel, Transparency::exclude)
		);
	}

	return *texture;
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture2d> Assets::get_black()
{
	return get_or_create(&black, 0x000000FF);
}

std::shared_ptr<Texture2d> Assets::get_white()
{
	return get_or_create(&white, 0xFFFFFFFF);
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture2d> Assets::get_cookie()
{
	return get_or_load(&cookie, COOKIE_01_PNG);
}

std::shared_ptr<Texture2d> Assets::get_dark_grid()
{
	return get_or_load(&dark_grid, DARK_01_PNG);
}

std::shared_ptr<Texture2d> Assets::get_light_grid()
{
	return get_or_load(&light_grid, LIGHT_01_PNG);
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture2d> Assets::get_container_diffuse()
{
	return get_or_load(&container_diffuse, CONTAINER_DIFFUSE_PNG);
}

std::shared_ptr<Texture2d> Assets::get_container_specular()
{
	return get_or_load(&container_specular, CONTAINER_SPECULAR_PNG);
}

std::shared_ptr<Texture2d> Assets::get_matrix()
{
	return get_or_load(&matrix, MATRIX_JPG);
}

std::shared_ptr<Texture2d> Assets::get_glass()
{
	return get_or_load(&glass, GLASS_PNG);
}

std::shared_ptr<Texture2d> Assets::get_grass()
{
	return get_or_load(&grass, GRASS_PNG, TextureEdge::clamp, Transparency::include);
}

std::shared_ptr<TextureCubemap> Assets::get_skybox()
{
	if (skybox == nullptr)
	{
		skybox = std::make_shared<TextureCubemap>(load_cubemap_from_embedded(
			SKYBOX_RIGHT_JPG, SKYBOX_LEFT_JPG, SKYBOX_TOP_JPG, SKYBOX_BOTTOM_JPG, SKYBOX_BACK_JPG, SKYBOX_FRONT_JPG
		));
	}

	return skybox;
}

}  //  namespace klotter
