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
	Texture2d(const void* pixel_data, int w, int h, TextureEdge te, TextureRenderStyle trs, Transparency t);
};

Texture2d load_image_from_color(u32 pixel, TextureEdge te, TextureRenderStyle trs, Transparency t);

struct TextureCubemap : BaseTexture
{
	TextureCubemap() = default;	 ///< creates a invalid cubemap

	TextureCubemap(const std::array<void*, 6>& pixel_data, int w, int h);
};

TextureCubemap load_cubemap_from_color(u32 pixel);



/// "render to texture" feature
struct FrameBuffer
{
	explicit FrameBuffer(unsigned int fbo);
	~FrameBuffer();

	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer(FrameBuffer&&) = delete;
	void operator=(const FrameBuffer&) = delete;
	void operator=(FrameBuffer&&) = delete;

	unsigned int fbo = 0;

	Texture2d texture;
	unsigned int rbo = 0;
};

struct FboSetup
{
	constexpr FboSetup(int w, int h)
		: width(w)
		, height(h)
	{
	}

	int width;
	int height;
};

std::shared_ptr<FrameBuffer> create_frame_buffer(
	const FboSetup& set, TextureEdge te, TextureRenderStyle trs, Transparency trans
);

/// raii class to render to a FrameBuffer
struct BoundFbo
{
	std::shared_ptr<FrameBuffer> fbo;

	BoundFbo(const BoundFbo&) = delete;
	BoundFbo(BoundFbo&&) = delete;
	void operator=(const BoundFbo&) = delete;
	void operator=(BoundFbo&&) = delete;

	explicit BoundFbo(std::shared_ptr<FrameBuffer> f);
	~BoundFbo();
};



}  //  namespace klotter
