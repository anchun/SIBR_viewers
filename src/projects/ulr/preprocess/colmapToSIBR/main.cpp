#include <fstream>
#include <iostream>
#include <core/system/CommandLineArgs.hpp>
#include <core/view/BasicIBRScene.hpp>
#include <core/raycaster/CameraRaycaster.hpp>
#include <core/assets/ImageListFile.hpp>
#include <core/system/Utils.hpp>


#define PROGRAM_NAME "sibr_colmap_to_sibr"
using namespace sibr;

const char* usage = ""
"Usage: " PROGRAM_NAME " -path " "\n"
;

struct ColmapPreprocessArgs :
	virtual BasicIBRAppArgs {
	Arg<bool> textureAlpha = { "textureAlpha" };
};

int main(const int argc, const char** argv)
{

	CommandLineArgs::parseMainArgs(argc, argv);
	ColmapPreprocessArgs myArgs;

	std::string pathScene = myArgs.dataset_path;

	std::vector<std::string> dirs = {"cameras", "images", "meshes", "textures"};

	std::cout << "Generating SIBR scene." << std::endl;
	BasicIBRScene scene(myArgs, true);

	// load the cams
	std::vector<InputCamera>	cams = scene.cameras()->inputCameras();
	const int maxCam = cams.size();
	const int minCam = 0;

	for (auto dir : dirs) {
		std::cout << dir << std::endl;
		if (!boost::filesystem::exists(pathScene + "/" + dir.c_str())) {
			sibr::makeDirectory(pathScene + "/" + dir.c_str());
		}
	}
	if (myArgs.textureAlpha) {
		sibr::makeDirectory(pathScene + "/textureAlpha/");
	}

	std::ofstream outputBundleCam;
	std::ofstream outputListIm;

	outputBundleCam.open(pathScene + "/cameras/bundle.out");
	outputListIm.open(pathScene + "/images/list_images.txt");
	outputBundleCam << "# Bundle file v0.3" << std::endl;
	outputBundleCam << maxCam << " " << 0 << std::endl;
	
	
	for (int c = minCam; c < maxCam; c++) {
		auto & camIm = cams[c];
		
		//InputCamera newCam(camIm, 2222, 2222);
		std::string extensionFile = boost::filesystem::extension(camIm.name());
		std::ostringstream ssZeroPad;
		ssZeroPad << std::setw(8) << std::setfill('0') << camIm.id();
		std::string newFileName = ssZeroPad.str() + extensionFile;

		boost::filesystem::copy_file(pathScene + "/colmap/stereo/images/" + camIm.name(), pathScene + "/images/" + newFileName, boost::filesystem::copy_option::overwrite_if_exists);
		outputBundleCam << camIm.toBundleString();
		outputListIm << newFileName << " " << camIm.w() << " " << camIm.h() << std::endl;
	}

	outputBundleCam.close();
	outputListIm.close();

	const std::string meshPath = pathScene + "/capreal/mesh.ply";
	sibr::copyFile(meshPath, pathScene + "/meshes/recon.ply", true);

	return EXIT_SUCCESS;
}