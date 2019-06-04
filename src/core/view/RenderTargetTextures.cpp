#include "RenderTargetTextures.hpp"

namespace sibr {

	void RenderTargetTextures::initializeRenderTargetSize(CalibratedCameras::Ptr cams) {

		if (_width == 0) { // use full resolution
			_width = cams->inputCameras()[0].w();
			_height = cams->inputCameras()[0].h();
		}
		else { // use constrained resolution
			_height = floor(_width / cams->inputCameras()[0].aspect());
		}
	}

	void RenderTargetTextures::initializeDefaultRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies)
	{
		initializeRenderTargetSize(cams);
		initializeImageRenderTargets(cams, imgs);
		initializeDepthRenderTargets(cams, imgs, proxies, true);
	}

	void RenderTargetTextures::initializeImageRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs)
	{
		SIBR_LOG << "Initializing input image RTs " << std::endl;

		_inputRGBARenderTextures.resize(imgs->inputImages().size());

		_imageFitParams = Vector4f(1.f, 1.f, 0.f, 0.f);

		sibr::GLShader textureShader;
		textureShader.init("Texture",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("texture.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("texture.fp")));
		uint interpFlag = (SIBR_SCENE_LINEAR_SAMPLING & SIBR_SCENE_LINEAR_SAMPLING) ? SIBR_GPU_LINEAR_SAMPLING : 0; // LINEAR_SAMPLING Set to default

		for (uint i = 0; i<imgs->inputImages().size(); i++) {
			if (cams->inputCameras()[i].isActive()) {
				ImageRGB img = std::move(imgs->inputImages()[i].clone());
				img.flipH();

				std::shared_ptr<Texture2DRGB> rawInputImage(new Texture2DRGB(img, interpFlag));

				glViewport(0, 0, _width, _height);
				_inputRGBARenderTextures[i].reset(new RenderTargetRGBA32F(_width, _height, interpFlag));
				_inputRGBARenderTextures[i]->clear();
				_inputRGBARenderTextures[i]->bind();

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, rawInputImage->handle());

				glDisable(GL_DEPTH_TEST);
				textureShader.begin();
				sibr::RenderUtility::renderScreenQuad();
				textureShader.end();
				_inputRGBARenderTextures[i]->unbind();
			}
		}
	}

	void RenderTargetTextures::initializeDepthRenderTargets(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, bool facecull)
	{
		sibr::GLParameter size;
		sibr::GLParameter proj;

		sibr::GLShader depthShader;
		depthShader.init("Depth",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("depth.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("depth.fp")));

		proj.init(depthShader, "proj"); // [SP]: ??
		size.init(depthShader, "size"); // [SP]: ??
		for (uint i = 0; i < imgs->inputImages().size(); i++) {
			if (cams->inputCameras()[i].isActive()) {
				_inputRGBARenderTextures[i]->bind();
				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BUFFER_BIT);
				glDepthMask(GL_TRUE);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
				
				if (!proxies->proxy().triangles().empty())
				{
					
					const uint w = _inputRGBARenderTextures[i]->w();
					const uint h = _inputRGBARenderTextures[i]->h();

					depthShader.begin();
					size.set((float)w, (float)h);
					proj.set(cams->inputCameras()[i].viewproj());
					proxies->proxy().render(true, facecull);

					depthShader.end();
				}
				_inputRGBARenderTextures[i]->unbind();
			}
		}
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	void RenderTargetTextures::initRGBandDepthTextureArrays(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies, int flags)
	{
		initializeRenderTargetSize(cams);
		initRGBTextureArrays(imgs, flags);
		initDepthTextureArrays(cams, imgs, proxies);
	}

	void RenderTargetTextures::initRGBTextureArrays(InputImages::Ptr imgs, int flags)
	{
		_inputRGBArrayPtr.reset(new sibr::Texture2DArrayRGB(imgs->inputImages(), _width, _height, flags));
	}

	void RenderTargetTextures::initDepthTextureArrays(CalibratedCameras::Ptr cams, InputImages::Ptr imgs, ProxyMesh::Ptr proxies)
	{
		if (!proxies->hasProxy()) {
			SIBR_WRG << " Cannot init DepthTextureArrays without proxy." << std::endl;
			return;
		}

		sibr::GLShader depthOnlyShader;
		depthOnlyShader.init("DepthOnly",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("depthonly.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("depthonly.fp")));

		const uint interpFlag = (SIBR_SCENE_LINEAR_SAMPLING & SIBR_SCENE_LINEAR_SAMPLING) ? SIBR_GPU_LINEAR_SAMPLING : 0;

		sibr::RenderTargetLum32F depthRT(_width, _height, interpFlag);

		sibr::GLParameter proj;
		proj.init(depthOnlyShader, "proj");

		_inputDepthMapArrayPtr.reset(new sibr::Texture2DArrayLum32F(_width, _height, (uint)imgs->inputImages().size(), SIBR_GPU_LINEAR_SAMPLING));

		for (uint i = 0; i<imgs->inputImages().size(); i++) {
			glViewport(0, 0, _width, _height);

			depthRT.bind();
			glEnable(GL_DEPTH_TEST);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glDepthMask(GL_TRUE);
			
			depthOnlyShader.begin();
			proj.set(cams->inputCameras()[i].viewproj());
			proxies->proxy().render();
			depthOnlyShader.end();

			depthRT.unbind();

			glCopyImageSubData(
				depthRT.handle(), GL_TEXTURE_2D, 0, 0, 0, 0,
				_inputDepthMapArrayPtr->handle(), GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
				_width, _height, 1);
			CHECK_GL_ERROR;
		}
		CHECK_GL_ERROR;
	}
}
