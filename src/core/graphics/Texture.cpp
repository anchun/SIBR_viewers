
#include "core/graphics/Texture.hpp"

namespace sibr
{
	void			blit(const ITexture2D& src, const IRenderTarget& dst, GLbitfield mask, GLenum filter)
	{
		GLuint sourceFrameBuffer = 0;
		glGenFramebuffers(1, &sourceFrameBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, sourceFrameBuffer);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src.handle(), 0);

		SIBR_ASSERT(glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBlitNamedFramebuffer(
			sourceFrameBuffer, dst.fbo(),
			0, 0, src.w(), src.h(),
			0, 0, dst.w(), dst.h(),
			mask, filter);

		glDeleteFramebuffers(1, &sourceFrameBuffer);
	}

	void			blit_and_flip(const ITexture2D& src, const IRenderTarget& dst, GLbitfield mask, GLenum filter)
	{
		GLuint sourceFrameBuffer = 0;
		glGenFramebuffers(1, &sourceFrameBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, sourceFrameBuffer);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src.handle(), 0);

		SIBR_ASSERT(glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBlitNamedFramebuffer(
			sourceFrameBuffer, dst.fbo(),
			0, 0, src.w(), src.h(),
			0, dst.h(), dst.w(), 0,
			mask, filter);
		glDeleteFramebuffers(1, &sourceFrameBuffer);
	}

	void			blit(const IRenderTarget& src, const ITexture2D& dst, GLbitfield mask, GLenum filter)
	{
		GLuint dstFrameBuffer = 0;
		glGenFramebuffers(1, &dstFrameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFrameBuffer);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst.handle(), 0);

		SIBR_ASSERT(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBlitNamedFramebuffer(
			src.fbo(), dstFrameBuffer,
			0, 0, src.w(), src.h(),
			0, 0, dst.w(), dst.h(),
			mask, filter);
		glDeleteFramebuffers(1, &dstFrameBuffer);
	}

	void			blit(const ITexture2D& src, const ITexture2D& dst, GLbitfield mask, GLenum filter)
	{
		GLuint fbo[2];
		glGenFramebuffers(2, fbo);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[0]);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src.handle(), 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[0]);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst.handle(), 0);

		SIBR_ASSERT(glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		SIBR_ASSERT(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glBlitNamedFramebuffer(
			fbo[0], fbo[1],
			0, 0, src.w(), src.h(),
			0, 0, dst.w(), dst.h(),
			mask, filter);
		glDeleteFramebuffers(2, fbo);
	}

} // namespace sibr
