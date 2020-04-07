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
	RequiredArg<std::string>	colmapPath = { "colmapPath","colmap path directory which contains COLMAP.bat" };
	RequiredArg<std::string>	sibrBinariesPath = { "sibrBinariesPath","binaries directory of SIBR" };
	Arg<std::string>			quality = 
								{ "quality","","quality of the reconstruction" };
	Arg<uint>					numGPUs = { "numGPUs",1,"Number of GPUs" };

	//Feature extractor 
	Arg<uint>	siftExtraction_ImageSize = 
				{"SiftExtraction.max_image_size",3200,"colmap feature extractor param"};
	Arg<uint>	siftExtraction_EstimateAffineShape = 
				{"SiftExtraction.estimate_affine_shape",0,"colmap feature extractor param"}; 
	Arg<uint>	siftExtraction_DomainSizePooling = 
				{"SiftExtraction.domain_size_pooling",0,"colmap feature extractor param"};
	Arg<uint>	siftExtraction_MaxNumFeatures = 
				{"SiftExtraction.max_num_features",8192,"colmap feature extractor param"};

	//Exhaustive matcher
	Arg<uint>	exhaustiveMatcher_ExhaustiveMatchingBlockSize = 
				{"ExhaustiveMatching.block_size",50,"colmap exhaustive matcher param"};

	//Mapper
	Arg<uint>	mapper_MapperDotbaLocalMaxNumIterations = 
				{"Mapper.ba_local_max_num_iterations",25,"colmap mapper param"};
	Arg<uint>	mapper_MapperDotbaGlobalMaxNumIterations = 
				{"Mapper.ba_global_max_num_iterations",50,"colmap mapper param"};
	Arg<float>	mapper_MapperDotbaGlobalImagesRatio = 
				{"Mapper.ba_global_images_ratio",1.10001f,"colmap mapper param"};
	Arg<float>	mapper_MapperDotbaGlobalPointsRatio = 
				{"Mapper.ba_global_points_ratio",1.10001f,"colmap mapper param"};
	Arg<uint>	mapper_MapperDotbaGlobalMaxRefinements = 
				{"Mapper.ba_global_max_refinements",5,"colmap mapper param"};
	Arg<uint>	mapper_MapperDotbaLocalMaxRefinements = 
				{"Mapper.ba_local_max_refinements",2,"colmap mapper param"};

	//Patch match stereo
	Arg<int>	patchMatchStereo_PatchMatchStereoDotMaxImageSize = 
				{"PatchMatchStereo.max_image_size",-1,"colmap patch match stereo param"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotWindowRadius = 
				{"PatchMatchStereo.window_radius",5,"colmap patch match stereo param"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotWindowStep = 
				{"PatchMatchStereo.window_step",1,"colmap patch match stereo param"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotNumSamples = 
				{"PatchMatchStereo.num_samples",15,"colmap patch match stereo param"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotNumIterations = 
				{"PatchMatchStereo.num_iterations",5,"colmap patch match stereo param"};
	Arg<uint>	patchMatchStereo_PatchMatchStereoDotGeomConsistency = 
				{"PatchMatchStereo.geom_consistency",5,"colmap patch match stereo param"};

	//Stereo fusion
	Arg<uint>	stereoFusion_CheckNumImages = 
				{"StereoFusion.check_num_images",50,"colmap stereo fusion param"};
	Arg<uint>	stereoFusion_MaxImageSize = 
				{"StereoFusion.max_image_size",-1,"colmap stereo fusion param"};

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
	if (globalArgs.contains("numGPUs")) {
		parameters.numGPUs(
			userArgs.numGPUs.get());
	}
}

void runColmap(	const std::string& colmapPath,
				const std::string& datasetPath,
				const ColmapParameters& parameters
	){
		
	auto gpusToString = [](uint nbGPUs) {
	
		std::string sGPUs = "0";
		for (uint i = 1; i < nbGPUs; i++) {
			sGPUs.append("," + std::to_string(i));
		}
		return sGPUs;
	};

	const std::string gpusIndices = gpusToString(parameters.numGPUs());
	constexpr size_t colmapCalls = 9;
	const std::array<std::string, colmapCalls> calls{
		"feature_extractor",
		"exhaustive_matcher",
		"mapper",
		"image_undistorter",
		"image_undistorter",
		"patch_match_stereo",
		"stereo_fusion",
		"delaunay_mesher",
		"model_converter"
	};

	const std::array<std::string, colmapCalls> params{
		"--database_path " + datasetPath + "\\colmap\\dataset.db " +
		"--image_path " + datasetPath + "\\images\\ " + "--ImageReader.single_camera 0 " +
		"--ImageReader.camera_model OPENCV " +
		" --SiftExtraction.max_image_size " +
			std::to_string(parameters.siftExtractionImageSize()) +
		" --SiftExtraction.estimate_affine_shape " +
			std::to_string(parameters.siftExtractionEstimateAffineShape()) +
		" --SiftExtraction.domain_size_pooling " +
			std::to_string(parameters.siftExtractionDomainSizePooling()) +
		" --SiftExtraction.max_num_features " +
			std::to_string(parameters.siftExtractionMaxNumFeatures()) +
		" --SiftExtraction.gpu_index=" + gpusIndices,

		"--database_path " + datasetPath + "\\colmap\\dataset.db " +
		" --SiftMatching.guided_matching 1"
		" --ExhaustiveMatching.block_size " +
			std::to_string(parameters.exhaustiveMatcherExhaustiveMatchingBlockSize()) +
		" --SiftMatching.gpu_index =" + gpusIndices,

		"--database_path " + datasetPath + "\\colmap\\dataset.db " +
		"--image_path " + datasetPath + "\\images\\ " + "--output_path " +
		datasetPath + "\\colmap\\sparse\\ "
		" --Mapper.ba_local_max_num_iterations " +
			std::to_string(parameters.mapperMapperDotbaLocalMaxNumIterations()) +
		" --Mapper.ba_global_max_num_iterations " +
			std::to_string(parameters.mapperMapperDotbaGlobalMaxNumIterations()) +
		" --Mapper.ba_global_images_ratio " +
			std::to_string(parameters.mapperMapperDotbaGlobalImagesRatio()) +
		" --Mapper.ba_global_points_ratio " +
			std::to_string(parameters.mapperMapperDotbaGlobalPointsRatio()) +
		" --Mapper.ba_global_max_refinements " +
			std::to_string(parameters.mapperMapperDotbaGlobalMaxRefinements()) +
		" --Mapper.ba_local_max_refinements " +
			std::to_string(parameters.mapperMapperDotbaLocalMaxRefinements()),

		"--image_path " + datasetPath + "\\images\\ " + "--input_path " +
		datasetPath + "\\colmap\\sparse\\0\\ " + "--output_path " +
		datasetPath + "\\colmap\\stereo\\ " + "--output_type COLMAP",

		"--image_path " + datasetPath + "\\images\\ " + "--input_path " +
		datasetPath + "\\colmap\\sparse\\0\\ " + "--output_path " +
		datasetPath + "\\capreal\\undistorted\\ " + "--output_type CMP-MVS",

		"--workspace_path " + datasetPath + "\\colmap\\stereo\\ " + "--workspace_format COLMAP " +
		" --PatchMatchStereo.max_image_size " +
			std::to_string(parameters.patchMatchStereoPatchMatchStereoDotMaxImageSize()) +
		" --PatchMatchStereo.window_radius " +
			std::to_string(parameters.patchMatchStereoPatchMatchStereoDotWindowRadius()) +
		" --PatchMatchStereo.window_step " +
			std::to_string(parameters.patchMatchStereoPatchMatchStereoDotWindowStep()) +
		" --PatchMatchStereo.num_samples " +
			std::to_string(parameters.patchMatchStereoPatchMatchStereoDotNumSamples()) +
		" --PatchMatchStereo.num_iterations " +
			std::to_string(parameters.patchMatchStereoPatchMatchStereoDotNumIterations()) +
		" --PatchMatchStereo.geom_consistency " +
			std::to_string(parameters.patchMatchStereoPatchMatchStereoDotGeomConsistency()) +
		" --PatchMatchStereo.gpu_index =" + gpusIndices,

		"--workspace_path " + datasetPath + "\\colmap\\stereo\\ " + "--workspace_format COLMAP " +
		"--input_type geometric --output_path " + datasetPath + "\\colmap\\stereo\\fused.ply " +
		" --StereoFusion.max_image_size " +
			std::to_string(parameters.stereoFusionMaxImageSize()) +
		" --StereoFusion.check_num_images " +
			std::to_string(parameters.stereoFusionCheckNumImages()),

		"--input_path " + datasetPath + "\\colmap\\stereo\\ " "--output_path " +
		datasetPath + "\\colmap\\stereo\\meshed-delaunay.ply --input_type dense",

		"--input_path " + datasetPath + "\\colmap\\stereo\\sparse " "--output_path " +
		datasetPath + "\\colmap\\stereo\\sparse --output_type TXT"

	};

	for (size_t i = 0; i < colmapCalls; ++i) {
		const std::string command = colmapPath + "\\COLMAP.bat " +
			calls.at(i) + " " + params.at(i);
		const std::string program = calls.at(i) ;
		SIBR_LOG << "Running: " << command << std::endl;
		const int result = boost::process::system(command);
		SIBR_LOG << "Program " << program << " is finished ..." << std::endl << std::endl ;
	}


}

int fixEndLinesMesh(const std::string& datasetPath) {
	const std::string inputMeshPath = datasetPath + "\\colmap\\stereo\\meshed-delaunay.ply";
	
	std::ifstream inputMesh(inputMeshPath , std::ios::binary);
	if (!inputMesh) {
		SIBR_ERR << "Impossible to open " + inputMeshPath;
		return EXIT_FAILURE;
	}
	
	const std::string outputMeshPath  = datasetPath + "\\colmap\\stereo\\unix-meshed-delaunay.ply";
	std::ofstream outputMesh(outputMeshPath, std::ios::binary );
	if (!outputMesh) {
		SIBR_ERR << "Impossible to open " + outputMeshPath;
		return EXIT_FAILURE;
	}

	//-------------HEADER PART--------------//
	std::string line;
	bool endHeaderFound = false;
	while ((!endHeaderFound) && std::getline(inputMesh, line)) {
		const std::string lineWithoutWindowsEndline(line.begin(), line.end() - 1);
		outputMesh<< lineWithoutWindowsEndline << "\n";
		if (lineWithoutWindowsEndline .compare("end_header") == 0) {
			endHeaderFound = true;
		}
	}
	//-------------BODY PART--------------//
	char charToWrite;
	while (inputMesh) {
		inputMesh.get(charToWrite);
		outputMesh << charToWrite;
	}
	inputMesh.close();
	outputMesh.close();
	//--------------------------------------//

	return EXIT_SUCCESS;
}

std::string checkProgram(const std::string& binariesPath, const std::string& programName) {

	std::string programPath;
	if (fileExists(binariesPath + "/" + programName + ".exe")) {
		programPath = binariesPath + "/" + programName + ".exe";
	} 
	else if (fileExists(binariesPath + "/" + programName + "_rdwi.exe")) {
		programPath = binariesPath + "/" + programName + "_rdwi.exe";
	} else if (fileExists(binariesPath + "/" + programName + "_d.exe")) {
		programPath = binariesPath + "/" + programName + "_d.exe";
	}	else {
		SIBR_ERR << "The unwrapMesh program does not exist in: " << binariesPath
			<< " ..." << std::endl << "Did you build and install it ?" << std::endl
			<< "If you do not find the binaries directory, usually it's sibr_basic2\\install\\bin"
			<< std::endl;
	}
	return programPath;
}

void runUnwrapMesh(const std::string& program,const std::string& datasetPath) {
	
	const std::string command = program + " --path "
		+ datasetPath + "\\colmap\\stereo\\unix-meshed-delaunay.ply --output "
		+ datasetPath + "\\capreal\\mesh.ply";
	SIBR_LOG << "Running: " << command << std::endl;
	const int result = boost::process::system(command);
	SIBR_LOG << "Program " << command << " is finished ..." << std::endl << std::endl ;
}

void runColmapToSibr(const std::string& program, const std::string& datasetPath) {
	const std::string command = program + " --path " + datasetPath;
	SIBR_LOG << "Running: " << command << std::endl;
	const int result = boost::process::system(command);
	SIBR_LOG << "Program " << command << " is finished ..." << std::endl << std::endl ;
}

void runTextureMesh(const std::string& program, const std::string& datasetPath) {
	const std::string command = program + " --path " + datasetPath
		+ " --output " + datasetPath + "\\meshes\\texture.png --size 8192 --flood";
	SIBR_LOG << "Running: " << command << std::endl;
	const int result = boost::process::system(command);
	SIBR_LOG << "Program " << command << " is finished ..." << std::endl << std::endl ;
}

int main(const int argc, const char** argv)
{

	CommandLineArgs::parseMainArgs(argc, argv);
	CommandLineArgs globalArgs = CommandLineArgs::getGlobal();
	FullProcessColmapPreprocessArgs myArgs;


	//-----------------PATH ARGUMENT-------------------//
	const std::string pathScene = myArgs.dataset_path;
	if (!directoryExists(pathScene + "/images")) {

		SIBR_ERR << "Your path does not contain an \"images\" directory..." << std::endl
			<< "Please create one and put your images into this" << std::endl;

		return EXIT_FAILURE;
	}


	//-----------------BINARIES ARGUMENT-------------------//
	const std::string unwrapMeshProgram = checkProgram(myArgs.sibrBinariesPath, "unwrapMesh");
	if (unwrapMeshProgram.empty()) {
		return EXIT_FAILURE;
	}

	const std::string textureMeshProgram = checkProgram(myArgs.sibrBinariesPath, "textureMesh");
	if (textureMeshProgram.empty()) {
		return EXIT_FAILURE;
	}

	const std::string colmapToSIBRProgram = checkProgram(myArgs.sibrBinariesPath, "colmapToSIBR");
	if (colmapToSIBRProgram.empty()) {
		return EXIT_FAILURE;
	}

	const std::string colmapTextureMesh = checkProgram(myArgs.sibrBinariesPath, "textureMesh");
	if (colmapToSIBRProgram.empty()) {
		return EXIT_FAILURE;
	}

	//----------------COLMAP PARAMETERS----------------//
	const std::shared_ptr < ColmapParameters::Quality > qualityRecon =
		getUserQuality(globalArgs, myArgs);
	if (!qualityRecon) { 
		return EXIT_FAILURE; 
	}

	ColmapParameters colmapParams(*qualityRecon);
	setPersonalParameters(globalArgs, myArgs, colmapParams);

	const std::vector<std::string> colmapDirs = { 
		"colmap", "capreal", "capreal/undistorted", "colmap/stereo", "colmap/sparse"};
	for (auto dir : colmapDirs) {
		std::cout << dir << std::endl;
		if (!directoryExists(pathScene + "/" + dir.c_str())) {
			makeDirectory(pathScene + "/" + dir.c_str());
		}
	}
	//-------------------------------------------------//


	runColmap(myArgs.colmapPath, myArgs.dataset_path, colmapParams);

	if (fixEndLinesMesh(pathScene) == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}

	runUnwrapMesh(unwrapMeshProgram, myArgs.dataset_path);
		
	const std::string meshPath = myArgs.dataset_path.get() + "\\capreal\\mesh.ply";
	Mesh mesh;
	mesh.load(meshPath);
	mesh.saveToASCIIPLY(meshPath, true, "texture.png");

	runColmapToSibr (colmapToSIBRProgram, myArgs.dataset_path);
	runTextureMesh (textureMeshProgram, myArgs.dataset_path);
	std::vector<std::string> dirs = { "cameras", "images", "meshes"};

	return EXIT_SUCCESS;
}