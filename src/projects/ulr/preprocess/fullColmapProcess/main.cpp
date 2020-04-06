#include <fstream>
#include <iostream>
#include <core/system/CommandLineArgs.hpp>
#include <core/scene/BasicIBRScene.hpp>
#include <core/raycaster/CameraRaycaster.hpp>
#include <core/assets/ImageListFile.hpp>
#include <core/system/Utils.hpp>
#include <boost/process.hpp> 

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
	Arg<uint>	siftExtraction_ImageSize = 
				{"SiftExtraction.max_image_size",0,"( default value : 3200)"};
	Arg<uint>	siftExtraction_EstimateAffineShape = 
				{"SiftExtraction.estimate_affine_shape",0,"( default value : 0)"}; 
	Arg<uint>	siftExtraction_DomainSizePooling = 
				{"SiftExtraction.domain_size_pooling",0,"( default value :0)"};
	Arg<uint>	siftExtraction_MaxNumFeatures = 
				{"SiftExtraction.max_num_features",0,"( default value : 8192)"};

	//Exhaustive matcher
	Arg<uint>	exhaustiveMatcher_ExhaustiveMatchingBlockSize = 
				{"ExhaustiveMatching.block_size",0,"( default value : 50)"};

	//Mapper
	Arg<uint>	mapper_MapperDotbaLocalMaxNumIterations = 
				{"Mapper.ba_local_max_num_iterations",0,"( default value : 25 )"};
	Arg<uint>	mapper_MapperDotbaGlobalMaxNumIterations = 
				{"Mapper.ba_global_max_num_iterations",0,"( default value : 50)"};
	Arg<float>	mapper_MapperDotbaGlobalImagesRatio = 
				{"Mapper.ba_global_images_ratio",0,"( default value : 1.100001)"};
	Arg<float>	mapper_MapperDotbaGlobalPointsRatio = 
				{"Mapper.ba_global_points_ratio",0,"( default value : 1.100001)"};
	Arg<uint>	mapper_MapperDotbaGlobalMaxRefinements = 
				{"Mapper.ba_global_max_refinements",0,"( default value : 5)"};
	Arg<uint>	mapper_MapperDotbaLocalMaxRefinements = 
				{"Mapper.ba_local_max_refinements",0,"( default value : 2)"};

	//Patch match stereo
	Arg<int>	patchMatchStereo_PatchMatchStereoDotMaxImageSize = 
				{"PatchMatchStereo.max_image_size",0,"( default value : -1)"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotWindowRadius = 
				{"PatchMatchStereo.window_radius",0,"( default value : 5)"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotWindowStep = 
				{"PatchMatchStereo.window_step",0,"( default value : 1)"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotNumSamples = 
				{"PatchMatchStereo.num_samples",0,"( default value : 15)"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotNumIterations = 
				{"PatchMatchStereo.num_iterations",0,"( default value : 5)"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotGeomConsistency = 
				{"PatchMatchStereo.geom_consistency",0,"( default value : 5)"};

	//Stereo fusion
	Arg<uint>	stereoFusion_CheckNumImages = 
				{"StereoFusion.check_num_images",0,"( default value : 50)"};
	Arg<uint>	stereoFusion_MaxImageSize = 
				{"StereoFusion.max_image_size",0,"( default value : -1)"};

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

void setPersonalParameters(const CommandLineArgs& globalArgs,
						const FullProcessColmapPreprocessArgs& userArgs,
						ColmapParameters& parameters) {

	if (globalArgs.contains("SiftExtraction.max_image_size")) {
		parameters.siftExtractionImageSize(
			userArgs.siftExtraction_ImageSize.get());
	}
	if (globalArgs.contains("SiftExtraction.estimate_affine_shape")) {
		parameters.siftExtractionEstimateAffineShape(
			userArgs.siftExtraction_EstimateAffineShape.get());
	}
	if (globalArgs.contains("SiftExtraction.domain_size_pooling")) {
		parameters.siftExtractionDomainSizePooling(
			userArgs.siftExtraction_DomainSizePooling.get());
	}
	if (globalArgs.contains("SiftExtraction.max_num_features")) {
		parameters.siftExtractionMaxNumFeatures (
			userArgs.siftExtraction_MaxNumFeatures.get());
	}

	if (globalArgs.contains("ExhaustiveMatching.block_size")) {
		parameters.exhaustiveMatcherExhaustiveMatchingBlockSize (
			userArgs.exhaustiveMatcher_ExhaustiveMatchingBlockSize.get());
	}

	if (globalArgs.contains("Mapper.ba_local_max_num_iterations")) {
		parameters.mapperMapperDotbaLocalMaxNumIterations (
			userArgs.mapper_MapperDotbaLocalMaxNumIterations.get());
	}
	if (globalArgs.contains("Mapper.ba_global_max_num_iterations")) {
		parameters.mapperMapperDotbaGlobalMaxNumIterations (
			userArgs.mapper_MapperDotbaGlobalMaxNumIterations.get());
	}
	if (globalArgs.contains("Mapper.ba_global_images_ratio")) {
		parameters.mapperMapperDotbaGlobalImagesRatio (
			userArgs.mapper_MapperDotbaGlobalImagesRatio.get());
	}
	if (globalArgs.contains("Mapper.ba_global_points_ratio")) {
		parameters.mapperMapperDotbaGlobalPointsRatio (
			userArgs.mapper_MapperDotbaGlobalImagesRatio.get());
	}
	if (globalArgs.contains("Mapper.ba_global_max_refinements")) {
		parameters.mapperMapperDotbaGlobalMaxRefinements (
			userArgs.mapper_MapperDotbaGlobalMaxRefinements.get());
	}
	if (globalArgs.contains("Mapper.ba_local_max_refinements")) {
		parameters.mapperMapperDotbaLocalMaxRefinements (
			userArgs.mapper_MapperDotbaLocalMaxRefinements.get());
	}

	if (globalArgs.contains("PatchMatchStereo.max_image_size")) {
		parameters.patchMatchStereoPatchMatchStereoDotMaxImageSize (
			userArgs.patchMatchStereo_PatchMatchStereoDotMaxImageSize.get());
	}
	if (globalArgs.contains("PatchMatchStereo.window_radius")) {
		parameters.patchMatchStereoPatchMatchStereoDotWindowRadius (
			userArgs.patchMatchStereo_PatchMatchStereoDotWindowRadius.get());
	}
	if (globalArgs.contains("PatchMatchStereo.window_step")) {
		parameters.patchMatchStereoPatchMatchStereoDotWindowStep (
			userArgs.patchMatchStereo_PatchMatchStereoDotWindowStep.get());
	}
	if (globalArgs.contains("PatchMatchStereo.num_samples")) {
		parameters.patchMatchStereoPatchMatchStereoDotNumSamples (
			userArgs.patchMatchStereo_PatchMatchStereoDotNumSamples.get());
	}
	if (globalArgs.contains("PatchMatchStereo.num_iterations")) {
		parameters.patchMatchStereoPatchMatchStereoDotNumIterations (
			userArgs.patchMatchStereo_PatchMatchStereoDotNumIterations.get());
	}
	if (globalArgs.contains("PatchMatchStereo.geom_consistency")) {
		parameters.patchMatchStereoPatchMatchStereoDotGeomConsistency (
			userArgs.patchMatchStereo_PatchMatchStereoDotGeomConsistency.get());
	}

	if (globalArgs.contains("StereoFusion.check_num_images")) {
		parameters.stereoFusionCheckNumImages (
			userArgs.stereoFusion_CheckNumImages.get());
	}
	if (globalArgs.contains("StereoFusion.max_image_size")) {
		parameters.stereoFusionMaxImageSize (
			userArgs.stereoFusion_MaxImageSize.get());
	}
}

void runColmap(const std::string& colmapPath) {
	
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
	const std::shared_ptr < ColmapParameters::Quality > qualityRecon =
		getUserQuality(globalArgs, myArgs);
	if (!qualityRecon) { 
		return EXIT_FAILURE; 
	}

	//boost::process::system

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