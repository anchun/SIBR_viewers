#include <fstream>
#include <iostream>
#include <core/system/CommandLineArgs.hpp>
#include <core/scene/BasicIBRScene.hpp>
#include <core/raycaster/CameraRaycaster.hpp>
#include <core/assets/ImageListFile.hpp>
#include <core/system/Utils.hpp>
#include "ColmapParameters.h"


#define PROGRAM_NAME "sibr_full_process_colmap"
using namespace sibr;

const char* usage = ""
"Usage: " PROGRAM_NAME " -path " "\n"
;

struct FullProcessColmapPreprocessArgs :
	virtual BasicIBRAppArgs {
	RequiredArg<std::string>	colmapPath = { "colmapPath","colmap .bat path directory" };

	Arg<std::string>			quality = 
								{ "quality","","quality of the reconstruction" };
	Arg<uint>					numberOfGPUs = { "GPU",1,"Number of GPUs" };

	//Feature extractor 
	Arg<uint>	_siftExtraction_ImageSize = 
				{"",0,"( default value : 3200)"};
	Arg<uint>	_siftExtraction_EstimateAffineShape = 
				{"",0,"( default value : 0)"}; 
	Arg<uint>	_siftExtraction_DomainSizePooling = 
				{"",0,"( default value :0)"};
	Arg<uint>	_siftExtraction_MaxNumFeatures = 
				{"",0,"( default value : 8192)"};

	//Exhaustive matcher
	Arg<uint>	_exhaustiveMatcher_ExhaustiveMatchingBlockSize = 
				{"",0,"( default value : 50)"};

	//Mapper
	Arg<uint>	_mapper_MapperDotbaLocalMaxNumIterations = 
				{"",0,"( default value : 25 )"};
	Arg<uint>	_mapper_MapperDotbaGlobalMaxNumIterations = 
				{"",0,"( default value : 50)"};
	Arg<float>	_mapper_MapperDotbaGlobalImagesRatio = 
				{"",0,"( default value : 1.100001)"};
	Arg<float>	_mapper_MapperDotbaGlobalPointsRatio = 
				{"",0,"( default value : 1.100001)"};
	Arg<uint>	_mapper_MapperDotbaGlobalMaxRefinements = 
				{"",0,"( default value : 5)"};
	Arg<uint>	_mapper_MapperDotbaLocalMaxRefinements = 
				{"",0,"( default value : 2)"};

	//Patch match stereo
	Arg<int>	_patchMatchStereo_PatchMatchStereoDotMaxImageSize = 
				{"",0,"( default value : -1)"};
	Arg<uint>	_patchMatchStereo_PatchMatchStereoDotWindowRadius = 
				{"",0,"( default value : 5)"};
	Arg<uint>	_patchMatchStereo_PatchMatchStereoDotWindowStep = 
				{"",0,"( default value : 1)"};
	Arg<uint>	_patchMatchStereo_PatchMatchStereoDotNumSamples = 
				{"",0,"( default value : 15)"};
	Arg<uint>	_patchMatchStereo_PatchMatchStereoDotNumIterations = 
				{"",0,"( default value : 5)"};
	Arg<uint>	_patchMatchStereo_PatchMatchStereoDotGeomConsistency = 
				{"",0,"( default value : 5)"};

	//Stereo fusion
	Arg<uint>	_stereoFusion_CheckNumImages = 
				{"",0,"( default value : 50)"};
	Arg<uint>	_stereoFusion_MaxImageSize = 
				{"",0,"( default value : -1)"};

};

std::shared_ptr<ColmapParameters::Quality> getUserQuality(
			const CommandLineArgs& globalArgs,
			const FullProcessColmapPreprocessArgs& userArgs) {

	std::shared_ptr<ColmapParameters::Quality> userQuality;
	if (globalArgs.contains("quality")) {
		const std::string userQuality = userArgs.quality.get();
		return ColmapParameters::stringToQuality(userQuality);
	}
	else {
		userQuality = std::make_shared<ColmapParameters::Quality>(
			ColmapParameters::Quality::DEFAULT );
	}
	return userQuality;
}
int main(const int argc, const char** argv)
{

	CommandLineArgs::parseMainArgs(argc, argv);
	CommandLineArgs globalArgs = CommandLineArgs::getGlobal();
	FullProcessColmapPreprocessArgs myArgs;

	//------------------Quality Argument---------------//
	//-------------------------------------------------//

	//-----------------PATH ARGUMENT-------------------//
	const std::string pathScene = myArgs.dataset_path;
	if (!directoryExists(pathScene + "/images")) {

		SIBR_ERR << "Your path does not contain an \"images\" directory..." << std::endl
			<< "Please create one and put your images into this" << std::endl;

		return EXIT_FAILURE;
	}
	//-------------------------------------------------//

	//----------------COLMAP PARAMETERS----------------//
	std::shared_ptr < ColmapParameters::Quality > qualityRecon =
		getUserQuality(globalArgs, myArgs);
	if (!qualityRecon) { 
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