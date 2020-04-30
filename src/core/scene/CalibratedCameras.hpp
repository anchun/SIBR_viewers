#pragma once
# include "core/scene/ICalibratedCameras.hpp"
#include "core/scene/Config.hpp"

namespace sibr
{
	/**
	\ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT CalibratedCameras : public ICalibratedCameras {
	public:

		/**
		* \brief Pointer to the instance of class sibr::CalibratedCameras.
		*/
		typedef std::shared_ptr<CalibratedCameras>	Ptr;

		// load from a path on disk in a predefined format (or could detect from file extension)

		/**
		* \brief Creates the calibrated cameras for a scene given data parsed from dataset path.
		* 
		* \param data Holds all information required to created a set of calibrated cameras
		*/
		void	setupFromData(const IParseData::Ptr & data) override;

		/**
		* \brief Function to mark the cameras used for rendering.
		* Generally used for debugging purposes
		* \param selectedCameras list of camera IDs that are used for rendering
		*/
		void	debugFlagCameraAsUsed(const std::vector<uint>& selectedCameras) override;


		const void									updateNearsFars(std::vector<sibr::Vector2f> & nearsFars) override;

	};
}