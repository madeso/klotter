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
	DEBUG_LABEL_ARG_MANY
	std::shared_ptr<Texture2d>* texture,
	const embedded_binary& bin,
	ColorData cd,
	TextureEdge texture_edge = TextureEdge::repeat,
	Transparency transparency = Transparency::exclude
)
{
	if (*texture == nullptr)
	{
		*texture = std::make_shared<Texture2d>(
			load_image_from_embedded(SEND_DEBUG_LABEL_MANY(debug_label) bin, texture_edge, TextureRenderStyle::mipmap, transparency, cd)
		);
	}
	return *texture;
}

std::shared_ptr<Texture2d> get_or_create(DEBUG_LABEL_ARG_MANY std::shared_ptr<Texture2d>* texture, SingleColor pixel_color, ColorData cd)
{
	if (*texture == nullptr)
	{
		*texture = std::make_shared<Texture2d>(
			load_image_from_color(SEND_DEBUG_LABEL_MANY(debug_label) pixel_color, TextureEdge::repeat, TextureRenderStyle::pixel, Transparency::exclude, cd)
		);
	}

	return *texture;
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture2d> Assets::get_black()
{
	return get_or_create(USE_DEBUG_LABEL_MANY("black from pixel") & black, color_from_rgba(0x00, 0x00, 0x00, 0xFF), ColorData::dont_care);
}

std::shared_ptr<Texture2d> Assets::get_white()
{
	return get_or_create(USE_DEBUG_LABEL_MANY("white from pixel") & white, color_from_rgba(0xFF, 0xFF, 0xFF, 0xFF), ColorData::dont_care);
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture2d> Assets::get_cookie()
{
	return get_or_load(USE_DEBUG_LABEL_MANY("cookie.png") &cookie, COOKIE_01_PNG, ColorData::color_data);
}

std::shared_ptr<Texture2d> Assets::get_dark_grid()
{
	return get_or_load(USE_DEBUG_LABEL_MANY("dark_grid.png") &dark_grid, DARK_01_PNG, ColorData::color_data);
}

std::shared_ptr<Texture2d> Assets::get_light_grid()
{
	return get_or_load(USE_DEBUG_LABEL_MANY("light_grid.png") &light_grid, LIGHT_01_PNG, ColorData::color_data);
}

// ----------------------------------------------------------------------------

std::shared_ptr<Texture2d> Assets::get_container_diffuse()
{
	return get_or_load(USE_DEBUG_LABEL_MANY("container-diffuse.png") &container_diffuse, CONTAINER_DIFFUSE_PNG, ColorData::color_data);
}

std::shared_ptr<Texture2d> Assets::get_container_specular()
{
	return get_or_load(USE_DEBUG_LABEL_MANY("container-specular.png") &container_specular, CONTAINER_SPECULAR_PNG, ColorData::non_color_data);
}

std::shared_ptr<Texture2d> Assets::get_matrix()
{
	return get_or_load(USE_DEBUG_LABEL_MANY("matrix.jpg") &matrix, MATRIX_JPG, ColorData::color_data);
}

std::shared_ptr<Texture2d> Assets::get_glass()
{
	return get_or_load(USE_DEBUG_LABEL_MANY("glass.png") &glass, GLASS_PNG, ColorData::color_data);
}

std::shared_ptr<Texture2d> Assets::get_grass()
{
	return get_or_load(USE_DEBUG_LABEL_MANY("grass.png") &grass, GRASS_PNG, ColorData::color_data, TextureEdge::clamp, Transparency::include);
}

std::shared_ptr<TextureCubemap> Assets::get_skybox()
{
	if (skybox == nullptr)
	{
		skybox = std::make_shared<TextureCubemap>(load_cubemap_from_embedded(
			USE_DEBUG_LABEL_MANY("skybox cubemap") 
			std::array{
				SKYBOX_RIGHT_JPG, SKYBOX_LEFT_JPG, SKYBOX_TOP_JPG, SKYBOX_BOTTOM_JPG, SKYBOX_BACK_JPG, SKYBOX_FRONT_JPG
			},
			ColorData::color_data
		));
	}

	return skybox;
}

}  //  namespace klotter
