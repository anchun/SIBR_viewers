#include <fstream>
#include <core/graphics/Window.hpp>
#include <core/view/MultiViewManager.hpp>
#include <projects/ulr/renderer/TexturedMeshView.hpp>
#include <core/scene/BasicIBRScene.hpp>
#include <core/raycaster/Raycaster.hpp>
#include <core/view/SceneDebugView.hpp>

#define PROGRAM_NAME "sibr_texturedMesh_app"
using namespace sibr;

const char* usage = ""
	"Usage: " PROGRAM_NAME " -path <dataset-path>"    	                                "\n"
	;


struct TexturedMeshAppArgs :
	virtual BasicIBRAppArgs {
	Arg<std::string> textureImagePath = { "texture", "" ,"texture path"};
	Arg<std::string> meshPath = { "mesh", "", "mesh path" };
};

int main( int ac, char** av )
{
	{

		// Parse Commad-line Args
		CommandLineArgs::parseMainArgs(ac, av);
		TexturedMeshAppArgs myArgs;
		
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


		std::string texImgPath, meshPath;

		if (myArgs.textureImagePath.get() != "") {
			texImgPath = myArgs.textureImagePath;
			meshPath = myArgs.meshPath;
		}
		else {
			texImgPath = removeExtension(scene->data()->meshPath()) + "_u1_v1.png";
			meshPath = removeExtension(scene->data()->meshPath()) + ".obj";
		}

		Mesh::Ptr newMesh(new Mesh(true));
		newMesh->load(meshPath);
		scene->proxies()->replaceProxyPtr(newMesh);

		sibr::ImageRGB inputTextureImg;
		if (sibr::fileExists(texImgPath)) {
			inputTextureImg.load(texImgPath);
			scene->inputMeshTextures().reset(new sibr::Texture2DRGB(inputTextureImg, SIBR_GPU_LINEAR_SAMPLING));
		}

		TexturedMeshView::Ptr	texturedView(new TexturedMeshView(scene, sceneResWidth, sceneResHeight));


		// Raycaster.
		std::shared_ptr<sibr::Raycaster> raycaster = std::make_shared<sibr::Raycaster>();
		raycaster->init();
		raycaster->addMesh(scene->proxies()->proxy());

		// Camera handler for main view.
		sibr::InteractiveCameraHandler::Ptr generalCamera(new InteractiveCameraHandler());
		generalCamera->setup(scene->cameras()->inputCameras(), Viewport(0, 0, (float)usedResolution.x(), (float)usedResolution.y()), raycaster);


		// Add views to mvm.
		MultiViewManager        multiViewManager(window, false);
		multiViewManager.addIBRSubView("ULR view", texturedView, usedResolution, ImGuiWindowFlags_ResizeFromAnySide);
		multiViewManager.addCameraForView("ULR view", generalCamera);

		// Top view
		const std::shared_ptr<sibr::SceneDebugView>    topView(new sibr::SceneDebugView(scene, multiViewManager.getViewport(), generalCamera, myArgs));
		multiViewManager.addSubView("Top view", topView, usedResolution);

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
