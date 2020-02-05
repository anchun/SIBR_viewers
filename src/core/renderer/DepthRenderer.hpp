#ifndef __SIBR_ASSETS_DEPTHRENDER_HPP__
#define __SIBR_ASSETS_DEPTHRENDER_HPP__

# include <core/renderer/Config.hpp>
# include "core/assets/InputCamera.hpp"
# include "core/graphics/Texture.hpp"
# include "core/graphics/Camera.hpp"
# include "core/graphics/RenderUtility.hpp"
# include "core/assets/Resources.hpp"
# include "core/graphics/Shader.hpp"
# include "core/graphics/Mesh.hpp"


namespace sibr
{

	/**
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT DepthRenderer
	{
	
	public:
		using Ptr = std::shared_ptr<DepthRenderer>;

		DepthRenderer(int w,int h)  ;
		~DepthRenderer();

		void render( const sibr::InputCamera &cam, const Mesh& mesh, bool backFaceCulling=false, bool frontFaceCulling=false);

		std::shared_ptr<sibr::RenderTargetLum32F> _depth_RT;

		
	private:
		sibr::GLShader				_depthShader;
		sibr::GLParameter			_depthShader_MVP;

	};

} // namespace

#endif
