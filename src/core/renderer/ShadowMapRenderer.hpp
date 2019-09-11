#ifndef __SIBR_ASSETS_SHADOWMAPRENDER_HPP__
#define __SIBR_ASSETS_SHADOWMAPRENDER_HPP__

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
	class SIBR_EXP_RENDERER_EXPORT ShadowMapRenderer
	{

	public:
		ShadowMapRenderer(const sibr::InputCamera& depthMapCam, std::shared_ptr<sibr::RenderTargetLum32F> _depth_RT)  ;
		~ShadowMapRenderer();

		void render(int w, int h,const sibr::InputCamera &cam, const Mesh& mesh, float bias= 0.0005f);

		std::shared_ptr<sibr::RenderTargetLum> _shadowMap_RT;
		std::shared_ptr<sibr::RenderTargetLum32F> _depthMap_RT;

		
	private:
		sibr::GLShader				_shadowMapShader;
		sibr::GLParameter			_shadowMapShader_MVP;
		sibr::GLParameter			_depthMap_MVP;
		sibr::GLParameter			_depthMap_MVPinv;
		sibr::GLParameter			_depthMap_radius;
		sibr::GLParameter			_lightDir;
		sibr::GLParameter			_bias_control;
		sibr::GLParameter			_sun_app_radius;
		
		//sibr::GLParameter			_winDims;
		std::shared_ptr<sibr::Texture2DLum32F>			_textureDepthMap;

	};

} // namespace

#endif
