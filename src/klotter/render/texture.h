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

struct BaseTexture
{
	unsigned int id;
	int width;
	int height;

	BaseTexture();	///< creates a invalid texture
	explicit BaseTexture(int w, int h);	 ///< creates a valid texture

	~BaseTexture();

	BaseTexture(const BaseTexture&) = delete;
	void operator=(const BaseTexture&) = delete;

	BaseTexture(BaseTexture&&) noexcept;
	BaseTexture& operator=(BaseTexture&&) noexcept;

	// clears the loaded texture to a invalid texture
	void unload();
};

struct Texture2d : BaseTexture
{
	Texture2d() = default;	///< creates a invalid texture

	/// "internal"
	Texture2d(void* pixel_data, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t);
};

Texture2d load_image_from_color(u32 pixel, TextureEdge te, TextureRenderStyle trs, Transparency t);

struct TextureCubemap : BaseTexture
{
	TextureCubemap() = default;	 ///< creates a invalid cubemap

	TextureCubemap(std::array<void*, 6> pixel_data, int w, int h);
};

TextureCubemap load_cubemap_from_color(u32 pixel);


}  //  namespace klotter
