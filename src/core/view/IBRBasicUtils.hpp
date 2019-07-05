#pragma once

#include "core/view/Config.hpp"
#include "core/graphics/Utils.hpp"
#include "core/graphics/Camera.hpp"
#include "core/graphics/Image.hpp"
#include "core/assets/InputCamera.hpp"
#include <map>

namespace sibr{

	class SIBR_VIEW_EXPORT IBRBasicUtils
	{

	public:
		
		static std::vector<uint>						selectCameras(const std::vector<InputCamera>& cams, const sibr::Camera& eye, uint count);

		static std::vector<uint>						selectCamerasSimpleDist(const std::vector<InputCamera>& cams, const sibr::Camera& eye, uint count);

	protected:
		
		static std::vector<uint>						selectCamerasAngleWeight(const std::vector<InputCamera>& cams, const sibr::Camera& eye, uint count);

	};

}