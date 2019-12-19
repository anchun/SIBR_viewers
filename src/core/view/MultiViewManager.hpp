#ifndef __SIBR_VIEW_MULTIVIEWMANAGER_HPP__
# define __SIBR_VIEW_MULTIVIEWMANAGER_HPP__

# include <type_traits>
# include <chrono>

# include "core/view/Config.hpp"
# include "core/graphics/Window.hpp"
# include "core/graphics/Texture.hpp"
# include "core/view/RenderingMode.hpp"
# include "core/view/FPSCamera.hpp"

# include "core/assets/InputCamera.hpp"
# include "core/graphics/Input.hpp"
# include "core/graphics/Image.hpp"
# include "core/graphics/RenderUtility.hpp"
# include "core/assets/CameraRecorder.hpp"
# include "core/view/ViewBase.hpp"
# include "core/graphics/Shader.hpp"
# include "core/view/FPSCounter.hpp"
#include "core/video/FFmpegVideoEncoder.hpp"
#include "InteractiveCameraHandler.hpp"
#include <random>
#include <map>


namespace sibr
{

	/**
	 * MultiViewManager is designed to provide 
	 * more flexibility and with a multi-windows system in mind.
	 * Once a MultiViewManager is created, you can register standard and 
	 * IBR subviews, providing additional functions for update and 
	 * rendering if needed, along with support for ImGui interfaces.
	 * To support legacy rendering modes and views, we introduce a 
	 * distinction between standard subviews, that will be rendered through 
	 * a call to onRender(Viewport&), and IBR subviews rendered through 
	 * a onRenderIBR(rt, eye) call. This also means that after updating 
	 * (via onUpdate) an IBR subview, you have to return the camera 
	 * that will be used for the onRenderIBR call.
	 * Note: new IBR views don't have to implement this distinction
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT MultiViewBase
	{
		SIBR_CLASS_PTR(MultiViewBase);

	public:

		/// Update callback for a standard view. Passes the view pointer, the correct input state, and the correct viewport.
		typedef  std::function<void(sibr::ViewBase::Ptr &, sibr::Input&, const sibr::Viewport&, const float)> ViewUpdateFonc;
		/// Update callback for an IBR view, see main description for the return value.
		typedef  std::function<sibr::InputCamera(sibr::ViewBase::Ptr &, sibr::Input&, const sibr::Viewport&, const float)> IBRViewUpdateFonc;
		/// Additional render callback for a subview.
		typedef  std::function<void(sibr::ViewBase::Ptr &, const sibr::Viewport&, const IRenderTarget::Ptr& )> AdditionalRenderFonc;

		/*
		 * \brief Creates a MultiViewManager in a given OS window.
		 * \param window The OS window to use.
		 * \param resize Should the window be resized by the manager to maximize usable space.
		 */
		MultiViewBase(const Vector2i & defaultViewRes = { 800, 600 });

		/**
		 * \brief Update subviews and the MultiViewManager.
		 * \param input The Input state to use.
		 */
		virtual void	onUpdate(Input & input);

		/**
		 * \brief Render the content of the MultiViewManager
		 * \param win The OS window into which the rendering should be performed.
		 */
		virtual void	onRender(Window& win);

		/**
		 * \brief Render additional gui
		 * \param win The OS window into which the rendering should be performed.
		 */
		virtual void	onGui(Window& win);

		/**
		 * \brief Register a standard subview (for instance a SceneDebugView). It will be rendered via a call to onRender(Viewport).
		 * \param title the title of the view.
		 * \param view a pointer to the view.		
		 * \param res a custom resolution used for the internal rendering and display. If null, the default value is used.
		 * \param flags ImGui_WindowFlags to pass to the internal window manager.
		 */
		void	addSubView(const std::string& title, ViewBase::Ptr view,
						const Vector2u & res = Vector2u(0,0),
						const ImGuiWindowFlags flags = 0);

		/**
		* \brief Register a standard subview (for instance a SceneDebugView). It will be rendered via a call to onRender(Viewport) in an implicit rendertarget managed by the MVM.
		* \param title the title of the view.
		* \param view a pointer to the view.
		* \param updateFunc the function that will be called to update your view.
		*					It will pass you the view, the correct Input (mouse position
		*					from 0,0 in the top left corner, key presses and mouse clicks
		*					only if the cursor is over the view), and the Viewport in the
		*					OS window.
		* \param res a custom resolution used for the internal rendering and display. If null, the default value is used.
		* \param flags ImGui_WindowFlags to pass to the internal window manager.
		*/
		void	addSubView(const std::string& title, ViewBase::Ptr view,
			const ViewUpdateFonc updateFunc,
			const Vector2u & res = Vector2u(0, 0),
			const ImGuiWindowFlags flags = 0);

