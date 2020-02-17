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
	Render a mesh to a depth rendertarget.
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT DepthRenderer
	{
	
	public:
		
		using Ptr = std::shared_ptr<DepthRenderer>;

		/** Constructor with a target size.
		\param w target width
		\param h target height
		*/
		DepthRenderer(int w,int h)  ;
		
		/// Destructor.
		~DepthRenderer();

		/** Render a mesh depth in the result rendertarget.
		\param cam the viewpoint to use
		\param mesh the mesh to render
		\param backFaceCulling should perform backface culling
		\param frontFaceCulling flip culling test orientation
		*/
		void render( const sibr::InputCamera &cam, const Mesh& mesh, bool backFaceCulling=false, bool frontFaceCulling=false);

		std::shared_ptr<sibr::RenderTargetLum32F> _depth_RT; ///< The result depth rendertarget.

	private:

		sibr::GLShader				_depthShader; ///< Depth shader.
		sibr::GLParameter			_depthShader_MVP; ///< Shader MVP.

	};

} // namespace

#endif
