#ifndef __SIBR_EXP_ULR_RENRDERER_HPP__
# define __SIBR_EXP_ULR_RENRDERER_HPP__

# include "Config.hpp"
# include <core/system/Config.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Shader.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/renderer/RenderMaskHolder.hpp>
# include <core/scene/BasicIBRScene.hpp>

namespace sibr { 
	class SIBR_EXP_ULR_EXPORT ULRRenderer : public RenderMaskHolder
	{
		SIBR_CLASS_PTR(ULRRenderer);

		//ULRRenderer(const sibr::BasicIBRScene::Ptr& ibrScene );
		ULRRenderer(const uint w, const uint h);

		void process(std::vector<uint>& imgs_ulr, const sibr::Camera& eye,
			const sibr::BasicIBRScene::Ptr scene,
			std::shared_ptr<sibr::Mesh>& altMesh,
			const std::vector<std::shared_ptr<RenderTargetRGBA32F> >& inputRTs,
			IRenderTarget& output);

			void doOccl(bool val) { _doOccl = val; }

	private:
		sibr::RenderTargetRGBA32F::Ptr _ulr0_RT;
		sibr::RenderTargetRGBA32F::Ptr _ulr1_RT;
		sibr::RenderTargetRGBA32F::Ptr _depth_RT;

		sibr::GLShader _ulrShaderPass1;
		sibr::GLShader _ulrShaderPass2;
		sibr::GLShader _depthShader;

		sibr::GLParameter _ulrShaderPass1_nCamPos;
		sibr::GLParameter _ulrShaderPass1_iCamPos;
		sibr::GLParameter _ulrShaderPass1_iCamDir;
		sibr::GLParameter _ulrShaderPass1_iCamProj;
		sibr::GLParameter _ulrShaderPass1_occlTest;
		sibr::GLParameter _ulrShaderPass1_masking;
		sibr::GLParameter _depthShader_proj;

		bool	_doOccl;

   };

} /*namespace sibr*/ 
#endif /* define */
