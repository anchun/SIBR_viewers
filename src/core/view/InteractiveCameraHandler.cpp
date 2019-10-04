#include "InteractiveCameraHandler.hpp"
#include "core/graphics/Input.hpp"
#include "core/graphics/Viewport.hpp"
#include "core/graphics/Window.hpp"
#include "core/raycaster/Raycaster.hpp"
#include "core/view/UIShortcuts.hpp"
#include "core/graphics/GUI.hpp"

# define IBRVIEW_SMOOTHCAM_POWER	0.1f
# define IBRVIEW_USESMOOTHCAM		true
# define SIBR_INTERPOLATE_FRAMES    30


namespace sibr {

	InteractiveCameraHandler::InteractiveCameraHandler(const bool supportRecording) : _trackball(true) {
		_currentMode = FPS;
		_shouldSmooth = IBRVIEW_USESMOOTHCAM;
		_startCam = 0;
		_interpFactor = 0;
		_shouldSnap = false;
		_supportRecording = supportRecording;
		_radius = 100.0f;
		_currentCamId = 0;
		_saveFrame = 0;
		_saveFrameDebug = 0;
		_viewport = Viewport(0, 0, 0, 0);
		_triggerCameraUpdate = false;
		_isSetup = false;

		sibr::UIShortcuts::global().add("[Camera] b", "orbit mode");
		sibr::UIShortcuts::global().add("[Camera] y", "trackball mode");
		sibr::UIShortcuts::global().add("[Camera] v", "interpolation mode");
		sibr::UIShortcuts::global().add("[Camera] maj+y", "show/hide trackball");
		if (_supportRecording) {
			sibr::UIShortcuts::global().add("c", "playback camera path");
			sibr::UIShortcuts::global().add("ctrl+c", "save camera path (enter filename in the prompt)");
			sibr::UIShortcuts::global().add("shift+c", "load camera path (enter filename in the prompt)");
			sibr::UIShortcuts::global().add("alt+c", "start recording camera path");
		}


	}

	// save default camera for a scene
	void InteractiveCameraHandler::saveDefaultCamera(const std::string& datasetPath)
	{
		std::string selectedFile = datasetPath;

		selectedFile.append("/default_camera.bin");
		_currentCamera.saveToBinary(selectedFile);
		SIBR_LOG << "Saved camera (" << selectedFile << ")." << std::endl;
	}

	void InteractiveCameraHandler::loadDefaultCamera(const sibr::InputCamera& cam, const std::string& datasetPath)
	{
		sibr::InputCamera savedCam;
		std::ifstream camFile(datasetPath + "/default_camera.bin");
		fromCamera(cam, false);
		if (camFile.good()) {
			savedCam.loadFromBinary(datasetPath + "/default_camera.bin");
			SIBR_LOG << "Loaded  " << datasetPath << "/default_camera.bin" << std::endl;
			fromCamera(savedCam, false);
		}
	}

	void InteractiveCameraHandler::setup(const std::vector<sibr::InputCamera>& cams, const sibr::Vector2u & resolution, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster)
	{
		setup(cams, viewport, raycaster);

	}

	void InteractiveCameraHandler::setup(const sibr::InputCamera & cam, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster) {
		_raycaster = raycaster;
		_viewport = viewport;
		fromCamera(cam, false);
	}

	void InteractiveCameraHandler::setup(const Eigen::AlignedBox<float, 3> & areaOfInterest, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster)
	{
		_raycaster = raycaster;
		_viewport = viewport;
		// Use the trackball to compute an initial camera.
		_trackball.fromBoundingBox(areaOfInterest, viewport);
		fromCamera(_trackball.getCamera(), false);
	}