		/**
		* \brief Register an IBR subview (for instance an ULRView). It will be rendered via a call to onRenderIBR(rt,cam,dst).
		* \param title the title of the view.
		* \param view a pointer to the view.
		* \param res a custom resolution used for the internal rendering. If null, the default value is used.
		* \param flags ImGui_WindowFlags to pass to the internal window manager.
		*/
		void	addIBRSubView(const std::string& title, ViewBase::Ptr view, 
						const Vector2u & res = Vector2u(0, 0),
						const ImGuiWindowFlags flags = 0);

		/**
		* \brief Register an IBR subview (for instance an ULRView). It will be rendered via a call to onRenderIBR(rt,cam,dst).
		* \param title the title of the view.
		* \param view a pointer to the view.
		* \param updateFunc the function that will be called to update your view.
		*					It will pass you the view, the correct Input (mouse position
		*					from 0,0 in the top left corner, key presses and mouse clicks
		*					only if the cursor is over the view), and the Viewport in the
		*					OS window. You should return the camera to use during rendering.
		* \param res a custom resolution used for the internal rendering. If null, the default value is used.
		* \param flags ImGui_WindowFlags to pass to the internal window manager.
		*/
		void	addIBRSubView(const std::string& title, ViewBase::Ptr view,
			const IBRViewUpdateFonc updateFunc,
			const Vector2u & res = Vector2u(0, 0),
			const ImGuiWindowFlags flags = 0);

		/** Add another multi-view manager as a subsystem of this one.
		 * \param title a name for the manager
		 * \param multiview the manager to add as a subview
		 */
		void	addSubMultiView(const std::string & title, MultiViewBase::Ptr multiview);

		/**
		* \param title
		* \return Return viewbase associated with title, will EXIT_ERROR if no view found
		*/
		ViewBase::Ptr &	getIBRSubView(const std::string& title);

		/**
		* \param title
		* \return the Viewport associated with title, will EXIT_ERROR if no viewport found
		*/
		Viewport & getIBRSubViewport(const std::string &title);

		/**
		* \brief Unregister a subview.
		* \param title the title of the view to remove.
		* \return the view removed from the MultiViewManager.
		*/
		ViewBase::Ptr	removeSubView(const std::string& title);
	
		/**
		 * \brief Change the rendering mode.
		 * \param mode The rendering mode to use (will be moved).
		 */
		void	renderingMode(const IRenderingMode::Ptr& mode);


		/**
		 * \brief Define the default rendering and display size for new subviews.
		 * \param size the default size to use.
		 */
		void				setDefaultViewResolution(const Vector2i& size);

		/**
		 * \brief Returns the default viewport used for subviews rendering.
		 * \return the current default subview viewport
		 */
		const Viewport		getViewport(void) const;

		/**
		 * \brief Returns the last frame time.
		 * \return the last frame time.
		 */
		const float & deltaTime() const { return _deltaTime; }

		/**
		 * \brief Add a camera handler that will automatically be updated and used by the MultiViewManager for the given subview.
		 * \param name the name of the subview to which the camera should be associated.
		 * \param cameraHandler a pointer to the camera handler to register.
		 */
		void addCameraForView(const std::string & name, ICameraHandler::Ptr cameraHandler);

		/**
		* \brief Register a function performing additional rendering for a given subview, 
		* called by the MultiViewManager after calling onRender() on the subview.
		* \param name the name of the subview to which the function should be associated.
		* \param renderFunc the function performing additional rendering..
		*/
		void addAdditionalRenderingForView(const std::string & name, const AdditionalRenderFonc renderFunc);

		/**
		* \brief Count NOT recursively the number of subviews.
		*/
		int numSubViews() const;

		/** Place all subviews on a regular grid in the given viewport.
		 * \param vp the region in which the views should be layed out.
		 */
		void mosaicLayout(const Viewport & vp);

		/** Toggle the display of sub-managers GUIs. */
		void toggleSubViewsGUI();

		/**
		* \brief Set the export path.
		* \param path path to the directory to use.
		*/
		void setExportPath(const std::string & path);

	protected:

		struct SubView {
			ViewBase::Ptr view;
			RenderTargetRGB::Ptr rt;
			ICameraHandler::Ptr handler;
			AdditionalRenderFonc renderFunc;
			sibr::Viewport viewport;
			std::string name;
			ImGuiWindowFlags flags;
			bool shouldUpdateLayout;

