
#ifndef __SIBR_EXP_RENDERER_BLURRENDERER_HPP__
# define __SIBR_EXP_RENDERER_BLURRENDERER_HPP__

# include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 

	/** Blur on color edges present in a texture.
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT BlurRenderer
	{
	public:
		typedef std::shared_ptr<BlurRenderer>	Ptr;

	public:

		/// Constructor.
		BlurRenderer( void );

		/** Process the texture.
		\param textureID the texture to blur
		\param textureSize the texture dimensions
		\param dst the destination rendertarget
		*/
		void	process(
			/*input*/	uint textureID,
			/*input*/	const Vector2f& textureSize,
			/*output*/	IRenderTarget& dst );

	private:

		GLShader			_shader; ///< Blur shader.
		GLParameter			_paramImgSize; ///< Texture size uniform.

	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDERER_BLURRENDERER_HPP__
