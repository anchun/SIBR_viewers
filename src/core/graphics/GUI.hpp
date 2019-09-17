
#ifndef __SIBR_GRAPHICS_GUI_HPP__
# define __SIBR_GRAPHICS_GUI_HPP__

# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Image.hpp"
# include "core/graphics/RenderTarget.hpp"
# include "core/graphics/Window.hpp"


// Add your own utility functions here.

namespace sibr
{
	/**	Note that the windowTitle name must be different from all other window
	*
	*   Returns TRUE if window is focused (useful for managing interactions).
	\ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT bool		showImGuiWindow(const std::string& windowTitle, const IRenderTarget& rt, ImGuiWindowFlags flags, Viewport & viewport, const bool invalidTexture, const bool updateLayout);

	/**
	\ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT void 		fitImageToDisplayRegion(const Vector2f & imgSize, const Vector2i & regionSize, Vector2f& offset, Vector2f& size);
	
	SIBR_GRAPHICS_EXPORT Mesh::Ptr generateMeshForText(const std::string & text, unsigned int & separationIndex);

} // namespace sibr

namespace ImGui {

	/**
	\ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT const float GetTitleBarHeight();

	/**
	\ingroup sibr_graphics
	*/
	SIBR_GRAPHICS_EXPORT void DrawTextWindow(const char * title, const float x, const float y, const char * text);

	/**
	* \ingroup sibr_graphics
	* \brief Push a width for item which is HiDPI aware.
	* \param item_width The with to push, in regular pixels.
	*/
	SIBR_GRAPHICS_EXPORT void PushScaledItemWidth(float item_width);

	SIBR_GRAPHICS_EXPORT bool TabButton(const char* label, bool highlight, const ImVec2& size = ImVec2(0, 0));
}

namespace sibr {

	struct SIBR_GRAPHICS_EXPORT CallBackData {
		sibr::Vector2f positionRatio;
		sibr::Vector2f itemPos, itemSize, mousePos;
		float scroll = 0.0f;
		bool isHoovered = false, isClickedRight = false, isClickedLeft = false,
			isReleasedRight = false, isReleasedLeft = false, ctrlPressed = false;
	};

	struct SIBR_GRAPHICS_EXPORT ZoomData {
		sibr::Vector2f topLeft()		const;
		sibr::Vector2f bottomRight()	const;
		sibr::Vector2f uvFromBoxPos(const sibr::Vector2f& pos)	const;
		ZoomData scaled(const sibr::Vector2f& size) const;

		sibr::Vector2f center = sibr::Vector2f(0.5f, 0.5f), diagonal = sibr::Vector2f(0.5f, 0.5f),
			tmpTopLeft, tmpBottonRight, firstClickPixel, secondClickPixel;
		bool underMofidication = false;
	};

	struct SIBR_GRAPHICS_EXPORT ZoomInterraction {
		void updateZoom(const sibr::Vector2f& canvasSize );
		
		CallBackData callBackData;
		ZoomData zoomData;
	};

	struct SIBR_GRAPHICS_EXPORT SegmentSelection {
		void update(const CallBackData & callback, const sibr::Vector2i & size, const ZoomData & zoom = {} );
		void computeRasterizedLine();

		sibr::Vector2i firstPosScreen, secondPosScreen, firstPosIm, secondPosIm;
		std::vector<sibr::Vector2i> rasterizedLine;
		bool first = false, valid = false;
	};

	SIBR_GRAPHICS_EXPORT void DisplayImageGui(
		GLuint texture,
		const sibr::Vector2i & displaySize,
		const sibr::Vector2f& uv0 = { 0, 0 },
		const sibr::Vector2f& uv1 = { 1, 1 }
	);

	SIBR_GRAPHICS_EXPORT void ImageWithCallback(
		GLuint texture,
		const sibr::Vector2i & displaySize,
		CallBackData & callbackDataOut,
		const sibr::Vector2f& uv0 = { 0, 0 },
		const sibr::Vector2f& uv1 = { 1, 1 }
	);

	SIBR_GRAPHICS_EXPORT void ImageWithZoom(
		GLuint texture,
		const sibr::Vector2i & displaySize,
		ZoomInterraction & zoom
	);


}

template<typename T> sibr::Vector<T, 2> toSIBR(const ImVec2 & v) {
	return sibr::Vector<T, 2>(v.x, v.y);
}

#endif // __SIBR_GRAPHICS_GUI_HPP__
