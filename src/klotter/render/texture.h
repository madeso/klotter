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


}  //  namespace klotter