	void InteractiveCameraHandler::setup(const std::vector<sibr::InputCamera>& cams, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster, const sibr::Vector2f & clippingPlanes) {

		// setup interpolation path if not set
		if (_interpPath.empty()) {
			setupInterpolationPath(cams);
		}
		// Update the near and far planes.
		sibr::InputCamera idealCam = cams[0];
		if(clippingPlanes[0] < 0.0f || clippingPlanes[1] < 0.0f) {
			float zFar = -1.0f, zNear = -1.0f;
			for (const auto & cam : cams) {
				zFar = (zFar<0 || cam.zfar() > zFar ? cam.zfar() : zFar);
				zNear = (zNear < 0 || cam.znear() < zNear ? cam.znear() : zNear);
			}
			idealCam.zfar(zFar*1.1f);
			idealCam.znear(zNear*0.9f);
		} else {
			idealCam.znear(clippingPlanes[0]);
			idealCam.zfar(clippingPlanes[1]);
		}
		
		SIBR_LOG << "Interactive camera using (" << idealCam.znear() << "," << idealCam.zfar() << ") near/far planes." << std::endl;

		setup(idealCam, viewport, raycaster);
	}

	void InteractiveCameraHandler::setup(const std::shared_ptr<sibr::Mesh> mesh, const sibr::Viewport & viewport) {
		_raycaster = std::make_shared<sibr::Raycaster>();
		_raycaster->addMesh(*mesh);
		_viewport = viewport;
		_trackball.fromBoundingBox(mesh->getBoundingBox(), viewport);
		_radius = mesh->getBoundingBox().diagonal().norm();
		fromCamera(_trackball.getCamera(), false);
	}

	void InteractiveCameraHandler::fromCamera(const sibr::InputCamera & cam, bool interpolate, bool updateResolution) {
		_isSetup = true;

		sibr::InputCamera idealCam(cam);
		if (updateResolution) {
			// Viewport might have not been set, in this case defer the full camera update 
			// until after the viewport has been updated, ie in onUpdate().
			if (_viewport.isEmpty()) {
				_triggerCameraUpdate = true;
			}
			else {
				const float w = _viewport.finalWidth();
				const float h = _viewport.finalHeight();
				idealCam.size(uint(w), uint(h));
				idealCam.aspect(w / h);
			}
		}

		_orbit.fromCamera(idealCam, _raycaster);
		_fpsCamera.fromCamera(idealCam);


		if (_raycaster != nullptr) {
			sibr::RayHit hit = _raycaster->intersect(sibr::Ray(idealCam.position(), idealCam.dir()));
			// If hit at the proxy surface, save the distance between the camera and the mesh, to use as a trackball radius.
			if (hit.hitSomething()) {
				_radius = hit.dist();
			}
		}
		_trackball.fromCamera(idealCam, _viewport, _radius);

		_currentCamera = idealCam;
		_cameraFovDeg = _currentCamera.fovy() * 180.0f / float(M_PI);

		if (!interpolate) {
			_previousCamera = _currentCamera;
		}

		_clippingPlanes[0] = _currentCamera.znear();
		_clippingPlanes[1] = _currentCamera.zfar();
	}

	void InteractiveCameraHandler::fromTransform(const Transform3f & transform, bool interpolate, bool updateResolution)
	{
		InputCamera camCopy = getCamera();
		camCopy.transform(transform);
		fromCamera(camCopy, interpolate, updateResolution);
	}

	void InteractiveCameraHandler::setClippingPlanes(float znear, float zfar) {
		if (znear > 0.0f) {
			_clippingPlanes[0] = znear;
		}
		if (zfar > 0.0f) {
			_clippingPlanes[1] = zfar;
		}
		_currentCamera.znear(_clippingPlanes[0]);
		_currentCamera.zfar(_clippingPlanes[1]);
		fromCamera(_currentCamera);
	}

	void InteractiveCameraHandler::switchMode(const InteractionMode mode) {
		if (_currentMode == mode) {
			return;
		}
		_currentMode = mode;

		// Synchronize internal cameras.
		fromCamera(_currentCamera, _shouldSmooth);

		_interpFactor = 0;

		std::cout << "Switched to ";
		switch (_currentMode) {
		case ORBIT:
			std::cout << "orbit";
			break;
		case INTERPOLATION:
			std::cout << "interpolation";
			break;
		case TRACKBALL:
			std::cout << "trackball";
			break;
		case NONE:
			std::cout << "none";
			break;
		case FPS:
		default:
			std::cout << "fps&pan";
			break;
		}
		std::cout << " mode." << std::endl;

	}

