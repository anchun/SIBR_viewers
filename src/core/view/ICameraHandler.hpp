#ifndef SIBR_CAMERA_HANDLER_H
#define SIBR_CAMERA_HANDLER_H

#include <memory>
#include <fstream>

#include "Config.hpp"

#include "core/assets/InputCamera.hpp"
#include "core/graphics/Viewport.hpp"

namespace sibr {
	class Input;

	/**
	 * Represent an interaction mode (FPS, trackball,...) for a camera controlled by the user, or a combination of multiple modes.
	 */
	class SIBR_VIEW_EXPORT ICameraHandler
	{
	public:
		SIBR_CLASS_PTR(ICameraHandler)

	public:

		/** Update the camera handler state.
		\param input user input
		\param deltaTime time elapsed since last udpate
		\param viewport view viewport
		*/
		virtual void update(const sibr::Input & input, const float deltaTime, const Viewport & viewport) = 0;

		/** \return the current camera. */
		virtual const InputCamera & getCamera(void) const = 0;

		// We allow for default empty implementations of render and onGUI.

		/** Render on top of the associated view(s). 
		\param viewport the rendering region
		*/
		virtual void onRender(const sibr::Viewport & viewport){};
		
		/** Display GUI options and infos
		\param windowName extra name to avoid collsiion between the windows of different handlers. 
		*/
		virtual void onGUI(const std::string & windowName) {};

	};
}

#endif // SIBR_CAMERA_HANDLER_H
