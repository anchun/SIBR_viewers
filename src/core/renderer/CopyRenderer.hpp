
#ifndef __SIBR_EXP_RENDERER_COPYRENDERER_HPP__
# define __SIBR_EXP_RENDERER_COPYRENDERER_HPP__

# include <core/graphics/Window.hpp>
# include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 

	/**
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT CopyRenderer
	{
	public:
		typedef std::shared_ptr<CopyRenderer>	Ptr;

	public:
		CopyRenderer(
			const std::string& vertFile = sibr::getBinDirectory() + "/shaders_rsc/noproj.vert",
			const std::string& fragFile = sibr::getBinDirectory() + "/shaders_rsc/copy.frag"
		);

		// Copy input texture to the output texture
		// * Copy also input's alpha into depth
		// Note: should have settings for specifying this
		void	process(
			/*input*/	uint textureID,
			/*output*/	IRenderTarget& dst,
			bool disableTest=true);

		void	copyToWindow(
			/*input*/	uint textureID,
			/*output*/	Window& dst);

		bool & flip() { return _flip.get(); }

	private:
		GLShader			_shader;
		GLuniform<bool>		_flip = false;
	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDERER_COPYRENDERER_HPP__
