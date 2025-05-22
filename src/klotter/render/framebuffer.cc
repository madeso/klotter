#include "klotter/render/framebuffer.h"

#include "klotter/assert.h"
#include "klotter/log.h"

namespace klotter
{


unsigned int create_fbo()
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	ASSERT(fbo != 0);
	return fbo;
}

FrameBuffer::FrameBuffer(unsigned int f)
	: fbo(f)
{
}

FrameBuffer::~FrameBuffer()
{
	if (fbo)
	{
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
	}
	if (rbo)
	{
		glDeleteRenderbuffers(1, &rbo);
		rbo = 0;
	}
}

BoundFbo::BoundFbo(std::shared_ptr<FrameBuffer> f)
	: fbo(std::move(f))
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
}

BoundFbo::~BoundFbo()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<FrameBuffer> create_frame_buffer(
	const FboSetup& set, TextureEdge te, TextureRenderStyle trs, Transparency trans
)
{
	LOG_INFO("Creating frame buffer %d %d", set.width, set.height);
	ASSERT(trs != TextureRenderStyle::mipmap);
	auto fbo = std::make_shared<FrameBuffer>(create_fbo());

	fbo->texture = Texture2d{nullptr, set.width, set.height, te, trs, trans};
	ASSERT(fbo->texture.id > 0);

	auto bound = BoundFbo{fbo};
	constexpr GLint mipmap_level = 0;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texture.id, mipmap_level);

	glGenRenderbuffers(1, &fbo->rbo);
	ASSERT(fbo->rbo != 0);
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, set.width, set.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Failed to create frame buffer");
		return nullptr;
	}

	return fbo;
}

}  //  namespace klotter
