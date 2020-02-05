
#ifndef __SIBR_EXP_RENDERER_BLURRENDERER_HPP__
# define __SIBR_EXP_RENDERER_BLURRENDERER_HPP__

# include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 

	/** Blur on edge (when adding object)
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT BlurRenderer
	{
	public:
		typedef std::shared_ptr<BlurRenderer>	Ptr;

	public:
		BlurRenderer( void );

		void	process(
			/*input*/	uint textureID,
			/*input*/	const Vector2f& textureSize,
			/*output*/	IRenderTarget& dst );

	private:
		GLShader			_shader;
		GLParameter			_paramImgSize;
	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDERER_BLURRENDERER_HPP__
