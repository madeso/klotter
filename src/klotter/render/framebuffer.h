#pragma once

#include "klotter/render/texture.h"

namespace klotter
{
/** \addtogroup framebuffer Frame buffer
 *  @{
*/


/// "render to texture" feature
struct FrameBuffer
{
	explicit FrameBuffer(unsigned int fbo);
	~FrameBuffer();

	unsigned int fbo = 0;

	Texture2d texture;
	unsigned int rbo = 0;
};

/// raii class to render to a FrameBuffer
struct BoundFbo
{
	std::shared_ptr<FrameBuffer> fbo;

	explicit BoundFbo(std::shared_ptr<FrameBuffer> f);
	~BoundFbo();
};

std::shared_ptr<FrameBuffer> create_frame_buffer(
	int width, int height, TextureEdge te, TextureRenderStyle trs, Transparency trans
);

/**
 * @}
*/

}  //  namespace klotter
