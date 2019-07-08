#pragma once

#include "MultiViewManager.hpp"
#include "MultiMeshManager.hpp"
#include "ImagesGrid.hpp"
#include "BasicIBRScene.hpp"

namespace sibr {

	class SIBR_VIEW_EXPORT DatasetView 
		: public MultiViewBase
	{
		SIBR_CLASS_PTR(DatasetView);

	public:
		DatasetView(const BasicIBRScene & scene, const Vector2i & defaultViewRes = { 800, 600 });

	protected:

	};
}