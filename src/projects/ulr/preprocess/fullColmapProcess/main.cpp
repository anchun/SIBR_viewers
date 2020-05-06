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
	Arg<uint>					numGPUs = { "numGPUs",2,"Number of GPUs" };
	Arg<std::string>			remoteUnix = { "remoteUnix","","ssh account, example: user@nef-devel.inria.fr" };
	Arg<std::string>			colmapWorkingDir = { "colmapWorkingDir","","colmap working directory in your Unix system" };
	Arg<std::string>			clusterGPU = { "clusterGPU","any","GPU number, example : \"clusterGPU 12\" , \"clusterGPU any\"" };

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
}

std::string convertWindowsToLinuxPath(const std::string& winPath) {
	std::string linuxPath;
	for (const char& c : winPath) {
		if (c == '\\') {
			linuxPath.push_back('/');
		}
		else linuxPath.push_back(c);
	}
	return linuxPath;
}

int sendImages(
	const std::string& datasetPath,
	const std::string& colmapWorkingDir,
	const std::string& sshAccount) {
	const std::string command = "scp -r " + convertWindowsToLinuxPath 
		(datasetPath)+ "/images " + sshAccount + ":" + colmapWorkingDir;

	SIBR_LOG << "Sending images ... " << std::endl;
	const int result = boost::process::system(command);
	SIBR_LOG << "Running: " << command << std::endl;

	if (result == EXIT_FAILURE) {
		SIBR_ERR << "Impossible to send the images dir to your remote dir ... " << std::endl
			<< "Are you sure that your remote working dir exists ?" << std::endl;
	}
	return result;
}

void getProject(
	const std::string& datasetPath,
	const std::string& colmapWorkingDir,
	const std::string& sshAccount) {
	
	boost::process::system("scp -r " +sshAccount + ":" + colmapWorkingDir + "/colmap " + 
		datasetPath );
	boost::process::system("scp -r " +sshAccount + ":" + colmapWorkingDir + "/capreal " + 
		datasetPath );
}

