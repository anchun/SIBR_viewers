#include "CalibratedCameras.hpp"

namespace sibr {
	void CalibratedCameras::setupFromData(const sibr::ParseData::Ptr& data)
	{


		_inputCameras.resize(data->numCameras());
		for (int i = 0, infosId = 0, currentIdZnearZfar = 0; i < data->numCameras() && infosId < data->imgInfos().size(); i++) {
			const sibr::ImageListFile::Infos& infos = data->imgInfos()[infosId];
			//if (i != infos.id)
			//	continue;

			_inputCameras[infosId] = InputCamera(infos.camId, infos.width, infos.height, data->outputCamsMatrix()[infosId], true);

			_inputCameras[infosId].name(infos.filename);

			const InputCamera::Z & current_Z = data->nearsFars()[currentIdZnearZfar];
			_inputCameras[infosId].znear(current_Z.near);
			_inputCameras[infosId].zfar(current_Z.far);

			if (!data->activeImages()[infosId])
				_inputCameras[infosId].setActive(false);

			if (data->excludeImages()[infosId])
				_inputCameras[infosId].setActive(false);

			++infosId;
			currentIdZnearZfar = std::min(currentIdZnearZfar + 1, (int)data->nearsFars().size() - 1);
		}


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