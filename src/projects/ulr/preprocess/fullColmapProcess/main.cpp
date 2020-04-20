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
	RequiredArg<std::string>	colmapPath = { "colmapPath","colmap path directory which contains COLMAP.bat (or colmap.bin if you use remoteUnix option" };
	RequiredArg<std::string>	sibrBinariesPath = { "sibrBinariesPath","binaries directory of SIBR" };
	Arg<std::string>			quality = 
								{ "quality","","quality of the reconstruction" };
	Arg<uint>					numGPUs = { "numGPUs",1,"Number of GPUs" };
	Arg<std::string>			remoteUnix = { "remoteUnix","","ssh account, example: user@nef-devel.inria.fr" };
	Arg<std::string>			colmapWorkingDir = { "colmapWorkingDir","","colmap working directory in your Unix system" };
	Arg<std::string>			clusterGPU = { "clusterGPU","","GPU number : exemple 12" };

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

void sendImages(
	const std::string& datasetPath,
	const std::string& colmapWorkingDir,
	const std::string& sshAccount) {
	
	boost::process::system("scp -r " + datasetPath + "\\images "
		+ sshAccount + ":" + colmapWorkingDir );
}

void getProject(
	const std::string& datasetPath,
	const std::string& colmapWorkingDir,
	const std::string& sshAccount) {
	
	boost::process::system("scp -r " +sshAccount + ":" + colmapWorkingDir + " " + 
		datasetPath );
}

void runColmap(const std::string& colmapProgramPath,
	const std::string& colmapWorkingDir,
	const ColmapParameters& parameters,
	bool remotly = false,
	const std::string& sshAccount = "",
	size_t gpuNodeNum = 9
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
		"--database_path " + colmapWorkingDir + "\\colmap\\dataset.db " +
		"--image_path " + colmapWorkingDir + "\\images\\ " + "--ImageReader.single_camera 0 " +
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

		"--database_path " + colmapWorkingDir + "\\colmap\\dataset.db " +
		" --SiftMatching.guided_matching 1"
		" --ExhaustiveMatching.block_size " +
			std::to_string(parameters.exhaustiveMatcherExhaustiveMatchingBlockSize()) +
		" --SiftMatching.gpu_index =" + gpusIndices,

		"--database_path " + colmapWorkingDir + "\\colmap\\dataset.db " +
		"--image_path " + colmapWorkingDir + "\\images\\ " + "--output_path " +
		colmapWorkingDir + "\\colmap\\sparse\\ "
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

		"--image_path " + colmapWorkingDir + "\\images\\ " + "--input_path " +
		colmapWorkingDir + "\\colmap\\sparse\\0\\ " + "--output_path " +
		colmapWorkingDir + "\\colmap\\stereo\\ " + "--output_type COLMAP",

		"--image_path " + colmapWorkingDir + "\\images\\ " + "--input_path " +
		colmapWorkingDir + "\\colmap\\sparse\\0\\ " + "--output_path " +
		colmapWorkingDir + "\\capreal\\undistorted\\ " + "--output_type CMP-MVS",

		"--workspace_path " + colmapWorkingDir + "\\colmap\\stereo\\ " + "--workspace_format COLMAP " +
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

		"--workspace_path " + colmapWorkingDir + "\\colmap\\stereo\\ " + "--workspace_format COLMAP " +
		"--input_type geometric --output_path " + colmapWorkingDir + "\\colmap\\stereo\\fused.ply " +
		" --StereoFusion.max_image_size " +
			std::to_string(parameters.stereoFusionMaxImageSize()) +
		" --StereoFusion.check_num_images " +
			std::to_string(parameters.stereoFusionCheckNumImages()),

		"--input_path " + colmapWorkingDir + "\\colmap\\stereo\\ " "--output_path " +
		colmapWorkingDir + "\\colmap\\stereo\\meshed-delaunay.ply --input_type dense",

		"--input_path " + colmapWorkingDir + "\\colmap\\stereo\\sparse " "--output_path " +
		colmapWorkingDir + "\\colmap\\stereo\\sparse --output_type TXT"

	};

	std::string unixListAllCommands ="touch started.txt";
	for (size_t i = 0; i < colmapCalls; ++i) {
		const std::string program = calls.at(i) ;
		std::string command;
		if (!remotly) {
			//Windows
			command = colmapProgramPath + " " + program + " " + params.at(i);
			SIBR_LOG << "Running: " << command << std::endl;
			const int result = boost::process::system(command);
			SIBR_LOG << "Program " << program << " is finished ..." << std::endl << std::endl;
		}
		else {
			//Unix remote, we prepare the steps
			unixListAllCommands += colmapProgramPath + " " + program + " " + params.at(i) + "\n";
			unixListAllCommands += "touch " + colmapWorkingDir + "/" + program + ".txt\n";
		}
	}
	if (remotly) {
		//We create the script that the node will execute
		const std::string scriptCommands = "ssh -t " + sshAccount + " \"cd " + 
			colmapWorkingDir + "; cat " + unixListAllCommands + " > colmapScript.sh; chmod 755 colmapScript.sh;"
			+ "oarsub - p \"host='nefgpu'" + std::to_string(gpuNodeNum)+".inria.fr'\" -l /nodes=1,walltime=01:00:00 " +
				colmapWorkingDir +"/colmapScript.sh";


		SIBR_LOG << "Creating script for the node and run it : " << std::endl << scriptCommands<< std::endl;
			const int result = boost::process::system(scriptCommands);
			SIBR_LOG << "The request is done  ... Waiting the answers..." << std::endl << std::endl;
	}

}