	int	InteractiveCameraHandler::findNearestCamera(const std::vector<sibr::InputCamera>& inputCameras) const
	{

		const sibr::Vector3f currentPos = _currentCamera.position();

		if (inputCameras.size() == 0)
			return -1;

		int selectedCam = 0;
		float dist = (inputCameras[0].position() - currentPos).norm();
		for (uint i = 1; i < inputCameras.size(); ++i)
		{
			if (inputCameras[i].isActive() == false)
				continue;

			float d = (inputCameras[i].position() - currentPos).norm();
			if (d < dist)
			{
				dist = d;
				selectedCam = i;
			}
		}
		return selectedCam;
	}

	void InteractiveCameraHandler::setupInterpolationPath(const std::vector<sibr::InputCamera> & cameras) {
		_interpPath.resize(cameras.size());

		bool defaultPath = false;
		for (int i = 0; i < cameras.size(); i++) {
			if (cameras[i].isActive()) {
				if (cameras[i].id() < cameras.size()) {
					_interpPath[cameras[i].id()] = cameras[i];
				}
				else {
					std::cout << "Cameras ID inconsistent. Setting default interpolation path." << std::endl;
					defaultPath = true;
					break;
				}
			}
		}

		if (defaultPath) {
			_interpPath.clear();
			for (int i = 0; i < cameras.size(); i++) {
				if (cameras[i].isActive()) {
					_interpPath.push_back(cameras[i]);
				}
			}
			std::sort(_interpPath.begin(), _interpPath.end(), [](const InputCamera & a, const InputCamera & b) {
				return a.id() < b.id();
			});
		}
	}

	void InteractiveCameraHandler::interpolate() {
		if (_interpPath.empty()) {
			return;
		}

		// If we reach the last frame of the interpolation b/w two cameras, skip to next camera.
		if (_interpFactor == SIBR_INTERPOLATE_FRAMES - 1)
		{
			_interpFactor = 0;
			_startCam++;
		}

		// If we reach the last camera, restart the interpolation.
		if (_startCam >= _interpPath.size() - 1) {
			_interpFactor = 0;
			_startCam = 0;
		}

		float k = std::min(std::max(((_interpFactor) / (float)SIBR_INTERPOLATE_FRAMES), 1e-6f), 1.0f - 1e-6f);

		sibr::InputCamera & camStart = _interpPath[_startCam];
		sibr::InputCamera & camNext = _interpPath[_startCam + 1];
		const sibr::Camera cam = sibr::Camera::interpolate(camStart, camNext, k);
		_currentCamera = sibr::InputCamera(cam, camStart.w(), camStart.h());
		_currentCamera.aspect(_viewport.finalWidth() / _viewport.finalHeight());


		_interpFactor = _interpFactor + 1;
	}

	void InteractiveCameraHandler::snapToCamera(const int i) {
		if (!_interpPath.empty()) {
			unsigned int nearestCam = (i == -1 ? findNearestCamera(_interpPath) : i);
			nearestCam = sibr::clamp(nearestCam, unsigned int(0), unsigned int(_interpPath.size() - 1));
			fromCamera(_interpPath[nearestCam], true, false);
		}
	}

	void InteractiveCameraHandler::setFPSCameraSpeed(const float speed) {
		_fpsCamera.setSpeed(speed);
	}

