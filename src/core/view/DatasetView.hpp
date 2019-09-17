#pragma once

#include "MultiViewManager.hpp"
#include "SceneDebugView.hpp"
#include "ImagesGrid.hpp"
#include "core/scene/BasicIBRScene.hpp"

namespace sibr {

	struct ReprojectionData {
	
		operator bool() const { return active; }

		std::vector<MVpixel> repros;
		MVpixel image_input;

		Vector3f point3D;
		bool occlusionTest = true;
		bool active = false;
	};

	class SIBR_VIEW_EXPORT DatasetView 
		: public MultiViewBase
	{
		SIBR_CLASS_PTR(DatasetView);

	public:
		DatasetView(const BasicIBRScene & scene, const Vector2u & defaultRenderingRes = { 0,0 }, const Vector2i & defaultViewRes = { 800, 600 });


		enum ReprojectionMode { NONE, IMAGE_TO_IMAGE, MESH_TO_IMAGE };
		virtual void	onGui(Window& win) override;
		virtual void	onUpdate(Input& input) override;
		virtual void	onRender(Window& win) override;

	protected:
		void repro(ReprojectionData & data);
		void displayRepro(const ReprojectionData & data);

		BasicSubView & getMeshView();
		BasicSubView & getGridView();
		MultiMeshManager::Ptr getMMM();
		ImagesGrid::Ptr getGrid();

		MeshData & proxyData();

		std::vector<RaycastingCamera> cams;
		ReprojectionData currentRepro;
		ReprojectionMode reproMode = MESH_TO_IMAGE;

		const std::string meshSubViewStr = "dataset view - mesh";
		const std::string gridSubViewStr = "grid";

		
	};
}