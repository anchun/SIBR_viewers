
# include "core/graphics/GUI.hpp"
# include "core/view/MultiViewManager.hpp"

namespace sibr
{
	MultiViewBase::MultiViewBase(const Vector2i & defaultViewRes)
	{
		/// \todo TODO: support launch arg for stereo mode.
		renderingMode(IRenderingMode::Ptr(new MonoRdrMode()));

		//Default view resolution.
		setDefaultViewResolution(defaultViewRes);

		_timeLastFrame = std::chrono::steady_clock::now();
		_deltaTime = 0.0;
		_exportPath = "./screenshots";
		_vdoPath = "./video.mp4";
		_savingVideo = false;
	}

	void MultiViewBase::onUpdate(Input& input)
	{
		if (input.key().isActivated(Key::LeftControl) && input.key().isPressed(Key::LeftAlt) && input.key().isPressed(Key::P)) {
			_onPause = !_onPause;
		}
		if (_onPause) {
			return;
		}

		// Elapsed time since last rendering.
		const auto timeNow = std::chrono::steady_clock::now();
		_deltaTime = (float)(std::chrono::duration_cast<std::chrono::microseconds>(timeNow - _timeLastFrame).count())/1000000.0f;
		_timeLastFrame = timeNow;

		for (auto & subview : _subViews) {
			if (subview.second.view->active()) {
				auto subInput = !subview.second.view->isFocused() ? Input() : Input::subInput(input, subview.second.viewport, false);

				if (subview.second.handler) {
					subview.second.handler->update(subInput, _deltaTime, subview.second.viewport);
				}

				subview.second.updateFunc(subview.second.view, subInput, subview.second.viewport, _deltaTime);

			}
		}

		for (auto & subview : _ibrSubViews) {
			MultiViewBase::IBRSubView & fView = subview.second;

			if (fView.view->active()) {
				auto subInput = !fView.view->isFocused() ? Input() : Input::subInput(input, fView.viewport, false);

				if (fView.handler) {
					fView.handler->update(subInput, _deltaTime, fView.viewport);
				}

				fView.cam = fView.updateFunc(fView.view, subInput, fView.viewport, _deltaTime);

				/// If we use the default update func and the integrated handler, 
				/// we have to use the handler's camera.
				if (fView.defaultUpdateFunc && fView.handler) {
					fView.cam = fView.handler->getCamera();
				}

			}
		}

		for (auto & subMultiView : _subMultiViews) {
			subMultiView.second->onUpdate(input);
		}
	}

	void MultiViewBase::onRender(Window& win)
	{
		// Render all views.
		for (auto & subview : _ibrSubViews) {
			if (subview.second.view->active()) {

				renderSubView(subview.second);

				if (_showSubViewsGui) {
					subview.second.view->onGUI();
					if (subview.second.handler) {
						subview.second.handler->onGUI("Camera " + subview.first);
					}
				}
			}
		}
		for (auto & subview : _subViews) {
			if (subview.second.view->active()) {

				renderSubView(subview.second);
				
				if (_showSubViewsGui) {
					subview.second.view->onGUI();
					if (subview.second.handler) {
						subview.second.handler->onGUI("Camera " + subview.first);
					}
				}
			}
		}
		for (auto & subMultiView : _subMultiViews) {
			subMultiView.second->onRender(win);
		}


	}

	void MultiViewBase::onGui(Window & win)
	{
	}

	void MultiViewBase::addSubView(const std::string & title, ViewBase::Ptr view, const Vector2u & res, const ImGuiWindowFlags flags)
	{
		const ViewUpdateFonc updateFunc =
			[](ViewBase::Ptr& vi, Input& in, const Viewport& vp, const float dt) {
			vi->onUpdate(in, vp);
		};
		addSubView(title, view, updateFunc, res, flags);
	}