	void InteractiveCameraHandler::update(const sibr::Input & input, float deltaTime, const sibr::Viewport & viewport) {
		if (!viewport.isEmpty()) {
			_viewport = viewport;
		}
		if (_triggerCameraUpdate && !_viewport.isEmpty()) {
			fromCamera(_currentCamera, false, true);
			_triggerCameraUpdate = false;
		}

		if (input.key().isReleased(sibr::Key::B)) {
			switchMode(_currentMode == ORBIT ? FPS : ORBIT);
		}
		else if (input.key().isReleased(sibr::Key::V)) {
			switchMode(_currentMode == INTERPOLATION ? FPS : INTERPOLATION);
		}
		else if (input.key().isActivated(sibr::Key::LeftShift) && input.key().isReleased(sibr::Key::Y)) {
			if (_currentMode == TRACKBALL) {
				_trackball.drawThis = !_trackball.drawThis;
				SIBR_LOG << "[Trackball] Display visual guides: " << (_trackball.drawThis ? "on" : "off") << "." << std::endl;
			}
		}
		else if (input.key().isReleased(sibr::Key::Y)) {
			switchMode(_currentMode == TRACKBALL ? FPS : TRACKBALL);
		}
		else if (input.key().isReleased(sibr::Key::Space)) {
			switchSnapping();
		}
		else if (input.key().isReleased(sibr::Key::P)) {
			snapToCamera(-1);

		}
		else if (_supportRecording) {
			if (input.key().isActivated(Key::LeftShift) && (input.key().isActivated(Key::LeftAlt) || input.key().isActivated(Key::LeftControl)) && input.key().isReleased(Key::C))
			{

				_saveFrame = !_saveFrame;
				if (_saveFrame) {
					std::string pathOutView;
					for (uint i = 0; i < 10; ++i) std::cout << std::endl;
					std::cout << "Enter path to output the frames:" << std::endl;
					std::getline(std::cin, pathOutView);

					if (!pathOutView.empty()) {
						_cameraRecorder.saving(pathOutView + "/");
					}
					else {
						_cameraRecorder.stopSaving();
						_saveFrame = false;
					}
				}
				else {
					_cameraRecorder.stopSaving();
				}
			}
			else if (input.key().isActivated(Key::LeftShift) && input.key().isReleased(Key::C))
			{
				std::string filename;

				int w, h;
				for (uint i = 0; i < 10; ++i) std::cout << std::endl;
				std::cout << "Enter a filename for loading a camera path:" << std::endl;
				std::getline(std::cin, filename);
				std::cout << "Enter width for camera" << std::endl;
				std::cin >> w;
				std::cout << "Enter height for camera" << std::endl;
				std::cin >> h;
				std::cin.get();

				_cameraRecorder.reset();
				if (boost::filesystem::extension(filename) == ".out")
					_cameraRecorder.loadBundle(filename, w, h);
				else
					_cameraRecorder.load(filename);
				_cameraRecorder.playback();
			}
			else if (input.key().isActivated(Key::LeftControl) && input.key().isReleased(Key::C))
			{
				std::string filename;
				for (uint i = 0; i < 10; ++i) std::cout << std::endl;
				std::cout << "Enter a filename for saving a camera path:" << std::endl;
				std::getline(std::cin, filename);
				_cameraRecorder.save(filename);
				_cameraRecorder.saveAsBundle(filename + ".out", _currentCamera.h());
				if (_fribrExport) {
					const int height = int(std::floor(1920.0f / _currentCamera.aspect()));
					_cameraRecorder.saveAsFRIBRBundle(filename + "_fribr/", 1920, height);
				}
				_cameraRecorder.stop();
			}
			else if (input.key().isActivated(Key::LeftAlt) && input.key().isReleased(Key::C))
			{
				_cameraRecorder.reset();
				_cameraRecorder.record();
			}
			else if (input.key().isActivated(Key::RightAlt) && input.key().isReleased(Key::C)) {
				std::string filename;
				for (uint i = 0; i < 10; ++i) std::cout << std::endl;
				std::cout << "Enter a filename for saving a camera path:" << std::endl;
				std::getline(std::cin, filename);
				_cameraRecorder.playback();
				_cameraRecorder.saveAsBundle(filename + ".out", _currentCamera.h());
				if (_fribrExport) {
					const int height = int(std::floor(1920.0f / _currentCamera.aspect()));
					_cameraRecorder.saveAsFRIBRBundle(filename + "_fribr/", 1920, height);
				}
			}
			else if (input.key().isReleased(Key::C)) {
				_cameraRecorder.playback();
			}
		}

		// If the camera recorder is currently playing, don't update the various camera modes.
		if (!_cameraRecorder.isPlaying()) {

			switch (_currentMode) {
			case ORBIT:
				_orbit.update(input, _raycaster);
				_currentCamera = _orbit.getCamera();
				break;
			case INTERPOLATION:
				interpolate();
				break;
			case TRACKBALL:
				_trackball.update(input, _viewport, _raycaster);
				_currentCamera = _trackball.getCamera();
				break;
			case NONE:
				//do nothing
				break;
			case FPS:
			default:
				_fpsCamera.update(input, deltaTime);
				if (_shouldSnap) {
					_fpsCamera.snap(_interpPath);
				}
				_currentCamera = _fpsCamera.getCamera();
				break;
			}

			if (_shouldSmooth && _currentMode != INTERPOLATION) {
				const sibr::Camera newcam = sibr::Camera::interpolate(_previousCamera, _currentCamera, IBRVIEW_SMOOTHCAM_POWER);
				_currentCamera = sibr::InputCamera(newcam, _currentCamera.w(), _currentCamera.h());
			}

		}

		// Note this call has three modes: record (only read the arg camera) | playback (overwrite the arg camera) | do nothing (do nothing)
		_cameraRecorder.use(_currentCamera);

		_previousCamera = sibr::InputCamera(_currentCamera);
		_clippingPlanes[0] = _currentCamera.znear();
		_clippingPlanes[1] = _currentCamera.zfar();
	}