void waitSteps(const std::string& colmapWorkingDir, const std::string& sshAccount) {
	
	constexpr size_t nbSteps = 10;
	const std::array<std::string, nbSteps> steps{
		"starting",
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
	for (const std::string& step : steps) {
		bool stepFinished = false;
		while (!stepFinished) {
			if (!fileExists(sshAccount + ":" + colmapWorkingDir + "/" + step + ".txt")) {
				std::cout << "Waiting for " + step + " step... ";
				Sleep(5000);
			}
		}
		std::cout << "Step " + step + " is DONE !";
	}
	std::cout << "Every steps are completed !" << std::endl;
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
	else if (fileExists(binariesPath + "/" + programName + "_rwdi.exe")) {
		programPath = binariesPath + "/" + programName + "_rwdi.exe";
	} else if (fileExists(binariesPath + "/" + programName + "_d.exe")) {
		programPath = binariesPath + "/" + programName + "_d.exe";
	}	else {
		SIBR_ERR << "The " << programName << " program does not exist in: " << binariesPath
			<< " ..." << std::endl << "Did you build and install it ?" << std::endl
			<< "If you do not find the binaries directory, usually it's sibr_basic2\\install\\bin"
			<< std::endl;
	}
	return programPath;
}

void runUnwrapMesh(const std::string& program,const std::string& datasetPath, bool remotly = false) {
	
	std::string meshToUnwrap;
	if (remotly) {
		//Meshed-delaunay.ply was created by colmap on Unix, we can directly use it
		meshToUnwrap = "meshed-delaunay.ply";
	}
	else {
		//On Windows, we need to use the converted mesh
		meshToUnwrap = "unix-meshed-delaunay.ply";
	}
	const std::string command = program + " --path "
		+ datasetPath + "\\colmap\\stereo\\"+ meshToUnwrap + " --output "
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

	//------------------------CHECKING USER'S PARAMETERS-------------------------//

	//-----------------REMOTE UNIX ARGUMENT-------------------//
	if (globalArgs.contains("remoteUnix")) {
		if (!globalArgs.contains("clusterGPU")) {
			SIBR_ERR << "Your specified the remoteUnix option but you did not specify the clusterGPU option.." << std::endl
				<< "Please specify it with a free GPU node on the cluster (example --clusterGPU 12)" << std::endl;
			return EXIT_FAILURE;
		}
		if (!globalArgs.contains("colmapWorkingDir")) {
			SIBR_ERR << "Your specified the remoteUnix option but you did not specify the colmapWorkingDir path option.." << std::endl
				<< "Please specify this path on your remote system" << std::endl;
			return EXIT_FAILURE;
		}
	}

	//-----------------PATH ARGUMENT-------------------//
	const bool runLocally = myArgs.remoteUnix.get().empty();
	const std::string scenePath = myArgs.dataset_path.get();

	if (!directoryExists(scenePath + "/images")) {

		SIBR_ERR << "Your path does not contain an \"images\" directory..." << std::endl
			<< "Please create one and put your images into this" << std::endl;

		return EXIT_FAILURE;
	}

	std::string workingPath;
	if (runLocally) {
		workingPath = myArgs.dataset_path.get();
	} else {
		workingPath = myArgs.remoteUnix.get() + ":" + myArgs.colmapWorkingDir.get();
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
	std::string colmapProgram;
	if (runLocally) { //Windows version
		colmapProgram = myArgs.colmapPath.get() + "\\COLMAP.bat";
		if (!fileExists(colmapProgram)) {
			SIBR_ERR << "Your path does not contain a COLMAP.bat program..." << std::endl;
			return EXIT_FAILURE;
		}

	} else { //Unix version
		colmapProgram = myArgs.colmapPath.get() + "/colmap";
		const std::string command = "ssh " + myArgs.remoteUnix.get() + " test -f " + colmapProgram;
		SIBR_LOG << "Running: " << command << std::endl;
		const int result = boost::process::system(command);
		SIBR_LOG << "ssh checking file is finished ..." << std::endl;
		
		if (result == EXIT_FAILURE) {
			SIBR_ERR << "Your remote path does not contain a colmap program..." << std::endl;
			return EXIT_FAILURE;
		}
		
	}

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
		if (!directoryExists(workingPath + "/" + dir.c_str())) {
			makeDirectory(workingPath + "/" + dir.c_str());
		}
	}
	//----------------------------CHECKING FINISHED------------------------------//

	
	if (runLocally) {
	//Windows
	runColmap(colmapProgram, myArgs.dataset_path, colmapParams);
	} 
	else {
	//REMOTE UNIX
	runColmap(colmapProgram, myArgs.colmapWorkingDir , colmapParams, true, myArgs.remoteUnix.get());
	waitSteps(myArgs.colmapWorkingDir, myArgs.remoteUnix.get());
	getProject(myArgs.dataset_path.get(), myArgs.colmapWorkingDir.get(), myArgs.remoteUnix.get());
	}


	if (runLocally) {
		//We fix the end of line only on Windows
		if (fixEndLinesMesh(scenePath) == EXIT_FAILURE) {
			return EXIT_FAILURE;
		}
	}

	runUnwrapMesh(unwrapMeshProgram, myArgs.dataset_path);


		
	const std::string meshPath = myArgs.dataset_path.get() + "\\capreal\\mesh.ply";
	Mesh mesh;
	mesh.load(meshPath);
	mesh.saveToASCIIPLY(meshPath, true, "texture.png");

	runColmapToSibr (colmapToSIBRProgram, myArgs.dataset_path);
	runTextureMesh (textureMeshProgram, myArgs.dataset_path);
		
	std::cout << "END OF FULL COLMAP PREPROCESS PROGRAMM" << std::endl;
	return EXIT_SUCCESS;
}