#ifndef SIBR_FPS_CAMERA_H
#define SIBR_FPS_CAMERA_H

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

	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT FPSCamera : public ICameraHandler
	{
	
	public:

		/**
		
		*/
		FPSCamera( void );

		/**
			Setup the FPS camera so that it has the same pose as the argument camera. 
		*/
		void fromCamera(const sibr::InputCamera & cam);

		/**
			Update the FPS camera based on the user input (keyboard). 
		*/
		void update( const sibr::Input & input, float deltaTime);

		void snap(const std::vector<sibr::InputCamera> & cams);

		/// ICameraHandler interface

		virtual void update(const sibr::Input & input, const float deltaTime, const Viewport & viewport) override;

		virtual const sibr::InputCamera & getCamera( void ) const override;

		void setSpeed(const float speed, const float angular = 0.0);

		//float & speed() { return _speedFpsCam; }
		//float & angularSpeed() { return _speedFpsCam; }

		virtual void onGUI(const std::string& suffix) override;

	private:

		float _speedFpsCam, _speedRotFpsCam;
		bool _hasBeenInitialized;
		sibr::InputCamera _currentCamera;
		bool _useAcceleration;


		void moveUsingWASD( const sibr::Input& input, float deltaTime);
		void moveUsingMousePan( const sibr::Input& input, float deltaTime);
	
	};

}
#endif // SIBR_FPS_CAMERA_H
