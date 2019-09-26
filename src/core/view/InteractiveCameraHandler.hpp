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
			FPS = 0, ORBIT = 1, INTERPOLATION = 2, TRACKBALL = 3, NONE=4
		};

		/** Constructor.
		 *\param supportRecording can this handler record camera paths.
		 *\todo Do we really need this option?
		 */
		InteractiveCameraHandler(const bool supportRecording = true);

		/** \deprecated Resolution is deprecated and will be removed in the near future.
		 *	See  setup(const std::vector<sibr::InputCamera>&, const sibr::Viewport&, std::shared_ptr<sibr::Raycaster>,...); instead. */
		void setup(const std::vector<sibr::InputCamera> & cams, const sibr::Vector2u & resolution, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster);

		/** Setup an interactive camera handler from an existing camera.
		The interactive camera will be initialized at the position of the argument camera.
		\param cam initialization camera
		\param viewport the window viewport
		\param raycaster raycaster containing the mesh displayed (used for the trackball centering), can be nullptr
		*/
		void setup(const sibr::InputCamera & cam, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster);

		/** Setup an interactive camera handler from an area of interest.
		The interactive camera will be initialized so that the area is completely visible.
		\param areaOfInterest the region of space to show
		\param viewport the window viewport
		\param raycaster raycaster containing the mesh displayed (used for the trackball centering), can be nullptr
		*/
		void setup(const Eigen::AlignedBox<float, 3>& areaOfInterest, const sibr::Viewport & viewport, const std::shared_ptr<sibr::Raycaster> raycaster);

		/** Setup an interactive camera handler from a series of existing cameras and mesh. 
		The interactive camera will be initialized at the position of the first camera from the list.
		\param cams a list of cameras (used for interpolation path)
		\param viewport the window viewport
		\param raycaster raycaster containing the mesh displayed (used for the trackball centering), can be nullptr
		\param clippingPlanes optional clipping planes to enforce
		*/
		void setup(const std::vector<sibr::InputCamera>& cams, const sibr::Viewport& viewport, std::shared_ptr<sibr::Raycaster> raycaster, const sibr::Vector2f & clippingPlanes = {-1.0f,-1.0f});

		/** Setup an interactive camera handler from a mesh.
		The interactive camera will be initialized so that the mesh is completely visible.
		\param mesh the mesh to display
		\param viewport the window viewport
		\note a raycaster will be set up internally
		*/
		void setup(std::shared_ptr<sibr::Mesh> mesh, const sibr::Viewport& viewport);

		/** Setup a camera path for the interpolation mode. */
		void setupInterpolationPath(const std::vector<sibr::InputCamera> & cameras);

		/** Move the interactive camera to a new position and change its internal parameters.
		\param cam the cameras the parameters and pose should be copied from
		\param interpolate smooth interpolation between the current pose and the new one
		\param updateResolution should the resolution of the camera be updated or not. Can be disabled if the new cam has a size incompatible with the current viewport.
		*/
		void fromCamera(const sibr::InputCamera & cam, bool interpolate = true, bool updateResolution = true);
		
		/** Move the interactive camera to a new position.
		\param cam the cameras the parameters and pose should be copied from
		\param interpolate smooth interpolation between the current pose and the new one
		\param updateResolution should the resolution of the camera be updated or not. Can be disabled if the new cam has a size incompatible with the current viewport.
		*/
		void fromTransform(const Transform3f & transform, bool interpolate = true, bool updateResolution = true);

		/** Find the camera in a list closest to the current interactive camera position
		\param inputCameras the list to search in
		\return the index of the closest camera in the list, or -1
		\note This function ignores cameras that are not 'active' in the list.
		*/
		int	findNearestCamera(const std::vector<sibr::InputCamera>& inputCameras) const;

		/** Toggle camera moves smoothing. */
		void switchSmoothing() { _shouldSmooth = !_shouldSmooth; SIBR_LOG << "Smoothing " << (_shouldSmooth ? "enabled" : "disabled") << std::endl; }

		/** Toggle automatic snapping when getting close to a camera from the interpolation path. */
		void switchSnapping() { _shouldSnap = !_shouldSnap; SIBR_LOG << "Snapping " << (_shouldSnap ? "enabled" : "disabled") << std::endl; }

		/** Switch the interaction mode (trackball, fps,...). 
			\param mode the new mode
		*/
		void switchMode(const InteractionMode mode);

		// Default camera load/save
		void saveDefaultCamera(const std::string& datasetPath);
		void loadDefaultCamera(const sibr::InputCamera&, const std::string& datasetPath);

		/** \return the current interaction mode. */
		InteractionMode getMode() const { return _currentMode; }

		/** Set the speed of the FPS camera.
		\param speed the new speed
		*/
		void setFPSCameraSpeed(const float speed);

		/// ICameraHandler interface.
		/** Update function, call at every tick.
		\param input the input object for the current view.
		\param deltaTime time elapsed since last frame
		\param viewport optional window viewport (can be used by the trackball for instance)
		*/
		virtual void update(const sibr::Input & input, float deltaTime, const sibr::Viewport & viewport = Viewport(0.0f, 0.0f, 0.0f, 0.0f)) override;

		/** \return the current camera. */
		virtual const sibr::InputCamera & getCamera(void) const override;

		/** Render additional information on screen (trackball gizmo).
		\param viewport the window viewport
		*/
		virtual void onRender(const sibr::Viewport & viewport) override;

		/** Show the GUI. 
		\param suffix additional GUI name suffix to avoid collisions when having multiple handlers. 
		*/
		virtual void onGUI(const std::string & suffix) override;

		/** \return the camera recorder */
		sibr::CameraRecorder & getCameraRecorder() { return _cameraRecorder; };
		
		/** \return the camera trackball */
		sibr::TrackBall & getTrackball() { return _trackball; }

		/** Snap the interactive camera to one of the interpolation path cameras.
		\param id the index of the camera to snap to. if -1, the closest camera. 
		*/
		void snapToCamera(int id = -1);

		/** \return the handler raycaster.
			\warn Can be nullptr 
		*/
		std::shared_ptr<Raycaster> & getRaycaster() { return _raycaster; }

		/** \return if the handler has been entirely setup */
		bool isSetup() const { return _isSetup; }

		/** \return if the handler has been entirely setup */
		const sibr::Viewport & getViewport() const { return _viewport; }


		/** \return radius used for trackball*/
		float & getRadius() { return _radius; }

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

		sibr::Vector2f _clippingPlanes;
		bool _saveFrame;
		bool _saveFrameDebug;
		bool _triggerCameraUpdate;
		bool _isSetup;
		float _cameraFovDeg;
		bool _fribrExport = false;

		void interpolate();

	};
}

#endif // SIBR_INTERACTIVE_CAMERA_HANDLER_H