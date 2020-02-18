
#ifndef __SIBR_EXP_RENDERER_ADDSHADOWRENDERER_HPP__
# define __SIBR_EXP_RENDERER_ADDSHADOWRENDERER_HPP__

# include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Camera.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 

	/** Composite two rendered scenes while generating local cast shadows from the top one to the bottom one.
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT AddShadowRenderer
	{
	public:
		typedef std::shared_ptr<AddShadowRenderer>	Ptr;

	public:

		/// Constructor.
		AddShadowRenderer( void );

		/** Composite the two textures, they sjould contain color+depth information in the alpha channel.
		\param backgroundTextureID handle of the background image
		\param foregroundTextureID handle of the foreground image
		\param textureSize the texture size (should be the same)
		\param camera the viewpoint used
		\param dst the destination rendertarget
		*/
		void	process(
			/*input*/	uint backgroundTextureID,
						uint foregroundTextureID,
						const Vector2f& textureSize,
						const Camera& camera,
			/*output*/	IRenderTarget& dst );

	private:

		GLShader			_shader; ///< Composite shader.
		GLParameter			_paramInvProj; ///< Inverse proj matrix uniform.
		GLParameter			_paramImgSize; ///< Image size uniform.

	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDERER_ADDSHADOWRENDERER_HPP__
