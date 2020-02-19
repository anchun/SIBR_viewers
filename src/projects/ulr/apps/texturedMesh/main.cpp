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
	"Usage: " PROGRAM_NAME " -path <dataset-path or mesh-path>"    	                                "\n"
	;


struct TexturedMeshAppArgs :
	virtual BasicIBRAppArgs {
	Arg<std::string> textureImagePath = { "texture", "" ,"texture path"};
	Arg<std::string> meshPath = { "mesh", "", "mesh path" };
	Arg<bool> noScene = { "noScene" };
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
		BasicIBRScene::Ptr		scene;
		std::string texImgPath, meshPath;

		if (myArgs.noScene) {
			scene = BasicIBRScene::Ptr(new BasicIBRScene());

			if (myArgs.textureImagePath.get() != "") {
				meshPath = myArgs.dataset_path.get();
				texImgPath = myArgs.textureImagePath;
			}
		}
		else {
			// Specify scene initlaization options
			BasicIBRScene::SceneOptions initOpts;

			initOpts.cameras = false;
			initOpts.images = false;
			initOpts.mesh = false;
			initOpts.renderTargets = false;

			scene = BasicIBRScene::Ptr(new BasicIBRScene(myArgs, initOpts));
			meshPath = removeExtension(scene->data()->meshPath()) + ".obj";
			texImgPath = removeExtension(scene->data()->meshPath()) + "_u1_v1.png";

		}

		// Load the texture image and provide it to the scene
		sibr::ImageRGB inputTextureImg;
		if (sibr::fileExists(texImgPath)) {
			inputTextureImg.load(texImgPath);
			scene->inputMeshTextures().reset(new sibr::Texture2DRGB(inputTextureImg, SIBR_GPU_LINEAR_SAMPLING));
		}
		else {
			SIBR_ERR << "No mesh and texture found! Please specify path to mesh using --path and path to the mesh texture using --texture!" << std::endl;
			return 0;
		}

		Mesh::Ptr newMesh(new Mesh(true));
		newMesh->load(meshPath);
		scene->proxies()->replaceProxyPtr(newMesh);


		// check rendering size; if no rendering-size specified, use 1080p
		rendering_width = (rendering_width <= 0) ? 1920 : rendering_width;
		rendering_height = (rendering_height <= 0) ? 1080 : rendering_height;
		Vector2u usedResolution(rendering_width, rendering_height);

		const unsigned int sceneResWidth = usedResolution.x();
		const unsigned int sceneResHeight = usedResolution.y();


		TexturedMeshView::Ptr	texturedView(new TexturedMeshView(scene, sceneResWidth, sceneResHeight));


		// Raycaster.
		std::shared_ptr<sibr::Raycaster> raycaster = std::make_shared<sibr::Raycaster>();
		raycaster->init();
		raycaster->addMesh(scene->proxies()->proxy());

		// Camera handler for main view.
		sibr::InteractiveCameraHandler::Ptr generalCamera(new InteractiveCameraHandler());
		generalCamera->setup(scene->proxies()->proxyPtr(), Viewport(0, 0, (float)usedResolution.x(), (float)usedResolution.y()));


		// Add views to mvm.
		MultiViewManager        multiViewManager(window, false);
		multiViewManager.addIBRSubView("TM View view", texturedView, usedResolution, ImGuiWindowFlags_ResizeFromAnySide);
		multiViewManager.addCameraForView("TM View view", generalCamera);

		// Top view
		const std::shared_ptr<sibr::SceneDebugView>    topView(new sibr::SceneDebugView(scene, generalCamera, myArgs));
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
