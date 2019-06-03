
#ifndef __SIBR_GRAPHICS_WINDOW_HPP__
# define __SIBR_GRAPHICS_WINDOW_HPP__



#include "core/graphics/imgui/imgui.h"
#include "core/graphics/imgui/imgui_impl_glfw_gl3.h"


# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Viewport.hpp"
# include "core/graphics/Texture.hpp"

namespace sibr
{
	/**
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT Window : public IRenderTarget
	{
	public:
		typedef std::shared_ptr<Window>		Ptr;

	public:
		Window(int width, int height, const std::string& title, bool fullScreen = false, bool doVSync = true, bool useGUI = true);

		Window(const std::string & title, const sibr::Vector2i & margins, bool fullScreen = false, bool doVSync = true, bool useGUI = true);


		GLFWwindow *  GLFW(void);
		inline void			makeContextCurrent(void);
		inline GLFWwindow *		getContextCurrent(void);
		inline void			makeContextNull(void);

		/// Cause to refresh what is displayed on the screen
		inline void			swapBuffer(void);

		void				size(int w, int h);
		Vector2i			size(void) const;
		void				position(const unsigned int x, const unsigned int y);
		Vector2i			position() const;

		/// Be sure to have an OpenGL (or GLFW) before calling this function
		static Vector2i		desktopSize(void);

		/// Returns true if at least one OpenGL context is running.
		static bool			contextIsRunning(void);

		/// id of the last created context (-1 if no context)
		static int			contextId;

		void				setFrameRate(int fps);


		void				enableCursor(bool enable);

		bool				isOpened(void) const;
		void				close(void);

		bool				isFullscreen(void) const;
		void				setFullscreen(const bool fullscreen);

		bool				isVsynced(void) const;
		void				setVsynced(const bool vsync);

		const Viewport&	viewport(void) const;
		void			viewport(const Viewport& view);

		// From IRenderTarget
		inline GLuint	texture(uint t = 0) const;
		inline GLuint	fbo(void) const;
		inline void		bind(void);
		inline void		unbind(void);
		inline void		clear(void);
		inline uint		w(void) const;
		inline uint		h(void) const;

		inline float	scaling() const;

	private:

		void setup(int width, int height, const std::string & title, bool fullScreen, bool doVSync, bool useGUI);

		typedef std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> GLFWwindowptr;

		struct AutoInitializer
		{
			AutoInitializer(void);
			~AutoInitializer(void);
		};

		bool				_shouldClose;
		GLFWwindowptr		_glfwWin;
		Vector2i			_size;
		const bool			_useGUI;
		bool				_useVSync;
		Vector2i			_oldPosition; ///< Backup for handling fullscreen/windowed mode restoration.
		Vector2i			_oldSize; ///< Backup for handling fullscreen/windowed mode restoration.
		Viewport			_viewport;
		float				_scaling = 1.0f;
		// Must be place add the end of member data
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

	inline GLuint	Window::texture(uint /*t (not used*/) const {
		SIBR_ERR << "Your try to bind the Window's backbuffer (You are supposed to use glBlitFramebuffer())." << std::endl;
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

	struct SIBR_GRAPHICS_EXPORT WindowTest {

		GLFWwindow *		_glfwWin;

		WindowTest(int width, int height, const std::string& title, bool fullScreen, bool doVSync, bool useGUI);

		void setup(int width, int height, const std::string& title, bool fullScreen, bool doVSync, bool useGUI);

		GLFWwindow * GLFW(void);


	};

} // namespace sibr

#endif // __SIBR_GRAPHICS_WINDOW_HPP__
