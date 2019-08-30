#include "BasicIBRScene.hpp"
#include <iostream>
#include <string>

namespace sibr
{
	
	BasicIBRScene::BasicIBRScene() {
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

		_userCamera.aspect((float)myArgs.rendering_size.get()[0] / (float)myArgs.rendering_size.get()[1]);
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
			std::vector<sibr::Vector2f>    nearsFars;
			CameraRaycaster::computeClippingPlanes(_proxies->proxy(), inCams, nearsFars);
			_cams->updateNearsFars(nearsFars);
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
