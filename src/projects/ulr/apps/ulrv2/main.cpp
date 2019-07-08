#include <fstream>

#include <core/graphics/Window.hpp>
#include <core/view/MultiViewManager.hpp>

#include "projects/ulr/renderer/ULRView.hpp"
#include <projects/ulr/renderer/ULRV2View.hpp>
#include <projects/ulr/renderer/ULRV3View.hpp>

#include <core/renderer/DepthRenderer.hpp>
#include <core/raycaster/Raycaster.hpp>
#include <core/view/SceneDebugView.hpp>

#include <core/view/ImagesGrid.hpp>

#define PROGRAM_NAME "sibr_ulr_app"
using namespace sibr;

const char* usage = ""
"Usage: " PROGRAM_NAME " -path <dataset-path>"    	                                "\n"
;


int legacyV1main(ULRAppArgs & myArgs);
int legacyV2main(ULRAppArgs & myArgs);

int main(int ac, char** av) {

	// Parse Commad-line Args
	CommandLineArgs::parseMainArgs(ac, av);
	ULRAppArgs myArgs;


	if (myArgs.version == 2) {
		return legacyV2main(myArgs);
	}
	if (myArgs.version == 1) {
		return legacyV1main(myArgs);
	}


	const bool doVSync = !myArgs.vsync;
	// rendering size
	uint rendering_width = myArgs.rendering_size.get()[0];
	uint rendering_height = myArgs.rendering_size.get()[1];
	// window size
	uint win_width = myArgs.win_width;
	uint win_height = myArgs.win_height;

	// Window setup
	sibr::Window		window(PROGRAM_NAME, sibr::Vector2i(50, 50), myArgs);

	BasicIBRScene::Ptr		scene(new BasicIBRScene(myArgs, true));

	// Setup the scene: load the proxy, create the texture arrays.
	const uint flags = SIBR_GPU_LINEAR_SAMPLING | SIBR_FLIP_TEXTURE;

	// check rendering size
	rendering_width = (rendering_width <= 0) ? scene->cameras()->inputCameras()[0].w() : rendering_width;
	rendering_height = (rendering_height <= 0) ? scene->cameras()->inputCameras()[0].h() : rendering_height;
	Vector2u usedResolution(rendering_width, rendering_height);

	const unsigned int sceneResWidth = usedResolution.x();
	const unsigned int sceneResHeight = usedResolution.y();
	scene->renderTargets()->initRGBandDepthTextureArrays(scene->cameras(), scene->images(), scene->proxies(), flags);

	// Create the ULR view.
	ULRV3View::Ptr	ulrView(new ULRV3View(scene, sceneResWidth, sceneResHeight));

	// Check if masks are provided and enabled.
	if (myArgs.masks) {
		if (!myArgs.maskParams.get().empty()) {
			if (!myArgs.maskParamsExtra.get().empty()) {
				ulrView->getULRrenderer()->loadMasks(scene, myArgs.maskParams.get(), "", myArgs.maskParamsExtra.get());
			}
			else {
				ulrView->getULRrenderer()->loadMasks(scene, myArgs.maskParams.get(), "", ".png");
			}
		}
		else {
			ulrView->getULRrenderer()->loadMasks(scene);
		}
		ulrView->getULRrenderer()->useMasks() = true;
	}

	// Raycaster.
	std::shared_ptr<sibr::Raycaster> raycaster = std::make_shared<sibr::Raycaster>();
	raycaster->init();
	raycaster->addMesh(scene->proxies()->proxy());

	// Camera handler for main view.
	sibr::InteractiveCameraHandler::Ptr generalCamera(new InteractiveCameraHandler());
	generalCamera->setup(scene->cameras()->inputCameras(), Viewport(0, 0, (float)usedResolution.x(), (float)usedResolution.y()), raycaster);



	// Add views to mvm.
	MultiViewManager        multiViewManager(window, false);
	multiViewManager.addIBRSubView("ULR view", ulrView, usedResolution, ImGuiWindowFlags_ResizeFromAnySide);
	multiViewManager.addCameraForView("ULR view", generalCamera);

	// Top view
	const std::shared_ptr<sibr::SceneDebugView> topView(new sibr::SceneDebugView(scene, multiViewManager.getViewport(), generalCamera, myArgs));
	multiViewManager.addSubView("Top view", topView, usedResolution);


	//test grid view - can be removed
	if (getCommandLineArgs().contains("test-grid")) {
		const std::shared_ptr<sibr::ImagesGrid> grid(new ImagesGrid());
		grid->addImageLayer("input images", scene->images()->inputImages(), 10);

		if (ulrView->getULRrenderer()->useMasks()) {
			grid->addImageLayer("masks", ulrView->getULRrenderer()->getMasks());
		}

		multiViewManager.addSubView("grid", grid, usedResolution);
	}

	CHECK_GL_ERROR;

	// Main looooooop.
	while (window.isOpened()) {

		sibr::Input::poll();
		window.makeContextCurrent();
		if (sibr::Input::global().key().isPressed(sibr::Key::Escape)) {
			window.close();
		}

		multiViewManager.onUpdate(sibr::Input::global());
		multiViewManager.onRender(window);

		window.swapBuffer();
		CHECK_GL_ERROR;
	}

	return EXIT_SUCCESS;
}

