#pragma once

#include "klotter/render/uniform.h"

namespace klotter
{


enum class TextureEdge
{
	clamp,
	repeat
};


enum class TextureRenderStyle
{
	pixel,
	mipmap,
	linear
};


enum class Transparency
{
	include,
	exclude
};

struct Texture2d
{
	unsigned int id;
	int width;
	int height;

	Texture2d();  ///< creates a invalid texture

	/// "internal"
	Texture2d(
		void* pixel_data, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t
	);

	~Texture2d();


	Texture2d(const Texture2d&) = delete;
	void operator=(const Texture2d&) = delete;

	Texture2d(Texture2d&&);
	void operator=(Texture2d&&);

	// clears the loaded texture to a invalid texture
	void unload();
};

Texture2d load_image_from_color(u32 pixel, TextureEdge te, TextureRenderStyle trs, Transparency t);

struct TextureCubemap
{
	unsigned int id;
	int width;
	int height;

	TextureCubemap();  ///< creates a invalid cubemap

	TextureCubemap(std::array<void*, 6> pixel_data, int w, int h);

	~TextureCubemap();


	TextureCubemap(const TextureCubemap&) = delete;
	void operator=(const TextureCubemap&) = delete;

	TextureCubemap(TextureCubemap&&);
	void operator=(TextureCubemap&&);

	// clears the loaded cubemap to a invalid texture
	void unload();
};

TextureCubemap load_cubemap_from_color(u32 pixel);


}  //  namespace klotter
