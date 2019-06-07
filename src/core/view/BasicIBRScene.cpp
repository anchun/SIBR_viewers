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

	BasicIBRScene::BasicIBRScene(const BasicIBRAppArgs & myArgs, bool noRTs)
	{

		// parse metadata file
		_data.reset(new ParseData());
		

		_data->ParseData::getParsedData(myArgs.dataset_path, myArgs.scene_metadata_filename);
		std::cout << "Number of input Images to read: " << _data->imgInfos().size() << std::endl;

		if (_data->imgInfos().size() != _data->numCameras())
			SIBR_ERR << "List Image file size do not match number of input cameras in Bundle file!" << std::endl;

		createFromData(noRTs, myArgs.texture_width);

		_userCamera.aspect((float)myArgs.rendering_size.get()[0] / (float)myArgs.rendering_size.get()[1]);
	}

	void BasicIBRScene::createFromCustomData(const ParseData::Ptr & data, bool noRTs, const uint width)
	{
		_data = data;
		createFromData(noRTs, width);
	}

	void BasicIBRScene::createFromData(bool noRTs, const uint width)
	{
		// setup calibrated cameras
		_cams.reset(new CalibratedCameras());
		_cams->sibr::CalibratedCameras::setupFromData(_data);
		std::cout << "Number of Cameras set up: " << _cams->inputCameras().size() << std::endl;

		// load input images
		_imgs.reset(new InputImages());
		_imgs->InputImages::loadFromData(_data);
		std::cout << "Number of Images loaded: " << _imgs->inputImages().size() << std::endl;

		// load proxy
		_proxies.reset(new ProxyMesh());
		_proxies->ProxyMesh::loadFromData(_data);

		_renderTargets.reset(new RenderTargetTextures(width));
		if (!noRTs) {
			createRenderTargets();
		}
	}

	void BasicIBRScene::createRenderTargets()
	{
		_renderTargets->initializeDefaultRenderTargets(_cams, _imgs, _proxies);
	}

	
}
