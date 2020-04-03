#include <fstream>
#include <iostream>
#include <core/system/CommandLineArgs.hpp>
#include <core/scene/BasicIBRScene.hpp>
#include <core/raycaster/CameraRaycaster.hpp>
#include <core/assets/ImageListFile.hpp>
#include <core/system/Utils.hpp>


#define PROGRAM_NAME "sibr_full_process_colmap"
using namespace sibr;

const char* usage = ""
"Usage: " PROGRAM_NAME " -path " "\n"
;

struct FullProcessColmapPreprocessArgs :
	virtual BasicIBRAppArgs {
	RequiredArg<std::string> colmapPath = { "colmapPath","colmap .bat path directory" };
	Arg<std::string> quality = { "quality","high","quality of the reconstruction" };
	Arg<uint> numberOfGPUs = { "GPU",1,"Number of GPUs" };
};

struct ColmapParameters {
	enum class Qualities { LOW, MEDIUM, HIGH, EXTREME };
	
};

int main(const int argc, const char** argv)
{

	CommandLineArgs::parseMainArgs(argc, argv);
	FullProcessColmapPreprocessArgs myArgs;

	//------------------Quality Argument---------------//
	constexpr uint numberOfQualities = 4;
	std::array<std::string, numberOfQualities> qualitiesList
	{ "low","medium","high","extreme" };

	bool foundOption = false;
	std::string qualitiesString;
	for ( const std::string& quality : qualitiesList){
			qualitiesString.append(quality);
			qualitiesString.append(" ");
			if (myArgs.quality.get().compare(quality) == 0) {
				foundOption = true;
			}
	}
	if (!foundOption) {
		SIBR_ERR << "Your quality paremeter is unknown ... the available values are:"
			<< qualitiesString << std::endl;
		return EXIT_FAILURE;
	}
	//-------------------------------------------------//

	//-----------------PATH ARGUMENT-------------------//
	const std::string pathScene = myArgs.dataset_path;
	if (!directoryExists(pathScene + "/images")) {

		SIBR_ERR << "Your path does not contain an \"images\" directory..." << std::endl
			<< "Please create one and put your images into this" << std::endl;

		return EXIT_FAILURE;
	}
	//-------------------------------------------------//


	const std::vector<std::string> colmapDirs = { "cameras", "images", "meshes"};


	std::vector<std::string> dirs = { "cameras", "images", "meshes"};

	std::cout << "Generating SIBR scene." << std::endl;
	BasicIBRScene scene(myArgs, true, true);

	// load the cams
	std::vector<InputCamera>	cams = scene.cameras()->inputCameras();
	const int maxCam = int(cams.size());
	const int minCam = 0;

	for (auto dir : dirs) {
		std::cout << dir << std::endl;
		if (!directoryExists(pathScene + "/" + dir.c_str())) {
			makeDirectory(pathScene + "/" + dir.c_str());
		}
	}


	return EXIT_SUCCESS;
}