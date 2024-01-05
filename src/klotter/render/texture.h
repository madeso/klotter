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

struct Texture
{
	unsigned int id;
	int width;
	int height;

	Texture();	///< creates a invalid texture

	/// "internal"
	Texture(void* pixel_data, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t);

	~Texture();


	Texture(const Texture&) = delete;
	void operator=(const Texture&) = delete;

	Texture(Texture&&);
	void operator=(Texture&&);

	// clears the loaded texture to a invalid texture
	void unload();
};

Texture load_image_from_color(u32 pixel, TextureEdge te, TextureRenderStyle trs, Transparency t);

struct Cubemap
{
	unsigned int id;
	int width;
	int height;

	Cubemap();	///< creates a invalid cubemap

	Cubemap(std::array<void*, 6> pixel_data, int w, int h);

	~Cubemap();


	Cubemap(const Cubemap&) = delete;
	void operator=(const Cubemap&) = delete;

	Cubemap(Cubemap&&);
	void operator=(Cubemap&&);

	// clears the loaded cubemap to a invalid texture
	void unload();
};

Cubemap load_cubemap_from_color(u32 pixel);


}  //  namespace klotter
