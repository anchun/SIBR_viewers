#pragma once
# include "core/scene/ParseData.hpp"
#include "core/scene/Config.hpp"

namespace sibr
{

	class SIBR_SCENE_EXPORT CalibratedCameras {
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
		void	setupFromData(const ParseData::Ptr & data);

		/**
		* \brief Assigns the calibrated cameras for a scene to a list of cameras passed as parameter.
		*
		* \param cams Vector of type sibr::InputCamera to which the scene inputCameras will be set
		*/
		void	setupCamerasFromExisting(const std::vector<InputCamera> & cams); // 
		
		/**
		* \brief Function to set a camera as active.
		* 
		* \param camId Integer ID of the camera to be set active
		*/
		void	activateCamera(uint camId);

		/**
		* \brief Function to set a camera as inactive.
		*
		* \param camId Integer ID of the camera to be set inactive
		*/
		void	deactivateCamera(uint camId);

		/**
		* \brief Function to mark the cameras used for rendering.
		* Generally used for debugging purposes
		* \param selectedImages Vector of camera IDs that are used for rendering
		*/
		void	debugFlagCameraAsUsed(const std::vector<uint>& selectedCameras);

		/**
		* \brief Function to check if the camera is used for rendering.
		*
		* \param camId Integer ID of the cameras to be checked if it is being used for rendering
		*/
		bool	isCameraUsedForRendering(size_t camId) const;
		
		/**
		* \brief Function to set the cameras used for rendering.
		*
		* \param usedCamera Vector to specify which cameras are used for rendering 
		*/
		void	usedCameraForRendering(const std::vector<bool> usedCamera);

		/**
		* \brief Getter to the vector of input cameras used to create the scene
		*
		*/
		const std::vector<InputCamera>&				inputCameras(void) const;


		const void									updateNearsFars(std::vector<sibr::Vector2f> & nearsFars);
	protected:

		std::vector<InputCamera>					_inputCameras; 
		std::vector<bool>							_usedCameraFlag;

	};
	
	///// INLINE DEFINITIONS /////

	inline const std::vector<InputCamera>& CalibratedCameras::inputCameras( void ) const {
		return _inputCameras;
	}

	inline void CalibratedCameras::activateCamera(uint camId)
	{
		_inputCameras[camId].setActive(true);
	}

	inline void CalibratedCameras::deactivateCamera(uint camId)
	{
		_inputCameras[camId].setActive(false);
	}

	inline bool CalibratedCameras::isCameraUsedForRendering(size_t camId) const
	{
		return (_usedCameraFlag.empty()) ? false : _usedCameraFlag[camId];
	}

	inline void CalibratedCameras::usedCameraForRendering(const std::vector<bool> usedCamera)
	{
		_usedCameraFlag = usedCamera;
	}
}