	void MultiViewBase::addSubView(const std::string & title, ViewBase::Ptr view, const ViewUpdateFonc updateFunc, const Vector2u & res, const ImGuiWindowFlags flags)
	{
		// We have to shift vertically to avoid an overlap with the menu bar.
		const Viewport viewport(0.0f, ImGui::GetTitleBarHeight(),
			res.x() > 0 ? res.x() : (float)_defaultViewResolution.x(),
			(res.y() > 0 ? res.y() : (float)_defaultViewResolution.y()) + ImGui::GetTitleBarHeight());
		RenderTargetRGB::Ptr rtPtr(new RenderTargetRGB((uint)viewport.finalWidth(), (uint)viewport.finalHeight(), SIBR_CLAMP_UVS));
		_subViews[title] = {view, rtPtr, viewport, title, flags, updateFunc };

	}

	void MultiViewBase::addIBRSubView(const std::string & title, ViewBase::Ptr view, const IBRViewUpdateFonc updateFunc, const Vector2u & res, const ImGuiWindowFlags flags, const bool defaultFuncUsed)
	{
		// We have to shift vertically to avoid an overlap with the menu bar.
		const Viewport viewport(0.0f, ImGui::GetTitleBarHeight(),
			res.x() > 0 ? res.x() : (float)_defaultViewResolution.x(),
			(res.y() > 0 ? res.y() : (float)_defaultViewResolution.y()) + ImGui::GetTitleBarHeight());
		RenderTargetRGB::Ptr rtPtr(new RenderTargetRGB((uint)viewport.finalWidth(), (uint)viewport.finalHeight(), SIBR_CLAMP_UVS));
		if (_ibrSubViews.count(title) > 0){
			const auto handler = _ibrSubViews[title].handler;
			_ibrSubViews[title] = { view, rtPtr, viewport, title, flags, updateFunc, defaultFuncUsed };
			_ibrSubViews[title].handler = handler;
		}
		else {
			_ibrSubViews[title] = { view, rtPtr, viewport, title, flags, updateFunc, defaultFuncUsed };
		}
		

	}

	void MultiViewBase::addIBRSubView(const std::string & title, ViewBase::Ptr view, const Vector2u & res, const ImGuiWindowFlags flags)
	{
		const auto updateFunc = [](ViewBase::Ptr& vi, Input& in, const Viewport& vp, const float dt) {
			vi->onUpdate(in, vp);
			return InputCamera();
		};
		addIBRSubView(title, view, updateFunc, res, flags, true);
	}

	void MultiViewBase::addIBRSubView(const std::string & title, ViewBase::Ptr view, const IBRViewUpdateFonc updateFunc, const Vector2u & res, const ImGuiWindowFlags flags)
	{
		addIBRSubView(title, view, updateFunc, res, flags, false);
	}

	void MultiViewBase::addSubMultiView(const std::string & title, MultiViewBase::Ptr multiview)
	{
		_subMultiViews[title] = multiview;
	}

	ViewBase::Ptr & MultiViewBase::getIBRSubView(const std::string & title)
	{
		if (_subViews.count(title) > 0) {
			return _subViews.at(title).view;
		}
		else if (_ibrSubViews.count(title) > 0) {
			return _ibrSubViews.at(title).view;
		}

		SIBR_ERR << " No subview with name <" << title << "> found." << std::endl;

		return _subViews.begin()->second.view;
	}

	Viewport & MultiViewBase::getIBRSubViewport(const std::string & title)
	{
		if (_subViews.count(title) > 0) {
			return _subViews.at(title).viewport;
		}
		else if (_ibrSubViews.count(title) > 0) {
			return _ibrSubViews.at(title).viewport;
		}

		SIBR_ERR << " No subviewport with name <" << title << "> found." << std::endl;

		return _subViews.begin()->second.viewport;
	}