/// Use ULRV2 view and renderer.
int legacyV2main(ULRAppArgs & myArgs)
{

	{

		const bool doVSync = !myArgs.vsync;
		// rendering size
		uint rendering_width = myArgs.rendering_size.get()[0];
		uint rendering_height = myArgs.rendering_size.get()[1];
		// window size
		uint win_width = myArgs.win_width;
		uint win_height = myArgs.win_height;

		// Window setup
		sibr::Window		window(PROGRAM_NAME, sibr::Vector2i(50, 50), myArgs);

		BasicIBRScene::Ptr		scene(new BasicIBRScene(myArgs));
		

		// check rendering size
		rendering_width = (rendering_width <= 0) ? scene->cameras()->inputCameras()[0].w() : rendering_width;
		rendering_height = (rendering_height <= 0) ? scene->cameras()->inputCameras()[0].h() : rendering_height;
		Vector2u usedResolution(rendering_width, rendering_height);

		const unsigned int sceneResWidth = usedResolution.x();
		const unsigned int sceneResHeight = usedResolution.y();

		ULRV2View::Ptr	ulrView(new ULRV2View(scene, sceneResWidth, sceneResHeight));
		ulrView->setNumBlend(40, 40);

		// Raycaster.
		std::shared_ptr<sibr::Raycaster> raycaster = std::make_shared<sibr::Raycaster>();
		raycaster->init();
		raycaster->addMesh(scene->proxies()->proxy());

		// Camera handler for main view.
		sibr::InteractiveCameraHandler::Ptr generalCamera(new InteractiveCameraHandler());
		generalCamera->setup(scene->cameras()->inputCameras(), Viewport(0, 0, (float)usedResolution.x(), (float)usedResolution.y()), raycaster);


		// Add views to mvm.
		MultiViewManager        multiViewManager(window, false);
		multiViewManager.addIBRSubView("ULR view", ulrView, usedResolution, ImGuiWindowFlags_ResizeFromAnySide);
		multiViewManager.addCameraForView("ULR view", generalCamera);

		// Top view
		const std::shared_ptr<sibr::SceneDebugView>    topView(new sibr::SceneDebugView(scene, multiViewManager.getViewport(), generalCamera, myArgs));
		multiViewManager.addSubView("Top view", topView, usedResolution);

		// Soft Visibility masks
		std::vector<sibr::ImageL32F>	depths3D(scene->cameras()->inputCameras().size());
		if (myArgs.softVisibility) {

			int numImages = (int)scene->cameras()->inputCameras().size();

			for (int imId = 0; imId < numImages; ++imId) {

				sibr::InputCamera cam = scene->cameras()->inputCameras()[imId];
				sibr::Vector3f camPos = cam.position();
				int w = cam.w();
				int h = cam.h();

				sibr::DepthRenderer rendererDepth(cam.w(), cam.h());

				sibr::ImageL32F depthMapSIBR(w, h);

				rendererDepth.render(cam, scene->proxies()->proxy());
				rendererDepth._depth_RT->readBack(depthMapSIBR);

				depths3D[imId] = sibr::ImageL32F(w, h, 0);

				for (int i = 0; i < w; i++) {
					for (int j = 0; j < h; j++) {
						sibr::Vector2i pixelPos(i, j);
						sibr::Vector3f pos3dMesh(cam.unprojectImgSpaceInvertY(pixelPos, depthMapSIBR(i, j).x()));
						depths3D[imId](i, j).x() = (camPos - pos3dMesh).norm();
					}
				}
				//showFloat(depthMapSIBR);
				//showFloat(depths3D[imId]);
			}
		}


		if (myArgs.masks) {
			if (!myArgs.maskParams.get().empty()) {
				ulrView->loadMasks(scene, usedResolution.x(), usedResolution.y(), myArgs.maskParams.get(), "", ".png");
			}
			else {
				ulrView->loadMasks(scene, usedResolution.x(), usedResolution.y());
			}

		}
		if (myArgs.invert) {
			ulrView->_ulr->setDoInvertMasks(true);
		}
		if (myArgs.alphas) {
			ulrView->_ulr->setAreMasksBinary(false);
		}
		if (myArgs.poisson) {
			ulrView->noPoissonBlend(true);
		}

		Texture2DArrayLum32F	soft_visibility_textures;
		if (myArgs.softVisibility) {
			int numImages = (int)scene->cameras()->inputCameras().size();
			std::vector<sibr::ImageL32F>	softVisibilities(numImages);

			int wSoft = depths3D[0].w();
			int hSoft = depths3D[0].h();
#pragma omp parallel for
			for (int imId = 0; imId < numImages; ++imId) {

				sibr::ImageRGBA tempVisibility;
				ulrView->computeVisibilityMap(depths3D[imId], tempVisibility);
				softVisibilities[imId] = std::move(sibr::convertRGBAtoL32F(tempVisibility));
				cv::Mat temp;
				cv::resize(softVisibilities[imId].toOpenCV(), temp, cv::Size(wSoft, hSoft), 0, 0, cv::INTER_NEAREST);
				softVisibilities[imId].fromOpenCV(temp);
			}

			soft_visibility_textures.createFromImages(softVisibilities, SIBR_GPU_LINEAR_SAMPLING | SIBR_FLIP_TEXTURE);

			ulrView->_ulr->getSoftVisibilityMaps() = &soft_visibility_textures;
			ulrView->_ulr->getSoftVisibilityThreshold() = 20.0f;
			//ulrView->noPoissonBlend(true);

		}

		CHECK_GL_ERROR;
		while (window.isOpened())
		{


			sibr::Input::poll();
			window.makeContextCurrent();
			if (sibr::Input::global().key().isPressed(sibr::Key::Escape))
				window.close();

			if (sibr::Input::global().key().isPressed(sibr::Key::Z)) {
				if (ulrView->_ulr->getSoftVisibilityMaps()) {
					std::cout << " disabling soft visibility" << std::endl;
					ulrView->_ulr->getSoftVisibilityMaps() = nullptr;
				}
				else {
					std::cout << " enabling soft visibility" << std::endl;
					ulrView->_ulr->getSoftVisibilityMaps() = &soft_visibility_textures;
				}
			}

			multiViewManager.onUpdate(sibr::Input::global());
			multiViewManager.onRender(window);
			window.swapBuffer();
			CHECK_GL_ERROR;
		}
	}


	return EXIT_SUCCESS;
}

