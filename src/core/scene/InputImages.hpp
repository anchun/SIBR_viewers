#pragma once

# include "core/scene/ParseData.hpp"
#include "core/scene/Config.hpp"

namespace sibr
{
	class SIBR_SCENE_EXPORT InputImages {
		SIBR_DISALLOW_COPY(InputImages);
	public:


		typedef std::shared_ptr<InputImages>				Ptr;
		const std::vector<sibr::ImageRGB::Ptr>&				inputImages(void) const;

		InputImages::InputImages(){};
		void												loadFromData(const ParseData::Ptr & data);
		void												loadFromExisting(const std::vector<sibr::ImageRGB::Ptr> & imgs);
		void												loadFromExisting(const std::vector<sibr::ImageRGB> & imgs);
		void												loadFromPath(const ParseData::Ptr & data, const std::string & prefix, const std::string & postfix);

		// Alpha blend and modify input images -- for fences
		void									alphaBlendInputImages(const std::vector<sibr::ImageRGB>& back, std::vector<sibr::ImageRGB>& alphas);

		~InputImages(){};

		/// \todo GD: TODO should probably check bounds ?
		const	sibr::ImageRGB& 	image(uint i)	{	return *_inputImages[i]; }

	protected:

		std::vector<sibr::ImageRGB::Ptr>							_inputImages;

	};

	inline const std::vector<sibr::ImageRGB::Ptr>& InputImages::inputImages(void) const {
		return _inputImages;
	}

}
