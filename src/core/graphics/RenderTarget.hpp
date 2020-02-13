
#ifndef __SIBR_GRAPHICS_RENDERTARGET_HPP__
# define __SIBR_GRAPHICS_RENDERTARGET_HPP__

# include "core/graphics/Config.hpp"
# include "core/graphics/Texture.hpp"
# include "core/system/Vector.hpp"

namespace sibr
{

	/**
	Copy the content of a render target to another render target, resizing if needed.
	\param src source rendertarget
	\param dst destination rendertarget
	\param mask which part of the buffer to copy (color, depth, stencil).
	\param filtering mode if the two rendertargets have different dimensions (linear or nearest)
	\note The blit can only happen for color attachment 0 in both src and dst.
	\warning If the mask contains the depth or stencil, filter must be GL_NEAREST
	 \ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT void			blit(const IRenderTarget& src, const IRenderTarget& dst, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);

	/**
	Copy the content of a render target to another render target, resizing if needed and flipping the result.
	\param src source rendertarget
	\param dst destination rendertarget
	\param mask which part of the buffer to copy (color, depth, stencil).
	\param filtering mode if the two rendertargets have different dimensions (linear or nearest)
	\note The blit can only happen for color attachment 0 in both src and dst.
	\warning If the mask contains the depth or stencil, filter must be GL_NEAREST
	 \ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT void			blit_and_flip(const IRenderTarget& src, const IRenderTarget& dst, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);

	/**
	Copy the content of a texture to a render target, resizing if needed.
	\param src source texture
	\param dst destination rendertarget
	\param mask which part of the buffer to copy (color, depth, stencil).
	\param filtering mode if the two buffers have different dimensions (linear or nearest)
	\note The blit can only happen for color attachment 0 in dst.
	\warning If the mask contains the depth or stencil, filter must be GL_NEAREST
	 \ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT void			blit(const ITexture2D& src, const IRenderTarget& dst, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);

	/**
	Copy the content of a texture to a render target, resizing if needed and flipping the result.
	\param src source texture
	\param dst destination rendertarget
	\param mask which part of the buffer to copy (color, depth, stencil).
	\param filtering mode if the two buffers have different dimensions (linear or nearest)
	\note The blit can only happen for color attachment 0 in dst.
	\warning If the mask contains the depth or stencil, filter must be GL_NEAREST
	 \ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT void			blit_and_flip(const ITexture2D& src, const IRenderTarget& dst, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);

	/**
	Copy the content of a rendertarget first color attachment to a texture, resizing if needed.
	\param src source rendertarget
	\param dst destination texture
	\param mask which part of the buffer to copy (color, depth, stencil).
	\param filtering mode if the two buffers have different dimensions (linear or nearest)
	\note The blit can only happen for color attachment 0 in dst.
	\warning If the mask contains the depth or stencil, filter must be GL_NEAREST
	 \ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT void			blit(const IRenderTarget& src, const ITexture2D& dst, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);

	// Note the following overload is defined in Texture.hpp
	// void			blit(const ITexture2D& src, const ITexture2D& dst, GLbitfield mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR);

	// Note you can also use glCopyTexSubImage2D for copyiung to a texture if the size are the same
	//void			copy(const IRenderTarget& src, const ITexture2D& dst) {
	//	glBindFramebuffer(GL_READ_FRAMEBUFFER, src.fbo());
	//	glBindTexture(GL_TEXTURE_2D, dst.handle());
	//	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 
	//		0, 0, 
	//		0, 0, src.w(), src.h());
	//	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	//}


	/** Display a rendertarget color content in a popup window (backed by OpenCV).
	\param rt the rendertarget to display
	\param layer the color attachment to display
	\param windowTitle name of the window
	\param closeWindow should the window be closed when pressing a key
	\ingroup sibr_graphics
	*/
	template <typename T_Type, unsigned T_NumComp>
	static void		show( const RenderTarget<T_Type, T_NumComp> & rt, uint layer=0, const std::string& windowTitle="sibr::show()" , bool closeWindow = true ) {
		sibr::Image<T_Type, T_NumComp>	img(rt.w(), rt.h());
		rt.readBack(img, layer);
		show(img, windowTitle, closeWindow);
	}
	
	/** Display a rendertarget depth content in a popup window (backed by OpenCV).
	\param rt the rendertarget to display
	\param windowTitle name of the window
	\param closeWindow should the window be closed when pressing a key
	\ingroup sibr_graphics
	*/
	template <typename T_Type, unsigned T_NumComp>
	static void		showDepth( const RenderTarget<T_Type, T_NumComp> & rt, const std::string& windowTitle="sibr::show()" , bool closeWindow = true ) {
		sibr::Image<float, 3>	img(rt.w(), rt.h());
		rt.readBackDepth(img);
		show(img, windowTitle, closeWindow);
	}
	
	/** Display a rendertarget alpha content as a grey map in a popup window (backed by OpenCV).
	\param rt the rendertarget to display
	\param windowTitle name of the window
	\param closeWindow should the window be closed when pressing a key
	\ingroup sibr_graphics
	*/
	template <typename T_Type, unsigned T_NumComp>
	static void		showDepthFromAlpha( const RenderTarget<T_Type, T_NumComp> & rt, const std::string& windowTitle="sibr::show()" , bool closeWindow = true ) {
		sibr::Image<float, 4>	img(rt.w(), rt.h());
		rt.readBack(img);

		for (uint y = 0; y < img.h(); ++y)
		{
			for (uint x = 0; x < img.w(); ++x)
			{
				sibr::ColorRGBA c = img.color(x, y);
				c = sibr::ColorRGBA(1.f, 1.f, 1.f, 0.f) * c[3];
				c[3] = 1.f;
				img.color(x, y, c);
			}
		}

		show(img, windowTitle, closeWindow);
	}

} // namespace sibr

#endif // __SIBR_GRAPHICS_RENDERTARGET_HPP__
