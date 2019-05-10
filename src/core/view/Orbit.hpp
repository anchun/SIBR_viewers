#ifndef SIBR_ORBIT_H
#define SIBR_ORBIT_H

#include <memory>
#include <fstream>

#include "Config.hpp"
#include "core/graphics/Shader.hpp"
#include "core/assets/InputCamera.hpp"
#include "ICameraHandler.hpp"


namespace sibr {
	class Viewport;
	class Mesh;
	class Input;
	class Raycaster;

	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT Orbit : public ICameraHandler
	{
	
	public:

		/**
			Create an orbit centered at (0,0,0) with basic initial parameters. It is recommended to call fromCamera after that to setup the orbit with a valid set of parameters.
		*/
		Orbit( void );

		/**
			Setup the orbit so that its camera has the same pose as the argument camera. A raycaster is required to find the center of the orbit.
		*/
		void fromCamera(const sibr::InputCamera & cam, const std::shared_ptr<sibr::Raycaster> raycaster);

		/**
			Update the orbit camera based on the user input (keyboard). Can require a raycaster if the user is alt-clicking to select a new orbit center.
		*/
		void update( const sibr::Input & input, const std::shared_ptr<sibr::Raycaster> raycaster = std::shared_ptr<sibr::Raycaster>());

		/// ICameraHandler interface
		virtual void update(const sibr::Input & input, const float deltaTime, const Viewport & viewport) override;

		virtual const sibr::InputCamera & getCamera( void ) const override;

		/// \todo TODO SR: Temporary, should be managed at a higher level.
		void use(bool shouldUse) { _orbit.interp = shouldUse; }
		bool isUsed() { return _orbit.interp; }

	private:

		/**
		Orbit mode for all renderers

		commands :

		to enable/disable the orbit (note that using at least once ( atl + click ) to retrieve a 3D point on the proxy is mandatory before enabling the orbit) :
		b
		in static mode (default mode) :
		5 to flip the orbit (might be the first thing to do if all commands seem broken/reversed, it is needed because there is an ambiguity when using the normal of the plan containing the input cameras and the clicked point)
		4 or 6 to rotate towards current camera x-axis
		2 or 8 to rotate towards current camera y-axis
		7 or 9 to rotate towards current camera z-axis
		1 or 3 to zoom in or out
		in dynamic mode ( rotates without interruption around an axis )  :
		alt + ( 4 or 6 ) to rotate towards current camera x-axis
		alt + ( 2 or 8 ) to rotate towards current camera y-axis
		alt + ( 7 or 9 ) to rotate towards current camera z-axis
		5 to inverse the direction (same axis)
		0 to switch back to static mode with initial camera
		. to switch back to static mode with current camera

		*/

		struct OrbitParameters
		{
			enum OrbitDirection { CW = 1, ACW = -1 };	//Clockwise, AntiClockWise
			enum OrbitStatus { STATIC, FORWARD_X, FORWARD_Y, FORWARD_Z };

			OrbitParameters(void) : interp(false), factor(0), status(STATIC),
				center(sibr::Vector3f(0.0f, 0.0f, 0.0f)), radius(1.0f), theta(0), phi(0), roll(0), direction(CW), keepCamera(false)
			{}

			void flip(void) {
				yAxis = -yAxis;
				xAxis = yAxis.cross(zAxis);
			}

			bool								interp;
			bool								keepCamera;
			int									factor;

			OrbitStatus							status;
			OrbitDirection						direction;

			sibr::Vector3f						center;
			sibr::Vector3f						xAxis;
			sibr::Vector3f						yAxis;
			sibr::Vector3f						zAxis;

			float								radius;
			float								theta;
			float								phi;
			float								roll;

			sibr::Camera						initialCamera;
			sibr::Vector4f						planePointCams;

		};

		/**
		*	Compute new camera pose from current orbit parameters.
		*/
		void interpolateOrbit();

		/**
		*	Updates the orbit's center and camera pose, by casting a ray from the clicked point (in Input) to the mesh.
		*/
		void updateOrbitParameters(const sibr::Input& input, const std::shared_ptr<sibr::Raycaster> raycaster);

		/**
		*	Updates the orbit's center and camera pose, by casting a ray from the center of the screen to the mesh.
		*/
		void updateOrbitParametersCentered(const std::shared_ptr<sibr::Raycaster> raycaster);

		/** 
		*	Compute the best fitting plane of the clicked points plus the input cams positions.
		*/
		sibr::Vector4f Orbit::computeFittingPlaneCameras(sibr::Vector3f & clickedPoint, const std::vector<sibr::InputCamera> & cams);

		bool _hasBeenInitialized;
		bool _orbitPointClicked;
		sibr::InputCamera _currentCamera;
		OrbitParameters _orbit;
	
	};
}

#endif // SIBR_ORBIT_H