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


	class SIBR_VIEW_EXPORT RTTextureSize {

	public:
		RTTextureSize(uint w = 0) : _width(w) {}

		void initSize(uint w, uint h);

		bool isInit() const;

	protected:
		uint		_width = 0; //constrained width provided by the command line args, defaults to 0
		uint		_height = 0; //associated height, computed in initializeRenderTargetSize
		bool		_isInit = false;

	};


	class SIBR_VIEW_EXPORT RGBDInputTextures : public virtual RTTextureSize {

	public:
		const std::vector<RenderTargetRGBA32F::Ptr> & inputImagesRT() const;

		void initializeImageRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs);
		void initializeDepthRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, bool facecull);

	protected:
		std::vector<RenderTargetRGBA32F::Ptr> _inputRGBARenderTextures;

	};


	class SIBR_VIEW_EXPORT DepthInputTextureArray : public virtual RTTextureSize {

	public:
		void initDepthTextureArrays(CalibratedCameras::Ptr cams, ProxyMesh::Ptr proxies, bool facecull);
		const Texture2DArrayLum32F::Ptr &  getInputDepthMapArrayPtr() const;

	protected:
		Texture2DArrayLum32F::Ptr _inputDepthMapArrayPtr;

	};

	class SIBR_VIEW_EXPORT RGBInputTextureArray : public virtual RTTextureSize {

	public:
		void initRGBTextureArrays(InputImages::Ptr imgs, int flags = 0);
		const Texture2DArrayRGB::Ptr & getInputRGBTextureArrayPtr() const;

	protected:
		Texture2DArrayRGB::Ptr _inputRGBArrayPtr;

	};


	class SIBR_VIEW_EXPORT RenderTargetTextures :
		public virtual RGBDInputTextures,
		public virtual DepthInputTextureArray,
		public virtual RGBInputTextureArray 
	{
		
	public:
		using Ptr = std::shared_ptr<RenderTargetTextures>;
		
		RenderTargetTextures(uint w = 0) : RTTextureSize(w) {}

		void initRGBandDepthTextureArrays(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, int flags);
		void initializeDefaultRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies);

	};

}
