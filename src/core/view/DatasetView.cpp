#include "DatasetView.hpp"

namespace sibr {
	
	DatasetView::DatasetView(const BasicIBRScene & scene, const Vector2i & defaultViewRes)
		: MultiViewBase(defaultViewRes)
	{
		const std::string mmm_str = "mesh";
		MultiMeshManager::Ptr mmm(new MultiMeshManager(mmm_str));
		mmm->addMesh("proxy", scene.proxies()->proxyPtr());
		mmm->getCameraHandler().fromCamera(scene.cameras()->inputCameras()[0]);

		const std::string grid_str = "grid";
		ImagesGrid::Ptr grid(new ImagesGrid());
		grid->addImageLayer("input images", scene.images()->inputImages());
	
		addSubView("dataset view - " + mmm_str, mmm);
		addSubView("dataset view - " + grid_str, grid);
	}
}