void runColmap(const std::string& colmapProgramPath,
	const std::string& datasetPath,
	const std::string& colmapWorkingDir,
	const ColmapParameters& parameters,
	const std::string& sshAccount = "",
	const std::string& displayOption = " ",
	std::string gpuNodeNum = "any",
	unsigned int numGPUs = 2
	){
	if (gpuNodeNum.size() == 1) {
	//We add a '0' if the number has only one char
		gpuNodeNum = "0" + gpuNodeNum;
	}
	const bool remotely = (!sshAccount.empty());
		
	auto gpusToString = [](uint nbGPUs) {
	
		std::string sGPUs = "0";
		for (uint i = 1; i < nbGPUs; i++) {
			sGPUs.append("," + std::to_string(i));
		}
		return sGPUs;
	};

	const std::string gpusIndices = gpusToString(numGPUs);
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

	std::string dirStr;
	if (remotely) { //UNIX
		dirStr = "/";
	}
	else { //WINDOW
		dirStr = "\\";
	}
	const std::array<std::string, colmapCalls> params{
		"--database_path " + colmapWorkingDir + dirStr + "colmap" + dirStr + "dataset.db " +
		"--image_path " + colmapWorkingDir + dirStr + "images" + dirStr + " --ImageReader.single_camera 0 " +
		"--ImageReader.camera_model OPENCV " +
		" --SiftExtraction.max_image_size " +
			std::to_string(parameters.siftExtractionImageSize()) +
		" --SiftExtraction.estimate_affine_shape " +
			std::to_string(parameters.siftExtractionEstimateAffineShape()) +
		" --SiftExtraction.domain_size_pooling " +
			std::to_string(parameters.siftExtractionDomainSizePooling()) +
		" --SiftExtraction.max_num_features " +
			std::to_string(parameters.siftExtractionMaxNumFeatures()) +
		" --SiftExtraction.gpu_index "+ gpusIndices,

		"--database_path " + colmapWorkingDir + dirStr + "colmap" + dirStr + "dataset.db " +
		" --SiftMatching.guided_matching 1"
		" --ExhaustiveMatching.block_size " +
			std::to_string(parameters.exhaustiveMatcherExhaustiveMatchingBlockSize()) +
		" --SiftMatching.gpu_index " + gpusIndices,

		"--database_path " + colmapWorkingDir + dirStr+ "colmap"+ dirStr+ "dataset.db " +
		"--image_path " + colmapWorkingDir + dirStr + "images"+ dirStr + " --output_path " +
		colmapWorkingDir + dirStr + "colmap" + dirStr +  "sparse" + dirStr +
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

		"--image_path " + colmapWorkingDir + dirStr + "images" + dirStr + " --input_path " +
		colmapWorkingDir + dirStr + "colmap" + dirStr + "sparse"+dirStr+"0"+ dirStr  + " --output_path " +
		colmapWorkingDir + dirStr + "colmap" + dirStr + "stereo" + dirStr  + " --output_type COLMAP",

		"--image_path " + colmapWorkingDir + dirStr + "images" + dirStr  + " --input_path " +
		colmapWorkingDir + dirStr + "colmap" + dirStr + "sparse"+dirStr+"0"+ dirStr + " --output_path " +
		colmapWorkingDir + dirStr + "capreal" +  dirStr +"undistorted" + dirStr + " --output_type CMP-MVS",

		"--workspace_path " + colmapWorkingDir + dirStr + "colmap" + dirStr + "stereo" + " --workspace_format COLMAP " +
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
		" --PatchMatchStereo.gpu_index " + gpusIndices,

		"--workspace_path " + colmapWorkingDir + dirStr + "colmap" + dirStr + "stereo" + dirStr + " --workspace_format COLMAP " +
		"--input_type geometric --output_path " + colmapWorkingDir + dirStr + "colmap" + dirStr + "stereo" + dirStr + "fused.ply " +
		" --StereoFusion.max_image_size " +
			std::to_string(parameters.stereoFusionMaxImageSize()) +
		" --StereoFusion.check_num_images " +
			std::to_string(parameters.stereoFusionCheckNumImages()),

		"--input_path " + colmapWorkingDir + dirStr + "colmap" + dirStr + "stereo" + dirStr + " --output_path " +
		colmapWorkingDir + dirStr + "colmap" + dirStr + "stereo" + dirStr + "meshed-delaunay.ply --input_type dense",

		"--input_path " + colmapWorkingDir + dirStr + "colmap" + dirStr + "stereo" + dirStr + "sparse " + dirStr + " --output_path " +
		colmapWorkingDir + dirStr + "colmap" + dirStr + "stereo" + dirStr + "sparse" + dirStr + " --output_type TXT"

	};

	std::string unixListAllCommands;
	unixListAllCommands.append("module load cuda/10.0 gcc/7.3.0\nCOLMAP_PATH=/data/graphdeco/share/colmap\n");
	unixListAllCommands.append("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$COLMAP_PATH/lib/colmap:$COLMAP_PATH/deps/ceres/lib64:$COLMAP_PATH/deps/cgal/lib64:$COLMAP_PATH/deps/freeimage/lib\n");	
	unixListAllCommands.append("cd "+ colmapWorkingDir + "\ntouch started.txt\n");
	for (size_t i = 0; i < colmapCalls; ++i) {
		const std::string program = calls.at(i) ;
		std::string command;
		if (!remotely) {
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
	if (remotely) {
		//We create the script that the node will execute
		const std::string scriptToSend = convertWindowsToLinuxPath (datasetPath) + 
			"/colmapScript.sh";
		std::ofstream scriptFile (scriptToSend, std::ios::binary);
		scriptFile << unixListAllCommands;
		scriptFile.close();
		
		SIBR_LOG << "Sending script file..." << std::endl << std::endl;
		const std::string command = "scp " + scriptToSend + " " + sshAccount + ":" + colmapWorkingDir;
		const int result = boost::process::system(command);
		SIBR_LOG << "Running: " << command << std::endl;

		SIBR_LOG << "The request is done  ... Waiting the answers..." << std::endl << std::endl;
		std::string runScript = "ssh" + displayOption + sshAccount + " \"cd " +
			colmapWorkingDir + ";chmod 777 " + colmapWorkingDir +
			"/colmapScript.sh;oarsub -p \\\"";
		if (gpuNodeNum.compare("any") != 0) {
			runScript += "host='nefgpu" + gpuNodeNum + ".inria.fr' and ";
		}

		runScript += "gpu='YES' and gpucapability>='5.0'\\\" -l /nodes=1/gpunum="
			+ std::to_string(numGPUs) + ",walltime=01:00:00 " +
			colmapWorkingDir + "/colmapScript.sh\"";
		const int resultRunScript = boost::process::system(runScript);
		if (resultRunScript == EXIT_FAILURE) {
			SIBR_LOG << "ERROR" << std::endl;
		}
		else {
		}
		SIBR_LOG << "Running: " << runScript << std::endl;


	}

}

std::string getDisplayOption( 
	const std::string& colmapWorkingDir,
	const std::string& sshAccount) {

	//On the Windows terminal, if we do not use the -t option for ssh, the Logs on the console have bugs 
	//with the endline character, so if the option is available, we use it. If the option is not
	//available, we don't use it

	//We are sure that this directory exists, but we test if the -t option are available
	const std::string command = "ssh -t " + sshAccount + " test -d " + colmapWorkingDir;

	const int result = boost::process::system(command);

	std::string displayOption;
	if (result == EXIT_SUCCESS) {
		displayOption = " -t "; //available
	} else 
		displayOption = " "; //not available

	return displayOption;
}

void waitSteps(const std::string& colmapWorkingDir, const std::string& sshAccount,
				const std::string& displayOption = " ") {
	
	constexpr size_t nbSteps = 10;
	const std::array<std::string, nbSteps> steps{
		"started",
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
			//const std::string command = "ssh -t " + sshAccount + " \" ls " + colmapWorkingDir + "/" + step + ".txt\"";
			const std::string command = "ssh" + displayOption + sshAccount + " test -f " + colmapWorkingDir + "/" + step + ".txt\"";
			//SIBR_LOG << "Running: " << command << std::endl;
			const int result = boost::process::system(command);
			//SIBR_LOG << "ssh checking file is finished ..." << std::endl;

			if (result == EXIT_SUCCESS) {
				stepFinished = true;
			}
			else {
				std::cout << "Waiting for \"" + step + "\" step... " << std::endl;
				Sleep(5000);
			}
		}
		std::cout << std::endl << "Step " + step + " is DONE !" << std::endl << std::endl;
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


void makeDirectories(const std::string& workingPath, const std::string& sshAccount = "", 
						const std::string& displayOption = " ") {
	const bool remotely = (!sshAccount.empty());
	const std::vector<std::string> colmapDirs = { 
		"colmap", "capreal", "capreal/undistorted", "colmap/stereo", "colmap/sparse"};
	if (remotely) {
		for (const std::string& dir : colmapDirs) {
			std::string strDisplayOption;

			const std::string makeDirCommand = "ssh" + displayOption + sshAccount + " \"cd " +
				workingPath + "; mkdir " + dir + "\"";

			SIBR_LOG << "Creating " << dir << " remotely : " << std::endl
				<< makeDirCommand << std::endl;
			const int result = boost::process::system(makeDirCommand);
		}
	}
	else {
		for (const std::string& dir : colmapDirs) {
			std::cout << dir << std::endl;
			if (!directoryExists(workingPath + "/" + dir.c_str())) {
				makeDirectory(workingPath + "/" + dir.c_str());
			}
		}
	}
}

std::string checkColmap(const std::string& colmapPath, bool runLocally,
				const std::string& sshAccount) {

	std::string colmapProgram;
	if (runLocally) { //Windows version
		colmapProgram = colmapPath + "\\COLMAP.bat";
		if (!fileExists(colmapProgram)) {
			SIBR_ERR << "Your path does not contain a COLMAP.bat program..." << std::endl;
			return "";
		}

	} else { //Unix version
		colmapProgram = colmapPath + "/colmap";
	}

	return colmapProgram;
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
			<< "If you do not find the binaries directory, usually it's sibr_core\\install\\bin"
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
	SIBR_LOG << "If the Generating atlas seems blocked at 98% or 99% that is OK do not worry" << std::endl;
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

void printExample() {
    SIBR_LOG << "OPTIONS EXAMPLE TO HELP YOU :" << std::endl
        << "LOCAL VERSION" << std::endl
        << "--path E:\\USERNAME\\testData\\colmap\\testcluster              The path which contain the dataset" << std::endl
        << "--sibrBinariesPath E:\\USERNAME\\dev\\sibr_basic2\\install\\bin The path where the program looks for the binaries" << std::endl
        << "--colmapPath D:\\colmap                                         The path where the Colmap program is present (COLMAP.bat on Windows, colmap on UNIX)" << std::endl
        << "--quality medium                                                Optional option. It's the pre-defined quality (low,medium,high,extreme)" << std::endl
        << std::endl
        << "REMOTE VERSION" << std::endl
        << "--path E:\\YOU\\testData\\colmap\\testcluster                   The path which contain the dataset" << std::endl
        << "--sibrBinariesPath E:\\YOU\\dev\\sibr_basic2\\install\\bin      The path where the program looks for the binaries"  << std::endl
        << "--remoteUnix YOURACCOUNT@nef-devel.inria.fr                     Your SSH account" << std::endl
        << "--colmapPath /data/graphdeco/share/colmap/bin/                  The path where the Colmap program is present (COLMAP.bat on Windows, colmap on UNIX)" << std::endl
        << "--colmapWorkingDir /data/graphdeco/user/YOU/colmapTests/test    A directory on the Cluster where Colmap will write the results" << std::endl
        << "--clusterGPU 34                                                 Optional option. It's the number of the node. If you do not specify it, the Cluster will decide" << std::endl
        << "--numGPUs                                                       Optional option. It's the number of the GPUs present in the Node" << std::endl
        << "--quality medium                                                Optional option. It's the pre-defined quality (low,medium,high,extreme)" << std::endl
        << std::endl;
}

int main(const int argc, const char** argv)
{
	printExample();
	CommandLineArgs::parseMainArgs(argc, argv);
	CommandLineArgs globalArgs = CommandLineArgs::getGlobal();
	FullProcessColmapPreprocessArgs myArgs;

	//------------------------CHECKING USER'S PARAMETERS-------------------------//

	//-----------------REMOTE UNIX ARGUMENT-------------------//
	if (globalArgs.contains("remoteUnix")) {
		if (!globalArgs.contains("colmapWorkingDir")) {
			SIBR_ERR << "Your specified the remoteUnix option but you did not specify the colmapWorkingDir path option.." << std::endl
				<< "Please specify this path on your remote system" << std::endl;
			printExample();
			return EXIT_FAILURE;
		}
	}

	//-----------------PATH ARGUMENT-------------------//
	const bool runLocally = myArgs.remoteUnix.get().empty();
	const std::string scenePath = myArgs.dataset_path.get();

	if (!directoryExists(scenePath + "/images")) {

		SIBR_ERR << "Your path does not contain an \"images\" directory..." << std::endl
			<< "Please create one and put your images into this" << std::endl;

		printExample();
		return EXIT_FAILURE;
	}

	std::string workingPath;
	if (runLocally) {
		workingPath = myArgs.dataset_path.get();
	} else {
		workingPath = myArgs.colmapWorkingDir.get();
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
	const std::string colmapProgram = 
		checkColmap(myArgs.colmapPath.get(),runLocally,myArgs.remoteUnix.get());
	if (colmapProgram.empty()) {
		printExample();
		return EXIT_FAILURE;
	}

	const std::shared_ptr < ColmapParameters::Quality > qualityRecon =
		getUserQuality(globalArgs, myArgs);
	if (!qualityRecon) { 
		printExample();
		return EXIT_FAILURE; 
	}
	//-------------------------------------------------//

	if (!runLocally) {
		if (sendImages(myArgs.dataset_path.get(), myArgs.colmapWorkingDir.get(),
			myArgs.remoteUnix.get()) == EXIT_FAILURE) {
			printExample();
			return EXIT_FAILURE;
		}
	}

	//----------------------------CHECKING FINISHED------------------------------//

	const std::string displayOption = getDisplayOption(myArgs.colmapWorkingDir.get(),
														myArgs.remoteUnix.get());
	makeDirectories(workingPath, myArgs.remoteUnix.get(), displayOption);

	ColmapParameters colmapParams(*qualityRecon);
	setPersonalParameters(globalArgs, myArgs, colmapParams);
	
	if (runLocally) {
	//Windows
	runColmap(colmapProgram,myArgs.dataset_path, myArgs.dataset_path, colmapParams);
	} 
	else {
	//REMOTE UNIX
	runColmap(colmapProgram, myArgs.dataset_path,myArgs.colmapWorkingDir , 
		colmapParams, myArgs.remoteUnix.get(), displayOption, myArgs.clusterGPU.get(), myArgs.numGPUs.get());
	waitSteps(myArgs.colmapWorkingDir, myArgs.remoteUnix.get(), displayOption);
	getProject(myArgs.dataset_path.get(), myArgs.colmapWorkingDir.get(), myArgs.remoteUnix.get());
	}


	if (runLocally) {
		//We fix the end of line only on Windows
		if (fixEndLinesMesh(scenePath) == EXIT_FAILURE) {
			return EXIT_FAILURE;
		}
	}

	runUnwrapMesh(unwrapMeshProgram, myArgs.dataset_path, !runLocally);

	const std::string meshPath = myArgs.dataset_path.get() + "\\capreal\\mesh.ply";
	Mesh mesh;
	mesh.load(meshPath);
	mesh.saveToASCIIPLY(meshPath, true, "texture.png");

	runColmapToSibr (colmapToSIBRProgram, myArgs.dataset_path);
	runTextureMesh (textureMeshProgram, myArgs.dataset_path);
		
	std::cout << "END OF FULL COLMAP PREPROCESS PROGRAM" << std::endl;
	return EXIT_SUCCESS;
}