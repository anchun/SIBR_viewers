
#ifndef __SIBR_GRAPHICS_WINDOW_HPP__
# define __SIBR_GRAPHICS_WINDOW_HPP__

#include "core/graphics/imgui/imgui.h"
#include "core/graphics/imgui/imgui_impl_glfw_gl3.h"


# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Viewport.hpp"
# include "core/graphics/Texture.hpp"
#include <core/system/CommandLineArgs.hpp>

namespace sibr
{

	/** System window backed by an internal framebuffer.
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT Window : public IRenderTarget
	{
	public:
		typedef std::shared_ptr<Window>		Ptr;

	public:
		/** Constructor.
		 *\param title window title
		 *\param args window setup arguments
		 *\sa WindowArgs
		 **/
		Window(const std::string& title, const WindowArgs & args = {});

		/** Constructor, overriding the window size.
		 *\param w window width
		 *\param h window height
		 *\param title window title
		 *\param args window setup arguments
		 *\sa WindowArgs
		 **/
		Window(uint w, uint h, const std::string& title, const WindowArgs & args = {});

		/** Constructor, adjust the size to fill the screen except for the margins.
		 *\param title window title
		 *\param margins horizontal and vertical margins to preserve on each side of the window
		 *\param args window setup arguments
		 *\sa WindowArgs
		 **/
		Window(const std::string & title, const sibr::Vector2i & margins, const WindowArgs & args = {});

		/** \return a pointer to the underlying GLFW window */
		GLFWwindow *  GLFW(void);

		/** Activate the associated graphics context. */
		inline void			makeContextCurrent(void);
		/** \return the context currently in use (represented by a GLFW window) */
		inline GLFWwindow *		getContextCurrent(void);
		/** Deactivate the associated graphics context. */
		inline void			makeContextNull(void);

		/** Flush the graphics pipeline and perform rendering, displaying the result in the abck buffer. */
		inline void			swapBuffer(void);

		/** Set the window size
		 *\param w width
		 *\param h height
		 **/
		void				size(int w, int h);
		
		/** \return the window size */
		Vector2i			size(void) const;

		/** Set the window position
		 *\param x horizontal location
		 *\param y vertical location
		 **/
		void				position(const unsigned int x, const unsigned int y);

		/** \return the window position on screen */
		Vector2i			position() const;

		/** \return the screen size. */
		static Vector2i		desktopSize(void);

		/** \return true if an openGL context is active. */
		static bool			contextIsRunning(void);

		/** Set the framerate.
		 *\param fps one of 60, 30, 15 
		 */
		void				setFrameRate(int fps);

		/** Display the cursor in the window. 
		 * \param enable boolean flag
		 */
		void				enableCursor(bool enable);

		/** \return if the window is currently opened */
		bool				isOpened(void) const;
		/** Mark the window as closed. */
		void				close(void);

		/** \return true if the window is fullscreen. */
		bool				isFullscreen(void) const;
		/** Toggle fullscreen.
		 *\param fullscreen if true the window will be resized to occupy the whole screen, without visible borders.
		 */
		void				setFullscreen(const bool fullscreen);

		/** \return true if the window is using V-sync. */
		bool				isVsynced(void) const;
		/** Toggle V-sync.
		 *\param vsync if true, framerate will be limited to 60 FPS
		 *\note When set to false, tearing might be visible.
		 */
		void				setVsynced(const bool vsync);

		/** \return the window viewport */
		const Viewport&	viewport(void) const;

		/** Set the window viewport.
		 *\param view the new viewport
		 */
		void			viewport(const Viewport& view);

		// From IRenderTarget
		/** Get the backbuffer texture ID. unsuported. */
		inline GLuint	texture(uint t = 0) const;

		/** Get the backbuffer texture ID. unsuported. */
		inline GLuint	handle(uint t = 0) const;

		/** \return the window buffer ID (0) */
		inline GLuint	fbo(void) const;

		/** Bind the window buffer. */
		inline void		bind(void);

		/** Unind the window buffer. */
		inline void		unbind(void);

		/** Clear the window buffer. */
		inline void		clear(void);

		/** \return the window buffer width */
		inline uint		w(void) const;

		/** \return the window buffer height */
		inline uint		h(void) const;

		/** \return the screens caling factor. */
		inline float	scaling() const;
		
		static int			contextId; ///< Last created window context ID (-1 initially).

	private:

		/** Setup the window.
		 *\param width window width on screen
		 *\param height window height on screen
		 *\param title window title
		 *\param args window setup arguments
		 *\sa WindowArgs
		 */
		void setup(int width, int height, const std::string & title, const WindowArgs & args);

		/// Window pointer for callbacks.
		typedef std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> GLFWwindowptr;

		/// Helper to handle window creation/destruction.
		struct AutoInitializer
		{
			AutoInitializer(void);
			~AutoInitializer(void);
		};

		bool				_shouldClose; ///< Is the window marked as closed.
		GLFWwindowptr		_glfwWin; ///< Undelrying GLF window.
		Vector2i			_size; ///< Window size.
		const bool			_useGUI; ///< Should ImGui windows be displayed.
		bool				_useVSync; ///< is the window using vsync.
		Vector2i			_oldPosition; ///< Backup for handling fullscreen/windowed mode restoration.
		Vector2i			_oldSize; ///< Backup for handling fullscreen/windowed mode restoration.
		Viewport			_viewport; ///< Current viewport.
		float				_scaling = 1.0f; ///< Internal scaling for HiDPI screens.
		// Must be placed add the end of member data .
		AutoInitializer		_hiddenInit; ///< nifty counter used to auto-init window system
	};

	///// INLINES /////
	void		Window::makeContextCurrent(void) {
		glfwMakeContextCurrent(_glfwWin.get());
	}

	void		Window::makeContextNull(void) {
		glfwMakeContextCurrent(0);
	}

	GLFWwindow *		Window::getContextCurrent(void) {
		return glfwGetCurrentContext();
	}

	void			Window::swapBuffer(void) {
		if (_useGUI) {
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "ImGui interface");
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			glPopDebugGroup();
		}
		glfwSwapBuffers(_glfwWin.get());
		// Keep the call below in all cases to avoid accumulating all interfaces in one frame.
		ImGui_ImplGlfwGL3_NewFrame();

	}

	inline GLuint	Window::texture(uint /*t*/) const {
		SIBR_ERR << "You are trying to read the Window's backbuffer (use sibr::blit instead)." << std::endl;
		return 0;
	}
	inline GLuint	Window::handle(uint /*t*/) const {
		SIBR_ERR << "You are trying to read the Window's backbuffer (use sibr::blit instead)." << std::endl;
		return 0;
	}
	inline GLuint	Window::fbo(void) const {
		return 0;
	}

	inline void		Window::bind(void) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
	}

	inline void		Window::unbind(void) {
		/*nothing*/
	}

	inline void		Window::clear(void) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	inline uint		Window::w(void) const {
		return (uint)size().x();
	}

	inline uint		Window::h(void) const {
		return (uint)size().y();
	}

	inline float Window::scaling() const
	{
		return _scaling;
	}

	inline const Viewport&	Window::viewport(void) const {
		return _viewport;
	}

	inline void				Window::viewport(const Viewport& view) {
		_viewport = view;
	}


} // namespace sibr

#endif // __SIBR_GRAPHICS_WINDOW_HPP__
