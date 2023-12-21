#include "klotter/assert.h"
#include "klotter/render/framebuffer.h"

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
	: fbo(f)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
}

BoundFbo::~BoundFbo()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<FrameBuffer> create_buffer(
	int width, int height, TextureEdge te, TextureRenderStyle trs, Transparency trans
)
{
	ASSERT(trs == TextureRenderStyle::mipmap);
	auto fbo = std::make_shared<FrameBuffer>(create_fbo());
	auto bound = BoundFbo{fbo};

	fbo->texture = Texture{nullptr, width, height, te, trs, trans};

	constexpr GLint mipmap_level = 0;
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texture.id, mipmap_level
	);


	glGenRenderbuffers(1, &fbo->rbo);
	ASSERT(fbo->rbo != 0);
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->rbo
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return nullptr;
	}

	return fbo;
	// glViewport(0, 0, width, height);
}

}  //  namespace klotter
