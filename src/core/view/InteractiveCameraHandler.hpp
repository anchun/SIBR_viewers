#ifndef SIBR_INTERACTIVE_CAMERA_HANDLER_H
#define SIBR_INTERACTIVE_CAMERA_HANDLER_H

#include <memory>
#include <fstream>

#include "Config.hpp"
#include "core/graphics/Shader.hpp"
#include "core/assets/InputCamera.hpp"

#include "core/view/FPSCamera.hpp"
#include "core/view/Orbit.hpp"
#include "core/view/TrackBall.h"
#include "core/assets/CameraRecorder.hpp"
#include "core/graphics/Viewport.hpp"
#include "core/graphics/Mesh.hpp"
#include "ICameraHandler.hpp"

namespace sibr {
	class Mesh;
	class Input;
	class Raycaster;

	/**
		The InteractiveCameraHandler gathers various types of camera interactions and
		allows the user to switch between them, keeping them in sync.
		It can also perform camera interpolation along a path.
		\ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT InteractiveCameraHandler : public ICameraHandler
	{

	public:

		SIBR_CLASS_PTR(InteractiveCameraHandler);

		enum InteractionMode {
			FPS = 0, ORBIT = 1, INTERPOLATION = 2, TRACKBALL = 3
		};

		InteractiveCameraHandler(const bool supportRecording = true);

		/// Resolution is deprecated and will be removed in the near future.
		void setup(const std::vector<sibr::InputCamera> & cams, const sibr::Vector2u & resolution, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster);

		void setup(const sibr::InputCamera & cam, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster);

		void setup(const Eigen::AlignedBox<float, 3>& areaOfInterest, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster);

		void setup(const std::vector<sibr::InputCamera>& cams, const sibr::Viewport& viewport, std::shared_ptr<sibr::Raycaster>);

		void setup(std::shared_ptr<sibr::Mesh>, const sibr::Viewport& viewport);

		void setupInterpolationPath(const std::vector<sibr::InputCamera> & cameras);

		void fromCamera(const sibr::InputCamera & cam, bool interpolate = true, bool updateResolution = true);

		void updateView(const sibr::InputCamera & cam);

		int	findNearestCamera(const std::vector<sibr::InputCamera>& inputCameras) const;

		void switchSmoothing() { _shouldSmooth = !_shouldSmooth; std::cerr << "Smoothing " << (_shouldSmooth ? "enabled" : "disabled") << std::endl; }

		void switchSnapping() { _shouldSnap = !_shouldSnap; std::cerr << "Snapping " << (_shouldSnap ? "enabled" : "disabled") << std::endl; }

		void switchMode(const InteractionMode mode);

		// Default camera load/save
		void saveDefaultCamera(const std::string& datasetPath);
		void loadDefaultCamera(const sibr::InputCamera&, const std::string& datasetPath);

		InteractionMode getMode() const { return _currentMode; }

		void setFPSCameraSpeed(const float speed);

		/// ICameraHandler interface.

		virtual void update(const sibr::Input & input, float deltaTime, const sibr::Viewport & viewport = Viewport(0.0f, 0.0f, 0.0f, 0.0f)) override;

		virtual const sibr::InputCamera & getCamera(void) const override;

		virtual void onRender(const sibr::Viewport & viewport) override;

		virtual void onGUI(const std::string & suffix) override;

		FPSCamera & fpsCamera() { return _fpsCamera; };

		sibr::CameraRecorder & cameraRecorder() { return _cameraRecorder; };

		sibr::TrackBall & getTrackball() { return _trackball; }

		void snapToCamera(int id = -1);

		std::shared_ptr<Raycaster> & getRaycaster() { return _raycaster; }

		bool isSetup() const { return _isSetup; }

		const sibr::Viewport & viewport() const { return _viewport; }

	private:

		int _currentCamId;

		bool _shouldSmooth;
		bool _shouldSnap;

		sibr::FPSCamera _fpsCamera;
		sibr::Orbit _orbit;
		sibr::TrackBall _trackball;

		InteractionMode _currentMode;

		float _radius;

		std::shared_ptr<sibr::Raycaster> _raycaster;
		sibr::Viewport _viewport;

		sibr::InputCamera _previousCamera;
		sibr::InputCamera _currentCamera;

		/// Parameters for path interpolation.
		uint _startCam;
		uint _interpFactor;
		std::vector<sibr::InputCamera> _interpPath;

		sibr::CameraRecorder _cameraRecorder;
		bool _supportRecording;

		bool _saveFrame;
		bool _triggerCameraUpdate;
		bool _isSetup;
		float _cameraFovDeg;
		bool _fribrExport = false;

		void interpolate();

	};
}

#endif // SIBR_INTERACTIVE_CAMERA_HANDLER_H