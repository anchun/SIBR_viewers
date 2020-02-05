#include "BasicIBRScene.hpp"
#include <iostream>
#include <string>

namespace sibr
{
	
	BasicIBRScene::BasicIBRScene() {
		_data.reset(new ParseData());
		_cams.reset(new CalibratedCameras());
		_imgs.reset(new InputImages());
		_proxies.reset(new ProxyMesh());
		_renderTargets.reset(new RenderTargetTextures());
	}

	BasicIBRScene::BasicIBRScene(const BasicIBRAppArgs & myArgs, bool noRTs, bool noMesh)
	{

		_cams.reset(new CalibratedCameras());
		_imgs.reset(new InputImages());
		_proxies.reset(new ProxyMesh());
		_renderTargets.reset(new RenderTargetTextures());

		// parse metadata file
		_data.reset(new ParseData());
		

		_data->ParseData::getParsedData(myArgs);
		std::cout << "Number of input Images to read: " << _data->imgInfos().size() << std::endl;

		if (_data->imgInfos().size() != _data->numCameras())
			SIBR_ERR << "List Image file size do not match number of input cameras in Bundle file!" << std::endl;

		if (_data->datasetType() != ParseData::Type::EMPTY) {
			createFromData(noRTs, myArgs.texture_width, noMesh);
		}
	}

	void BasicIBRScene::createFromCustomData(const ParseData::Ptr & data, bool noRTs, const uint width)
	{
		_data = data;
		createFromData(noRTs, width);
	}

	void BasicIBRScene::createFromData(bool noRTs, const uint width, bool noMesh)
	{
		// setup calibrated cameras
		_cams.reset(new CalibratedCameras());

		//_cams->sibr::CalibratedCameras::setupFromData(_data);
			
		//_cams->sibr::CalibratedCameras::setupFromData(_data);
		if (_data->datasetType() != ParseData::Type::NVM) {
			_cams->sibr::CalibratedCameras::setupFromData(_data);
		}
		else {
			std::vector<Vector2u> imgSizes(_data->imgInfos().size());
			for (int i = 0; i < _data->imgInfos().size(); i++) {
				imgSizes[i].x() = _data->imgInfos()[i].width;
				imgSizes[i].y() = _data->imgInfos()[i].height;
			}
			_cams->setupCamerasFromExisting(sibr::InputCamera::loadNVM(_data->basePathName() + "/scene.nvm", 0.001f, 1000.0f, imgSizes));
		}

		std::cout << "Number of Cameras set up: " << _cams->inputCameras().size() << std::endl;

		// load input images
		_imgs.reset(new InputImages());
		_imgs->InputImages::loadFromData(_data);
		std::cout << "Number of Images loaded: " << _imgs->inputImages().size() << std::endl;

		if (!noMesh) {
			// load proxy
			_proxies.reset(new ProxyMesh());
			_proxies->ProxyMesh::loadFromData(_data);


			std::vector<InputCamera> inCams = _cams->inputCameras();
			float eps = 0.1f;
			if (inCams.size() > 0 && (abs(inCams[0].znear() - 0.1) < eps || abs(inCams[0].zfar() - 1000.0) < eps || abs(inCams[0].zfar() - 100.0 ) < eps)) {
				std::vector<sibr::Vector2f>    nearsFars;
				CameraRaycaster::computeClippingPlanes(_proxies->proxy(), inCams, nearsFars);
				_cams->updateNearsFars(nearsFars);
			}

			//// Load the texture.
			//sibr::ImageRGB inputTextureImg;
			//if (sibr::fileExists(_data->texImgPath())) {
			//	inputTextureImg.load(_data->texImgPath());
			//	_inputMeshTexture.reset(new sibr::Texture2DRGB(inputTextureImg, SIBR_GPU_LINEAR_SAMPLING));
			//}
			
		}


		_renderTargets.reset(new RenderTargetTextures(width));
		if (!noRTs) {
			createRenderTargets();
		}
	}

	void BasicIBRScene::createRenderTargets()
	{
		_renderTargets->initializeDefaultRenderTargets(_cams, _imgs, _proxies);
	}

	BasicIBRScene::BasicIBRScene(BasicIBRScene & scene)
	{
		_data = scene.data();
		_cams = scene.cameras();
		_imgs = scene.images();
		_proxies = scene.proxies();
		_renderTargets = scene.renderTargets();
	}
	
}
