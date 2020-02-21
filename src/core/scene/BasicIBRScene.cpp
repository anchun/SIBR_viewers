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

		BasicIBRScene();
		// parse metadata file
		_data.reset(new ParseData());
		_currentOpts.renderTargets = !noRTs;
		_currentOpts.mesh = !noMesh;

		_data->ParseData::getParsedData(myArgs);
		std::cout << "Number of input Images to read: " << _data->imgInfos().size() << std::endl;

		if (_data->imgInfos().size() != _data->numCameras())
			SIBR_ERR << "List Image file size do not match number of input cameras in Bundle file!" << std::endl;

		if (_data->datasetType() != ParseData::Type::EMPTY) {
			createFromData(myArgs.texture_width);
		}
	}

	BasicIBRScene::BasicIBRScene(const BasicIBRAppArgs& myArgs, SceneOptions myOpts)
	{
		BasicIBRScene();
		_currentOpts = myOpts;

		// parse metadata file
		_data.reset(new ParseData());


		_data->ParseData::getParsedData(myArgs);
		std::cout << "Number of input Images to read: " << _data->imgInfos().size() << std::endl;

		if (_data->imgInfos().size() != _data->numCameras())
			SIBR_ERR << "List Image file size do not match number of input cameras in Bundle file!" << std::endl;

		if (_data->datasetType() != ParseData::Type::EMPTY) {
			createFromData(myArgs.texture_width);
		}
	}

	void BasicIBRScene::createFromCustomData(const ParseData::Ptr & data, const uint width, BasicIBRScene::SceneOptions myOpts)
	{
		_data = data;
		_currentOpts = myOpts;
		createFromData(width);
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

	void BasicIBRScene::createFromData(const uint width)
	{
		_cams.reset(new CalibratedCameras());
		_imgs.reset(new InputImages());
		_proxies.reset(new ProxyMesh());
		_renderTargets.reset(new RenderTargetTextures(width));

		// setup calibrated cameras
		if (_currentOpts.cameras) {
			
			_cams->sibr::CalibratedCameras::setupFromData(_data);

			std::cout << "Number of Cameras set up: " << _cams->inputCameras().size() << std::endl;
		}

		// load input images

		if (_currentOpts.images) {
			_imgs->InputImages::loadFromData(_data);
			std::cout << "Number of Images loaded: " << _imgs->inputImages().size() << std::endl;
		}

		if (_currentOpts.mesh) {
			// load proxy
			_proxies->ProxyMesh::loadFromData(_data);


			std::vector<InputCamera> inCams = _cams->inputCameras();
			float eps = 0.1f;
			if (inCams.size() > 0 && (abs(inCams[0].znear() - 0.1) < eps || abs(inCams[0].zfar() - 1000.0) < eps || abs(inCams[0].zfar() - 100.0) < eps)) {
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

		if (_currentOpts.renderTargets) {
			createRenderTargets();
		}
	}
	
}