			SubView() {};

			SubView(ViewBase::Ptr view_, RenderTargetRGB::Ptr rt_, const sibr::Viewport viewport_, const std::string & name_, const ImGuiWindowFlags flags_) :
				view(view_), rt(rt_), handler(), viewport(viewport_), name(name_), flags(flags_), shouldUpdateLayout(false) {
				renderFunc = [](ViewBase::Ptr &, const Viewport&, const IRenderTarget::Ptr & ) {};
			}

			virtual void render(const IRenderingMode::Ptr & rm, const Viewport & renderViewport) const = 0;
		};

		struct BasicSubView : SubView {
			ViewUpdateFonc updateFunc;

			BasicSubView() : SubView() {};

			BasicSubView(ViewBase::Ptr view_, RenderTargetRGB::Ptr rt_, const sibr::Viewport viewport_, const std::string & name_, const ImGuiWindowFlags flags_, ViewUpdateFonc f_) :
				SubView(view_, rt_, viewport_, name_, flags_), updateFunc(f_) {
			}

			virtual void render(const IRenderingMode::Ptr & rm, const Viewport & renderViewport) const override {
				rt->bind();
				renderViewport.bind();
				renderViewport.clear();
				view->onRender(renderViewport);
				rt->unbind();
			}
		};

	
		struct IBRSubView : SubView {
			IBRViewUpdateFonc updateFunc;
			sibr::InputCamera cam;
			bool defaultUpdateFunc;

			IBRSubView() : SubView() {};

			IBRSubView(ViewBase::Ptr view_, RenderTargetRGB::Ptr rt_, const sibr::Viewport viewport_, const std::string & name_, const ImGuiWindowFlags flags_, IBRViewUpdateFonc f_, const bool defaultUpdateFunc_) :
				SubView(view_, rt_, viewport_, name_, flags_), updateFunc(f_), defaultUpdateFunc(defaultUpdateFunc_){
				cam = sibr::InputCamera();
			}

			virtual void render(const IRenderingMode::Ptr & rm, const Viewport & renderViewport) const override {
				if (rm) {
					rm->render(*view, cam, renderViewport, rt.get());
				}
			}
		};

	protected:
		void				addIBRSubView(const std::string & title, ViewBase::Ptr view, 
											const IBRViewUpdateFonc updateFunc, const Vector2u & res, 
											const ImGuiWindowFlags flags, const bool defaultFuncUsed);

		void				renderSubView(SubView & subview);

		static void				captureView(const SubView & view, const std::string & path = "./screenshots", const std::string & filename = "");
		
		IRenderingMode::Ptr _renderingMode = nullptr;
		std::map<std::string, BasicSubView> _subViews;
		std::map<std::string, IBRSubView> _ibrSubViews;
		std::map<std::string, std::shared_ptr<MultiViewBase> > _subMultiViews;


		Vector2i _defaultViewResolution;
		std::string _exportPath;
		std::string _vdoPath;
		bool _savingVideo;
		std::vector<cv::Mat> _videoFrames;
		std::chrono::time_point<std::chrono::steady_clock> _timeLastFrame;
		float _deltaTime;
		bool _showSubViewsGui = true;
		bool _onPause = false;
	};


	class SIBR_VIEW_EXPORT MultiViewManager : public MultiViewBase
	{
	public:
		/*
		 * \brief Creates a MultiViewManager in a given OS window.
		 * \param window The OS window to use.
		 * \param resize Should the window be resized by the manager to maximize usable space.
		 */
		MultiViewManager(Window& window, bool resize = true);

		/**
		 * \brief Update subviews and the MultiViewManager.
		 * \param input The Input state to use.
		 */
		void	onUpdate(Input & input) override;

		/**
		 * \brief Render the content of the MultiViewManager and its interface
		 * \param win The OS window into which the rendering should be performed.
		 */
		void	onRender(Window& win) override;

		/**
		 * \brief Render menus and additional gui
		 * \param win The OS window into which the rendering should be performed.
		 */
		void	onGui(Window& win) override;

	private:
		void toggleGUI();

		Window& _window;
		FPSCounter _fpsCounter;
		bool _showGUI = true;

	};

	///// INLINE /////

	inline void					MultiViewBase::setDefaultViewResolution(const Vector2i& size) {
		_defaultViewResolution = size;
	}

	

} // namespace sibr

#endif // __SIBR_VIEW_MULTIVIEWMANAGER_HPP__
