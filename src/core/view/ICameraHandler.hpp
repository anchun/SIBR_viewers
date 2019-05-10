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
	 * Represent an interaction mode (FPS, trackball,...) or a combination of these.
	 */
	class SIBR_VIEW_EXPORT ICameraHandler
	{
	public:
		SIBR_CLASS_PTR(ICameraHandler)

	public:

		// Mandatory methods.
		virtual void update(const sibr::Input & input, const float deltaTime, const Viewport & viewport) = 0;
		virtual const sibr::InputCamera & getCamera(void) const = 0;

		// We allow for default empty implementations of render and onGUI.
		virtual void onRender(const sibr::Viewport & viewport){};
		virtual void onGUI(const std::string & windowName) {};

	};
}

#endif // SIBR_CAMERA_HANDLER_H
