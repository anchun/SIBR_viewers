#pragma once

#include "core/graphics/Texture.hpp"
#include "core/scene/CalibratedCameras.hpp"
#include "core/scene/InputImages.hpp"
#include "core/scene/ProxyMesh.hpp"
#include "core/assets/Resources.hpp"
# include "core/graphics/Shader.hpp"
#include "core/graphics/Utils.hpp"
#include "core/scene/Config.hpp"


# define SIBR_SCENE_LINEAR_SAMPLING			4


namespace sibr{


	class SIBR_SCENE_EXPORT RTTextureSize {

	public:
		RTTextureSize(uint w = 0) : _width(w) {}

		void initSize(uint w, uint h);

		bool isInit() const;

	protected:
		uint		_width = 0; //constrained width provided by the command line args, defaults to 0
		uint		_height = 0; //associated height, computed in initSize
		bool		_isInit = false;
		int			_initActiveCam = 0;

	};


	class SIBR_SCENE_EXPORT RGBDInputTextures : public virtual RTTextureSize {
		SIBR_CLASS_PTR(RGBDInputTextures)
	public:
		const std::vector<RenderTargetRGBA32F::Ptr> & inputImagesRT() const;

		void initializeImageRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs);
		void initializeDepthRenderTargets(CalibratedCameras::Ptr cams, ProxyMesh::Ptr proxies, bool facecull);

	protected:
		std::vector<RenderTargetRGBA32F::Ptr> _inputRGBARenderTextures;

	};


	class SIBR_SCENE_EXPORT DepthInputTextureArray : public virtual RTTextureSize {
		SIBR_CLASS_PTR(DepthInputTextureArray)
	public:
		void initDepthTextureArrays(CalibratedCameras::Ptr cams, ProxyMesh::Ptr proxies, bool facecull, int flags = SIBR_GPU_LINEAR_SAMPLING);
		const Texture2DArrayLum32F::Ptr &  getInputDepthMapArrayPtr() const;

	protected:
		Texture2DArrayLum32F::Ptr _inputDepthMapArrayPtr;

	};

	class SIBR_SCENE_EXPORT RGBInputTextureArray : public virtual RTTextureSize {

		SIBR_CLASS_PTR(RGBInputTextureArray)

	public:
		void initRGBTextureArrays(InputImages::Ptr imgs, int flags = 0);
		const Texture2DArrayRGB::Ptr & getInputRGBTextureArrayPtr() const;

	protected:
		Texture2DArrayRGB::Ptr _inputRGBArrayPtr;

	};


	class SIBR_SCENE_EXPORT RenderTargetTextures :
		public virtual RGBDInputTextures,
		public virtual DepthInputTextureArray,
		public virtual RGBInputTextureArray 
	{
		
	public:
		SIBR_CLASS_PTR(RenderTargetTextures)
		
		RenderTargetTextures(uint w = 0) : RTTextureSize(w) {}

		void initRGBandDepthTextureArrays(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, int textureFlags, bool faceCull = true);
		void initializeDefaultRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies);

	protected:
		void initRenderTargetRes(CalibratedCameras::Ptr cams);

	};


}
