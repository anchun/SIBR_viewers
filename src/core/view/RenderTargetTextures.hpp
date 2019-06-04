#pragma once

#include "core/graphics/Texture.hpp"
#include "core/view/CalibratedCameras.hpp"
#include "core/view/InputImages.hpp"
#include "core/view/ProxyMesh.hpp"
#include "core/view/Resources.hpp"
# include "core/graphics/Shader.hpp"
#include "core/graphics/Utils.hpp"


# define SIBR_SCENE_LINEAR_SAMPLING			4


namespace sibr{
	class SIBR_VIEW_EXPORT RenderTargetTextures {
		SIBR_DISALLOW_COPY(RenderTargetTextures);
	public:

		typedef std::shared_ptr<RenderTargetTextures>						Ptr;

		RenderTargetTextures() {};
		//void test(void);
		void initializeDefaultRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies);
		void initializeImageRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs);
		void initializeDepthRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, bool facecull);

		// void initRGBandDepthTextureArrays(int flags = 0); 
		void initRGBandDepthTextureArrays(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, uint width, uint height, int flags = 0);
		void initRGBTextureArrays(InputImages::Ptr imgs, uint width, uint height, int flags = 0);
		void initDepthTextureArrays(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies);

		const std::vector<std::shared_ptr<RenderTargetRGBA32F>>											inputImagesRT(void) const;
		const sibr::Texture2DArrayRGB::Ptr & getInputRGBTextureArrayPtr() const { return _inputRGBArrayPtr; }
		const sibr::Texture2DArrayLum32F::Ptr &  getInputDepthMapArrayPtr() const { return _inputDepthMapArrayPtr; }
		~RenderTargetTextures() {};

	protected:

		std::vector<std::shared_ptr<RenderTargetRGBA32F>>					_inputRGBARenderTextures;
		sibr::Texture2DArrayRGB::Ptr										_inputRGBArrayPtr;
		sibr::Texture2DArrayLum32F::Ptr										_inputDepthMapArrayPtr;
		Vector4f															_imageFitParams;
		
	};

	inline const std::vector<std::shared_ptr<RenderTargetRGBA32F>> RenderTargetTextures::inputImagesRT(void) const
	{
		return _inputRGBARenderTextures;
	}
}
