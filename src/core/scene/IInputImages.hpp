#pragma once

#include "core/scene/Config.hpp"
#include "core/scene/IParseData.hpp"

namespace sibr
{
	/** 
	\ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT IInputImages {
		SIBR_DISALLOW_COPY(IInputImages);
	public:


		typedef std::shared_ptr<IInputImages>				Ptr;
		const std::vector<sibr::ImageRGB::Ptr>&				inputImages(void) const;

		virtual void										loadFromData(const IParseData::Ptr & data) = 0;
		virtual void										loadFromExisting(const std::vector<sibr::ImageRGB::Ptr> & imgs);
		virtual void										loadFromExisting(const std::vector<sibr::ImageRGB> & imgs) = 0;
		virtual void										loadFromPath(const IParseData::Ptr & data, const std::string & prefix, const std::string & postfix) = 0;

		// Alpha blend and modify input images -- for fences
		virtual void										alphaBlendInputImages(const std::vector<sibr::ImageRGB>& back, std::vector<sibr::ImageRGB>& alphas) = 0;

		/// \todo GD: TODO should probably check bounds ?
		const	sibr::ImageRGB& 	image(uint i)	{	return *_inputImages[i]; }

	protected:
		IInputImages() {};

		std::vector<sibr::ImageRGB::Ptr>							_inputImages;

	};

	inline void IInputImages::loadFromExisting(const std::vector<sibr::ImageRGB::Ptr>& imgs)
	{
		_inputImages = imgs;
	}

	inline const std::vector<sibr::ImageRGB::Ptr>& IInputImages::inputImages(void) const {
		return _inputImages;
	}

}
