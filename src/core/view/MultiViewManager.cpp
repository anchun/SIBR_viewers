
# include "core/graphics/GUI.hpp"
# include "core/view/MultiViewManager.hpp"

namespace sibr
{
	MultiViewManager::MultiViewManager(Window& window, bool resize) : _window(window), _fpsCounter(false)
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
		_timeLastFrame = std::chrono::steady_clock::now();
		_deltaTime = 0.0;
		_exportPath = "./screenshots";
	}

	void MultiViewManager::onUpdate(Input& input)
	{
		if (input.key().isActivated(sibr::Key::LeftControl) && input.key().isPressed(sibr::Key::LeftAlt) && input.key().isPressed(sibr::Key::P)) {
			_onPause = !_onPause;
		}
		if (_onPause) {
			return;
		}
		if (input.key().isActivated(sibr::Key::LeftControl) && input.key().isActivated(sibr::Key::LeftAlt) && input.key().isReleased(sibr::Key::G)) {
			_showGUI = !_showGUI;
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
			sibr::MultiViewManager::IBRSubView & fView = subview.second;

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

	}

	void MultiViewManager::onRender(Window& win)
	{

		win.viewport().bind();
		glClearColor(37.f / 255.f, 37.f / 255.f, 38.f / 255.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.f, 1.f, 1.f, 1.f);

		// Menu
		if (_showGUI) {
			if (ImGui::BeginMainMenuBar())
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
							if(isHiDPI) {
								ImGui::GetStyle().ScaleAllSizes(1.0f/win.scaling());
								ImGui::GetIO().FontGlobalScale = 1.0f;
							} else {
								ImGui::GetStyle().ScaleAllSizes(win.scaling());
								ImGui::GetIO().FontGlobalScale = win.scaling();
							}	
						}
						
						if (ImGui::MenuItem("Hide GUI (!)", "Ctrl+Alt+G")) {
							_showGUI = !_showGUI;
						}
						ImGui::EndMenu();
					}


					if (ImGui::MenuItem("Mosaic layout")) {
						const int viewsCount = _subViews.size() + _ibrSubViews.size();
						// Do square decomposition for now.
						// Find the next square.
						const int sideCount = int(ceil(sqrt(viewsCount)));
						const int verticalShift = ImGui::GetTitleBarHeight();
						const sibr::Vector2i itemSize = ((_window.size() - sibr::Vector2i(0, verticalShift)).cast<float>() / sideCount).cast<int>();
						int vid = 0;
						for (auto & view : _ibrSubViews) {
							// Compute position on grid.
							const int col = vid % sideCount;
							const int row = vid / sideCount;
							view.second.viewport = sibr::Viewport(col*itemSize[0], verticalShift  + row*itemSize[1], (col + 1)*itemSize[0] - 1, verticalShift + (row + 1)*itemSize[1] - 1);
							view.second.shouldUpdateLayout = true;
							++vid;
						}
						for (auto & view : _subViews) {
							// Compute position on grid.
							const int col = vid % sideCount;
							const int row = vid / sideCount;
							view.second.viewport = sibr::Viewport(col*itemSize[0], verticalShift + row*itemSize[1], (col + 1)*itemSize[0] - 1, verticalShift + (row + 1)*itemSize[1] - 1);
							view.second.shouldUpdateLayout = true;
							++vid;
						}
					}


					if (ImGui::MenuItem("Row layout")) {
						sibr::Vector2i itemSize = _window.size();
						itemSize[0] = int(float(itemSize[0]) / (_subViews.size() + _ibrSubViews.size()));
						const int verticalShift = ImGui::GetTitleBarHeight();
						int vid = 0;
						for (auto & view : _ibrSubViews) {
							// Compute position on grid.
							view.second.viewport = sibr::Viewport(vid*itemSize[0], verticalShift, (vid + 1)*itemSize[0] - 1, verticalShift + itemSize[1] - 1);
							view.second.shouldUpdateLayout = true;
							++vid;
						}
						for (auto & view : _subViews) {
							// Compute position on grid.
							view.second.viewport = sibr::Viewport(vid*itemSize[0], verticalShift, (vid + 1)*itemSize[0] - 1, verticalShift + itemSize[1] - 1);
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
						if (sibr::showFilePicker(selectedDirectory, FilePickerMode::Directory)) {
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

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}
		
		// Render all views.
		for (auto & subview : _ibrSubViews) {
			if (subview.second.view->active()) {

				renderSubView(subview.second);

				if (_showGUI) {
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
				
				if (_showGUI) {
					subview.second.view->onGUI();
					if (subview.second.handler) {
						subview.second.handler->onGUI("Camera " + subview.first);
					}
				}
			}
		}

		_fpsCounter.update(_showGUI);

	}

	void MultiViewManager::addSubView(const std::string & title, ViewBase::Ptr view, const Vector2u & res, const ImGuiWindowFlags flags)
	{
		const ViewUpdateFonc updateFunc =
			[](ViewBase::Ptr& vi, Input& in, const Viewport& vp, const float dt) {
			vi->onUpdate(in, vp);
		};
		addSubView(title, view, updateFunc, res, flags);
	}

	void MultiViewManager::addSubView(const std::string & title, ViewBase::Ptr view, const ViewUpdateFonc updateFunc, const Vector2u & res, const ImGuiWindowFlags flags)
	{
		// We have to shift vertically to avoid an overlap with the menu bar.
		const Viewport viewport(0.0f, ImGui::GetTitleBarHeight(),
			res.x() > 0 ? res.x() : (float)_defaultViewResolution.x(),
			(res.y() > 0 ? res.y() : (float)_defaultViewResolution.y()) + ImGui::GetTitleBarHeight());
		RenderTargetRGB::Ptr rtPtr(new RenderTargetRGB((uint)viewport.finalWidth(), (uint)viewport.finalHeight(), SIBR_CLAMP_UVS));
		_subViews[title] = {view, rtPtr, viewport, title, flags, updateFunc };

	}

	void MultiViewManager::addIBRSubView(const std::string & title, ViewBase::Ptr view, const IBRViewUpdateFonc updateFunc, const Vector2u & res, const ImGuiWindowFlags flags, const bool defaultFuncUsed)
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

	void MultiViewManager::addIBRSubView(const std::string & title, ViewBase::Ptr view, const Vector2u & res, const ImGuiWindowFlags flags)
	{
		const auto updateFunc = [](ViewBase::Ptr& vi, Input& in, const Viewport& vp, const float dt) {
			vi->onUpdate(in, vp);
			return InputCamera();
		};
		addIBRSubView(title, view, updateFunc, res, flags, true);
	}

	void MultiViewManager::addIBRSubView(const std::string & title, ViewBase::Ptr view, const IBRViewUpdateFonc updateFunc, const Vector2u & res, const ImGuiWindowFlags flags)
	{
		addIBRSubView(title, view, updateFunc, res, flags, false);
	}

	ViewBase::Ptr & MultiViewManager::getIBRSubView(const std::string & title)
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

	Viewport & MultiViewManager::getIBRSubViewport(const std::string & title)
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

	void MultiViewManager::renderSubView(SubView & subview) const
	{
		bool invalidTexture = false;

		if (!_onPause) {

			// The rendering viewport is the same as the final viewport, but with a (0,0) origin.
			unsigned int w = (unsigned int)subview.viewport.finalWidth();
			unsigned int h = (unsigned int)subview.viewport.finalHeight();

			/// \todo Offline video dumping.
			//int camIdDump = 0;
			//// This requires a scene, a handler and the save flag on the camera. (TODO: move flag to the handler).
			//if (subview.handler != NULL && subview.handler->getCamera().needSave() && subview.view->getScenePtr()) {
			//	// Change size.
			//	w = subview.view->scene().args().win_width;
			//	h = subview.view->scene().args().win_height;
			//	// If we furthermore have to leave an image out, disable it in the scene.
			//	if (subview.view->scene().args().dumpLeave1Out) {
			//		// Get closest camera (should be the same camera)
			//		camIdDump = subview.view->scene().selectCameras(subview.handler->getCamera(), 1)[0];
			//		std::cout << "Leaving out image number " << camIdDump << std::endl;
			//		subview.view->scene().deactivateCamera(camIdDump);
			//		// Readjust the size using the reference camera.
			//		const auto & refCam = subview.view->scene().inputCameras()[camIdDump];
			//		w = std::min(1920,(int)refCam.w());
			//		h = (unsigned int)((float)w/ refCam.w())*refCam.h();
			//	}
			//}

			// Adjsut rendering size if needed.
			const Viewport renderViewport(0.0, 0.0, (float)w, (float)h);

			if (subview.rt->w() != w || subview.rt->h() != h) {
				subview.rt.reset(new RenderTargetRGB(w, h, SIBR_CLAMP_UVS));
				invalidTexture = true;
			}

			subview.render(_renderingMode, renderViewport);

			// Offline video dumping, continued. We ignore addiitonal rendering as those often are GUI overlays.
			//if (subview.handler != NULL && subview.handler->getCamera().needSave()) {
			//	sibr::ImageRGB frame;
			//	subview.rt->readBack(frame);
			//	frame.save(subview.handler->getCamera().savePath());
			//	// Restore the disabled camera.
			//	if (subview.view->getScenePtr() && subview.view->scene().args().dumpLeave1Out) {
			//		subview.view->scene().activateCamera(camIdDump);
			//	}
			//}
			//else if (subview.handler != NULL && subview.view->getScenePtr() &&(subview.view->scene().args().dumpLeave1Out || subview.view->scene().args().dumpPath != "")) {
			//	// We were asked to dump the video, but we don't need anymore: job done, we should signal the window that we want to exit.
			//	_window.close();
			//}

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

	ViewBase::Ptr MultiViewManager::removeSubView(const std::string & name)
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

	void MultiViewManager::renderingMode(const IRenderingMode::Ptr& mode)
	{
		_renderingMode = std::move(mode);
	}

	const Viewport MultiViewManager::getViewport(void) const
	{
		return Viewport(0.0f, 0.0f, (float)_defaultViewResolution.x(), (float)_defaultViewResolution.y());
	}

	void MultiViewManager::addCameraForView(const std::string & name, ICameraHandler::Ptr cameraHandler)
	{
		if (_subViews.count(name) > 0) {
			_subViews.at(name).handler = cameraHandler;
		}
		else if (_ibrSubViews.count(name) > 0) {
			_ibrSubViews.at(name).handler = cameraHandler;

			SubView & subview = _ibrSubViews.at(name);

			/*if (subview.handler != NULL && subview.view->getScenePtr())
				if (InteractiveCameraHandler * handler = dynamic_cast<InteractiveCameraHandler *>(subview.handler.get())) {
					if (subview.view->scene().args().dumpPath != "" || subview.view->scene().args().dumpLeave1Out) {
						if (subview.view->scene().args().outputDir == "") {
							std::cerr << "Can't dump Path or Rephotography, outPutDir must be specified" << std::endl;
						}
						else {

							std::string dumpPath;
							if (subview.view->scene().args().dumpPath != "") {
								dumpPath = subview.view->scene().args().dumpPath;

								if (boost::filesystem::extension(dumpPath) == ".out")
									handler->cameraRecorder().loadBundle(dumpPath, subview.view->scene().args().win_width, subview.view->scene().args().win_height);
								else
									handler->cameraRecorder().load(dumpPath);
							}
							else if (subview.view->scene().args().dumpLeave1Out) {
								handler->cameraRecorder().cams().clear();
								for (const sibr::InputCamera & cam : subview.view->scene().inputCameras()) {
									handler->cameraRecorder().cams().emplace_back(cam);
								}
							}

							std::string outputDir = subview.view->scene().args().outputDir;

							handler->cameraRecorder().playback();
							handler->cameraRecorder().saving(outputDir + "/");
						}

					}
				}*/
		}
		else {
			SIBR_WRG << "No view named <" << name << "> found." << std::endl;
		}

	}

	void MultiViewManager::addAdditionalRenderingForView(const std::string & name, const AdditionalRenderFonc renderFunc)
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

	void MultiViewManager::captureView(const SubView & view, const std::string& path, const std::string & filename) {

		const uint w = view.rt->w();
		const uint h = view.rt->h();

		sibr::ImageRGB renderingImg(w, h);

		view.rt->readBack(renderingImg);

		std::string finalPath = path + (!path.empty() ? "/" : "");
		if (!filename.empty()) {
			finalPath.append(filename);
		}
		else {
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);
			std::stringstream buffer;
			buffer << std::put_time(&tm, "%Y_%m_%d_%H_%M_%S");
			std::string autoName = view.name + "_" + buffer.str();
			finalPath.append(autoName + ".png");
		}

		sibr::makeDirectory(path);
		renderingImg.save(finalPath, false);
	}

} // namespace sibr