/// Use ULRV1 view and renderer.
int legacyV1main(ULRAppArgs & myArgs)
{

	{

		const bool doVSync = !myArgs.vsync;
		// rendering size
		uint rendering_width = myArgs.rendering_size.get()[0];
		uint rendering_height = myArgs.rendering_size.get()[1];
		// window size
		uint win_width = myArgs.win_width;
		uint win_height = myArgs.win_height;

		// Window setup
		sibr::Window        window(PROGRAM_NAME, sibr::Vector2i(50, 50), myArgs);

		// Setup IBR
		BasicIBRScene::Ptr		scene(new BasicIBRScene(myArgs));
		
		// check rendering size
		rendering_width = (rendering_width <= 0) ? scene->cameras()->inputCameras()[0].w() : rendering_width;
		rendering_height = (rendering_height <= 0) ? scene->cameras()->inputCameras()[0].h() : rendering_height;
		Vector2u usedResolution(rendering_width, rendering_height);

		const unsigned int sceneResWidth = usedResolution.x();
		const unsigned int sceneResHeight = usedResolution.y();

		ULRView::Ptr	ulrView(new ULRView(scene, sceneResWidth, sceneResHeight));
		ulrView->setNumBlend(50, 50);

		// Raycaster.
		std::shared_ptr<sibr::Raycaster> raycaster = std::make_shared<sibr::Raycaster>();
		raycaster->init();
		raycaster->addMesh(scene->proxies()->proxy());

		// Camera handler for main view.
		sibr::InteractiveCameraHandler::Ptr generalCamera(new InteractiveCameraHandler());
		generalCamera->setup(scene->cameras()->inputCameras(), Viewport(0, 0, (float)usedResolution.x(), (float)usedResolution.y()), raycaster);


		// Add views to mvm.
		MultiViewManager        multiViewManager(window, false);
		multiViewManager.addIBRSubView("ULR view", ulrView, usedResolution, ImGuiWindowFlags_ResizeFromAnySide);
		multiViewManager.addCameraForView("ULR view", generalCamera);

		// Top view
		const std::shared_ptr<sibr::SceneDebugView>    topView(new sibr::SceneDebugView(scene, multiViewManager.getViewport(), generalCamera, myArgs));
		multiViewManager.addSubView("Top view", topView);

		while (window.isOpened())
		{
			sibr::Input::poll();
			window.makeContextCurrent();
			if (sibr::Input::global().key().isPressed(sibr::Key::Escape))
				window.close();

			multiViewManager.onUpdate(sibr::Input::global());
			multiViewManager.onRender(window);
			window.swapBuffer();
			CHECK_GL_ERROR
		}

	}
	return EXIT_SUCCESS;
}