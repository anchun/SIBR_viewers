
#ifndef __SIBR_EXP_RENDERER_ADDSHADOWRENDERER_HPP__
# define __SIBR_EXP_RENDERER_ADDSHADOWRENDERER_HPP__

# include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Camera.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 

	/**
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT AddShadowRenderer
	{
	public:
		typedef std::shared_ptr<AddShadowRenderer>	Ptr;

	public:
		AddShadowRenderer( void );

		void	process(
			/*input*/	uint backgroundTextureID,
						uint foregroundTextureID,
						const Vector2f& textureSize,
						const Camera& camera,
			/*output*/	IRenderTarget& dst );

	private:
		GLShader			_shader;
		GLParameter			_paramInvProj;
		GLParameter			_paramImgSize;
	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDERER_ADDSHADOWRENDERER_HPP__
