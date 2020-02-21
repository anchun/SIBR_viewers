#include "CalibratedCameras.hpp"

namespace sibr {
	void CalibratedCameras::setupFromData(const sibr::ParseData::Ptr& data)
	{


		_inputCameras.resize(data->numCameras());
		_inputCameras = data->cameras();
		return;
	}

	void CalibratedCameras::setupCamerasFromExisting(const std::vector<InputCamera>& cams)
	{
		_inputCameras = cams;
	}

	void CalibratedCameras::debugFlagCameraAsUsed(const std::vector<uint>& selectedCameras)
	{
		// Used for Debugging -- Display colored cameras in TopView
		std::vector<bool> cameraUsed(inputCameras().size(), false);
		for (uint usedID : selectedCameras)
			cameraUsed[usedID] = true;
		usedCameraForRendering(cameraUsed);

	}

	const void CalibratedCameras::updateNearsFars(std::vector<Vector2f> & nearsFars) 
	{
		if (_inputCameras.size() != nearsFars.size())
			SIBR_ERR << "InputCamera size does not match Clipping Planes size!" << std::endl;

		for (int c = 0; c < _inputCameras.size(); c++){
			_inputCameras[c].znear(nearsFars[c].x());
			_inputCameras[c].zfar(nearsFars[c].y());
		}
	}

	

	
}