	const sibr::InputCamera & InteractiveCameraHandler::getCamera(void) const {
		return _currentCamera;
	}

	void InteractiveCameraHandler::onRender(const sibr::Viewport & viewport) {
		if (_currentMode == TRACKBALL) {
			_trackball.onRender(viewport);
		}
	}

	void InteractiveCameraHandler::onGUI(const std::string & suffix) {

		const std::string fullName = (suffix);
		

		// Saving camera.
		if (ImGui::Begin(fullName.c_str())) {

			ImGui::PushScaledItemWidth(130);
			ImGui::Combo("Mode", (int*)&_currentMode, "FPS\0Orbit\0Interp.\0Trackball\0None\0\0");
			switchMode(_currentMode);
			ImGui::SameLine();
			if (ImGui::Button("Load camera")) {
				std::string selectedFile;
				if (sibr::showFilePicker(selectedFile, Default)) {
					if (!selectedFile.empty()) {
						sibr::InputCamera savedCam;
						savedCam.loadFromBinary(selectedFile);
						SIBR_LOG << "Loaded saved camera (" << selectedFile << ")." << std::endl;
						fromCamera(savedCam, false);
					}
				}
			}
			
			ImGui::SameLine();
			if (ImGui::Button("Save camera (bin)")) {
				std::string selectedFile;
				if (sibr::showFilePicker(selectedFile, Save)) {
					if (!selectedFile.empty()) {
						if (selectedFile[selectedFile.size() - 1] == '/' || selectedFile[selectedFile.size() - 1] == '\\') {
							selectedFile.append("default_camera.bin");
						}
						_currentCamera.saveToBinary(selectedFile);
						SIBR_LOG << "Saved camera (" << selectedFile << ")." << std::endl;
					}
				}
			}
			
			
			ImGui::Separator();
			if (ImGui::Button("Snap to closest")) {
				_currentCamId = findNearestCamera(_interpPath);
				snapToCamera(_currentCamId);
			}
			ImGui::SameLine();
			if (ImGui::InputInt("Snap to", &_currentCamId, 1, 10)) {
				_currentCamId = sibr::clamp(_currentCamId, 0, int(_interpPath.size()) - 1);
				snapToCamera(_currentCamId);
			}
			
			if (_currentMode == TRACKBALL) {
				ImGui::SameLine();
				ImGui::Checkbox("Show trackball", &_trackball.drawThis);
			}

			if (ImGui::InputFloat("Fov Y", &_cameraFovDeg, 1.0f, 5.0f)) {
				_cameraFovDeg = sibr::clamp(_cameraFovDeg, 1.0f, 180.0f);
				_currentCamera.fovy(_cameraFovDeg * float(M_PI) / 180.0f);
				// Synchronize internal cameras.
				fromCamera(_currentCamera, _shouldSmooth);
			}
			ImGui::SameLine();
			if(ImGui::InputFloat("Near", &_clippingPlanes[0], 1.0f, 10.0f)) {
				_currentCamera.znear(_clippingPlanes[0]);
				fromCamera(_currentCamera);
			}
			ImGui::SameLine();
			if (ImGui::InputFloat("Far", &_clippingPlanes[1], 1.0f, 10.0f)) {
				_currentCamera.zfar(_clippingPlanes[1]);
				fromCamera(_currentCamera);
			}
			
			ImGui::Separator();
			ImGui::PopItemWidth();
		}
		ImGui::End();

		// Recording handling.
		if (_supportRecording) {
			std::string selectedFile;

			if (ImGui::Begin(fullName.c_str())) {
				ImGui::PushScaledItemWidth(130);

				if (ImGui::Button("Play")) {
					_cameraRecorder.playback();
				}
				ImGui::SameLine();
				if (ImGui::Button("Record")) {
					_cameraRecorder.reset();
					_cameraRecorder.record();
				}
				ImGui::SameLine();
				if (ImGui::Button("Stop")) {
					_cameraRecorder.stop();
				}
				ImGui::SameLine();
				if(ImGui::InputFloat("Speed##CamRecorder", &_cameraRecorder.speed(), 0.1f)) {
					_cameraRecorder.speed() = sibr::clamp(_cameraRecorder.speed(), 0.0f, 1.0f);
				}

				if (ImGui::Button("Load path")) {
					if (sibr::showFilePicker(selectedFile, Default)) {
						if (!selectedFile.empty()) {
							SIBR_LOG << "Loading" << std::endl;
							_cameraRecorder.reset();
							if (boost::filesystem::extension(selectedFile) == ".out")
								_cameraRecorder.loadBundle(selectedFile, 1280, 800);
							else
								_cameraRecorder.load(selectedFile);
							_cameraRecorder.playback();
						}

					}
				}
				
				ImGui::SameLine();
				if (ImGui::Button("Save path")) {
					_cameraRecorder.stop();
					if (sibr::showFilePicker(selectedFile, Save)) {
						if (!selectedFile.empty()) {
							SIBR_LOG << "Saving" << std::endl;
							_cameraRecorder.save(selectedFile + ".path");
							_cameraRecorder.saveAsBundle(selectedFile + ".out", _currentCamera.h());
							if (_fribrExport) {
								const int height = int(std::floor(1920.0f / _currentCamera.aspect()));
								_cameraRecorder.saveAsFRIBRBundle(selectedFile + "_fribr/", 1920, height);
							}
						}
					}
				}
				

				//ImGui::SameLine();
				ImGui::Checkbox("Save video (from playing)", (&_saveFrame));
				if (_saveFrame) {
					_cameraRecorder.frameDebug(_saveFrame);
				}
				
				ImGui::SameLine();
				const bool saveFrameOld = _saveFrameDebug;
				ImGui::Checkbox("Save frames (from playing)", (&_saveFrameDebug));
				if (_saveFrameDebug && !saveFrameOld) {
					if (sibr::showFilePicker(selectedFile, Save)) {
						if (!selectedFile.empty()) {
							_cameraRecorder.saving(selectedFile + "/");
							_cameraRecorder.frameDebug(_saveFrameDebug);
						}
						else {
							_cameraRecorder.stopSaving();
							_saveFrameDebug = false;
							_cameraRecorder.frameDebug(_saveFrameDebug);
						}
					}
				}
				else if (!_saveFrameDebug && saveFrameOld) {
					_cameraRecorder.stopSaving();
					_cameraRecorder.frameDebug(_saveFrameDebug);
				}
				
				//ImGui::SameLine();
				//ImGui::Checkbox("Fribr export", &_fribrExport);
				ImGui::Separator();
				ImGui::PopItemWidth();
			}
			ImGui::End();
		}
		// add the FPS camera controls in the same ImGui window.
		_fpsCamera.onGUI(suffix);
		

	}

}

