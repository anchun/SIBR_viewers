#pragma once

#include "core/scene/IInputImages.hpp"
#include "core/scene/Config.hpp"

namespace sibr
{
	/** 
	\ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT InputImages : public IInputImages {
		SIBR_DISALLOW_COPY(InputImages);
	public:


		typedef std::shared_ptr<InputImages>				Ptr;

		InputImages::InputImages(){};
		void												loadFromData(const IParseData::Ptr & data) override;
		void												loadFromExisting(const std::vector<sibr::ImageRGB> & imgs) override;
		void												loadFromPath(const IParseData::Ptr & data, const std::string & prefix, const std::string & postfix) override;

		// Alpha blend and modify input images -- for fences
		void									alphaBlendInputImages(const std::vector<sibr::ImageRGB>& back, std::vector<sibr::ImageRGB>& alphas) override;

		~InputImages(){};

	};

}
