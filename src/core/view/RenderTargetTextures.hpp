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
		
	public:

		typedef std::shared_ptr<RenderTargetTextures>						Ptr;

		RenderTargetTextures() = default;

		RenderTargetTextures(const uint w) : _width(w) {}

		void initializeDefaultRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies);
		

		void initRGBandDepthTextureArrays(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, int flags = 0);
	

		const std::vector<std::shared_ptr<RenderTargetRGBA32F>>											inputImagesRT(void) const;
		const sibr::Texture2DArrayRGB::Ptr & getInputRGBTextureArrayPtr() const { return _inputRGBArrayPtr; }
		const sibr::Texture2DArrayLum32F::Ptr &  getInputDepthMapArrayPtr() const { return _inputDepthMapArrayPtr; }
		

	protected:

		void initializeRenderTargetSize(CalibratedCameras::Ptr cams);

		void initializeImageRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs);
		void initializeDepthRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, bool facecull);

		void initRGBTextureArrays(InputImages::Ptr imgs, int flags = 0);
		void initDepthTextureArrays(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies);

		std::vector<std::shared_ptr<RenderTargetRGBA32F>>					_inputRGBARenderTextures;
		sibr::Texture2DArrayRGB::Ptr										_inputRGBArrayPtr;
		sibr::Texture2DArrayLum32F::Ptr										_inputDepthMapArrayPtr;
		Vector4f															_imageFitParams;
		uint																_width = 0; //constrained width provided by the command line args, defaults to 0
		uint																_height = 0; //associated height, computed in initializeRenderTargetSize
	};

	inline const std::vector<std::shared_ptr<RenderTargetRGBA32F>> RenderTargetTextures::inputImagesRT(void) const
	{
		return _inputRGBARenderTextures;
	}
}