	void MultiViewBase::renderSubView(SubView & subview) 
	{
		bool invalidTexture = false;
		
		if (!_onPause) {

			/// \todo Offline video dumping.
			int camIdDump = 0;
			

			const Viewport renderViewport(0.0, 0.0, (float)subview.rt->w(), (float)subview.rt->h());
			subview.render(_renderingMode, renderViewport);

			// Offline video dumping, continued. We ignore additional rendering as those often are GUI overlays.
			if (subview.handler != NULL && (subview.handler->getCamera().needVideoSave() || subview.handler->getCamera().needSave())) {
				
				//std::cout << "Need video save: " << subview.handler->getCamera().needVideoSave()<< "; Need Image and Video save: " << subview.handler->getCamera().needSave() << std::endl;
				ImageRGB frame;

				subview.rt->readBack(frame);
				
				if (subview.handler->getCamera().needSave()) {
					frame.save(subview.handler->getCamera().savePath());
				}
				_videoFrames.push_back(frame.toOpenCVBGR());
				
			}

			if (_savingVideo) {

				if (_videoFrames.size() > 0) {
					std::cout << "Exporting video to : " << _vdoPath << std::endl;
					FFVideoEncoder vdoEncoder(_vdoPath, 30, Vector2i(subview.rt->w(), subview.rt->h()));
					for (int i = 0; i < _videoFrames.size(); i++) {
						vdoEncoder << _videoFrames[i];
					}
					_videoFrames.clear();
				}
				else {
					std::cout << "No frames to export!! Check save frames in camera options for the view you want to render and play the path and re-export!" << std::endl;
				}
				_savingVideo = false;
				std::cout << "Fin!" << std::endl;
			}

			// Additional rendering.
			subview.renderFunc(subview.view, renderViewport, std::static_pointer_cast<IRenderTarget>(subview.rt));

			// Render handler if needed.
			if (subview.handler) {
				subview.rt->bind();
				renderViewport.bind();
				subview.handler->onRender(renderViewport);
				subview.rt->unbind();
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		subview.view->setFocus(showImGuiWindow(subview.name, *subview.rt, subview.flags, subview.viewport, invalidTexture, subview.shouldUpdateLayout));
		ImGui::PopStyleVar();
		// If we have updated the layout, don't do it next frame.
		subview.shouldUpdateLayout = false;
	}

	ViewBase::Ptr MultiViewBase::removeSubView(const std::string & name)
	{
		ViewBase::Ptr viewPtr = nullptr;
		if (_subViews.count(name) > 0) {
			viewPtr = _subViews.at(name).view;
			_subViews.erase(name);
		}
		else if (_ibrSubViews.count(name) > 0) {
			viewPtr = _ibrSubViews.at(name).view;
			_ibrSubViews.erase(name);
		}
		else {
			SIBR_WRG << "No view named <" << name << "> found." << std::endl;
		}
		return viewPtr;
	}

	void MultiViewBase::renderingMode(const IRenderingMode::Ptr& mode)
	{
		_renderingMode = std::move(mode);
	}

	const Viewport MultiViewBase::getViewport(void) const
	{
		return Viewport(0.0f, 0.0f, (float)_defaultViewResolution.x(), (float)_defaultViewResolution.y());
	}

	void MultiViewBase::addCameraForView(const std::string & name, ICameraHandler::Ptr cameraHandler)
	{
		if (_subViews.count(name) > 0) {
			_subViews.at(name).handler = cameraHandler;
		}
		else if (_ibrSubViews.count(name) > 0) {
			_ibrSubViews.at(name).handler = cameraHandler;

			SubView & subview = _ibrSubViews.at(name);
		}
		else {
			SIBR_WRG << "No view named <" << name << "> found." << std::endl;
		}

	}

	void MultiViewBase::addAdditionalRenderingForView(const std::string & name, const AdditionalRenderFonc renderFunc)
	{
		if (_subViews.count(name) > 0) {
			_subViews.at(name).renderFunc = renderFunc;
		}
		else if (_ibrSubViews.count(name) > 0) {
			_ibrSubViews.at(name).renderFunc = renderFunc;
		}
		else {
			SIBR_WRG << "No view named <" << name << "> found." << std::endl;
		}
	}

	int MultiViewBase::numSubViews() const
	{
		return static_cast<int>(_subViews.size() + _ibrSubViews.size() + _subMultiViews.size());
	}

	void MultiViewBase::captureView(const SubView & view, const std::string& path, const std::string & filename) {

		const uint w = view.rt->w();
		const uint h = view.rt->h();

		ImageRGB renderingImg(w, h);

		view.rt->readBack(renderingImg);

		std::string finalPath = path + (!path.empty() ? "/" : "");
		if (!filename.empty()) {
			finalPath.append(filename);
		}
		else {
			auto now = std::time(nullptr);
#ifdef SIBR_OS_WINDOWS
			tm ltm = { 0,0,0,0,0,0,0,0,0 };
			localtime_s(&ltm, &now);
#else
			tm ltm = *(std::localtime(&now));
#endif
			std::stringstream buffer;
			buffer << std::put_time(&ltm, "%Y_%m_%d_%H_%M_%S");
			const std::string autoName = view.name + "_" + buffer.str();
			finalPath.append(autoName + ".png");
		}

		makeDirectory(path);
		renderingImg.save(finalPath, false);
	}

	void MultiViewBase::mosaicLayout(const Viewport & vp)
	{
		const int viewsCount = numSubViews();
		
		// Do square decomposition for now.
		// Find the next square.
		const int sideCount = int(std::ceil(std::sqrt(viewsCount)));
		const int verticalShift = int(ImGui::GetTitleBarHeight());

		Viewport usedVP = Viewport(vp.finalLeft(), vp.finalTop() + verticalShift, vp.finalRight(), vp.finalBottom());
		Vector2f itemRatio = Vector2f(1, 1) / sideCount;

		int vid = 0;
		for (auto & view : _ibrSubViews) {
			// Compute position on grid.
			const int col = vid % sideCount;
			const int row = vid / sideCount;
			view.second.viewport = Viewport(usedVP, col*itemRatio[0], row * itemRatio[1], (col + 1)*itemRatio[0], (row + 1)*itemRatio[1]);
			view.second.shouldUpdateLayout = true;
			++vid;
		}
		for (auto & view : _subViews) {
			// Compute position on grid.
			const int col = vid % sideCount;
			const int row = vid / sideCount;
			view.second.viewport = Viewport(usedVP, col*itemRatio[0], row * itemRatio[1], (col + 1)*itemRatio[0], (row + 1)*itemRatio[1]);
			view.second.shouldUpdateLayout = true;
			++vid;
		}
		for (auto & view : _subMultiViews) {
			// Compute position on grid.
			const int col = vid % sideCount;
			const int row = vid / sideCount;
			view.second->mosaicLayout(Viewport(usedVP, col*itemRatio[0], row * itemRatio[1], (col + 1)*itemRatio[0], (row + 1)*itemRatio[1]));
			++vid;
		}

	}
	void MultiViewBase::toggleSubViewsGUI()
	{
		_showSubViewsGui = !_showSubViewsGui;

		for (auto & view : _subMultiViews) {
			view.second->toggleSubViewsGUI();
		}
	}

	MultiViewManager::MultiViewManager(Window& window, bool resize)
		: _window(window), _fpsCounter(false)
	{
		if (resize) {
			window.size(
				Window::desktopSize().x() - 200,
				Window::desktopSize().y() - 200);
			window.position(100, 100);
		}

		/// \todo TODO: support launch arg for stereo mode.
		renderingMode(IRenderingMode::Ptr(new MonoRdrMode()));

		//Default view resolution.
		int w = int(window.size().x() * 0.5f);
		int h = int(window.size().y() * 0.5f);
		setDefaultViewResolution(Vector2i(w, h));

		ImGui::GetStyle().WindowBorderSize = 0.0;
	}

	void MultiViewManager::onUpdate(Input & input)
	{
		MultiViewBase::onUpdate(input);

		if (input.key().isActivated(Key::LeftControl) && input.key().isActivated(Key::LeftAlt) && input.key().isReleased(Key::G)) {
			toggleSubViewsGUI();
		}
	}

	void MultiViewManager::onRender(Window & win)
	{
		win.viewport().bind();
		glClearColor(37.f / 255.f, 37.f / 255.f, 38.f / 255.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.f, 1.f, 1.f, 1.f);

		onGui(win);

		MultiViewBase::onRender(win);

		_fpsCounter.update(_showGUI);
	}

	void MultiViewManager::onGui(Window & win)
	{
		MultiViewBase::onGui(win);

		// Menu
		if (_showGUI && ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				ImGui::MenuItem("Pause", "", &_onPause);
				if (ImGui::BeginMenu("Display")) {
					const bool currentScreenState = win.isFullscreen();
					if (ImGui::MenuItem("Fullscreen", "", currentScreenState)) {
						win.setFullscreen(!currentScreenState);
					}

					const bool currentSyncState = win.isVsynced();
					if (ImGui::MenuItem("V-sync", "", currentSyncState)) {
						win.setVsynced(!currentSyncState);
					}

					const bool isHiDPI = ImGui::GetIO().FontGlobalScale > 1.0f;
					if (ImGui::MenuItem("HiDPI", "", isHiDPI)) {
						if (isHiDPI) {
							ImGui::GetStyle().ScaleAllSizes(1.0f / win.scaling());
							ImGui::GetIO().FontGlobalScale = 1.0f;
						} else {
							ImGui::GetStyle().ScaleAllSizes(win.scaling());
							ImGui::GetIO().FontGlobalScale = win.scaling();
						}
					}

					if (ImGui::MenuItem("Hide GUI (!)", "Ctrl+Alt+G")) {
						toggleGUI();
					}
					ImGui::EndMenu();
				}


				if (ImGui::MenuItem("Mosaic layout")) {
					mosaicLayout(win.viewport());
				}

				if (ImGui::MenuItem("Row layout")) {
					Vector2f itemSize = win.size().cast<float>();
					itemSize[0] = std::round(float(itemSize[0]) / float(_subViews.size() + _ibrSubViews.size()));
					const float verticalShift = ImGui::GetTitleBarHeight();
					float vid = 0.0f;
					for (auto & view : _ibrSubViews) {
						// Compute position on grid.
						view.second.viewport = Viewport(vid*itemSize[0], verticalShift, (vid + 1.0f)*itemSize[0] - 1.0f, verticalShift + itemSize[1] - 1.0f);
						view.second.shouldUpdateLayout = true;
						++vid;
					}
					for (auto & view : _subViews) {
						// Compute position on grid.
						view.second.viewport = Viewport(vid*itemSize[0], verticalShift, (vid + 1.0f)*itemSize[0] - 1.0f, verticalShift + itemSize[1] - 1.0f);
						view.second.shouldUpdateLayout = true;
						++vid;
					}
				}


				if (ImGui::MenuItem("Quit", "Escape")) { win.close(); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Views"))
			{
				for (auto & subview : _subViews) {
					if (ImGui::MenuItem(subview.first.c_str(), "", subview.second.view->active())) {
						subview.second.view->active(!subview.second.view->active());
					}
				}
				for (auto & subview : _ibrSubViews) {
					if (ImGui::MenuItem(subview.first.c_str(), "", subview.second.view->active())) {
						subview.second.view->active(!subview.second.view->active());
					}
				}
				if (ImGui::MenuItem("Metrics", "", _fpsCounter.active())) {
					_fpsCounter.toggleVisibility();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Capture"))
			{

				if (ImGui::MenuItem("Set export directory...")) {
					std::string selectedDirectory;
					if (showFilePicker(selectedDirectory, FilePickerMode::Directory)) {
						std::cout << selectedDirectory << std::endl;
						if (!selectedDirectory.empty()) {
							_exportPath = selectedDirectory;
						}
					}
				}

				for (auto & subview : _subViews) {
					if (ImGui::MenuItem(subview.first.c_str())) {
						captureView(subview.second, _exportPath);
					}
				}
				for (auto & subview : _ibrSubViews) {
					if (ImGui::MenuItem(subview.first.c_str())) {
						captureView(subview.second, _exportPath);
					}
				}

				if (ImGui::MenuItem("Export Video")) {
					std::string saveFile;
					if (showFilePicker(saveFile, FilePickerMode::Save)) {
						std::cout << saveFile << std::endl;
						_vdoPath = saveFile + ".mp4";
						_savingVideo = true;
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void MultiViewManager::toggleGUI()
	{
		_showGUI = !_showGUI;
		toggleSubViewsGUI();
	}

} // namespace sibr
