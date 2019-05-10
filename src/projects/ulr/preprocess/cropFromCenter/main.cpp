#include <fstream>
#include <iostream>
#include <iomanip>
#include <core/graphics/Image.hpp>
#include <core/system/CommandLineArgs.hpp>
#include <boost/filesystem.hpp>

/*
Crop input images from center so they end up with resolution <crop_width> x <crop_height>
if scale down factor is also passed, after the image has been cropped, it will be scaled down by that value
*/
const char* USAGE						= "Usage: cropFromCenter --inputFile <path_to_input_file> --outputPath <path_to_output_folder> --avgResolution <width x height> --cropResolution <width x height> [--scaleDownFactor <alpha> --targetResolution <width x height>] \n";
//const char* USAGE						= "Usage: cropFromCenter --inputFile <path_to_input_file> --outputPath <path_to_output_folder> --avgResolution <width x height> --cropResolution <widht x height> [--scaleDownFactor <alpha> --targetResolution <width x height>] \n";
const char* TAG							= "[cropFromCenter]";
const unsigned PROCESSING_BATCH_SIZE	= 150;
const char* LOG_FILE_NAME				= "cropFromCenter.log";
const char* SCALED_DOWN_SUBFOLDER		= "scaled";
const char* SCALED_DOWN_FILENAME		= "scale_factor.txt";

struct Image {
	std::string	filename;
	unsigned	width;
	unsigned	height;
};

bool getParamas(int argc, const char ** argv,
	std::string & inputFile, boost::filesystem::path & outputPath,
	sibr::Vector2i & avgResolution, sibr::Vector2i & cropResolution, float & scaleDownFactor, sibr::Vector2i & targetResolution
);
void printUsage();
std::vector<std::string> getPathToImgs(const std::string & inputFileName);
bool getIsEmptyFile(const char * filename);
void logExecution(const sibr::Vector2i & resolution, unsigned nrImages, long long elapsedTime, bool wasTransformed);
void writeListImages(const std::string path_to_file, const std::vector<Image> & listOfImages);
void writeScaleFactor(const std::string path_to_file, float scaleFactor);
void writeTargetResolution(const std::string path_to_file, const sibr::Vector2i & targetResolution);
sibr::Vector2i parseResolution(const std::string & param);

int main(const int argc, const char** argv)
{
	// process parameters
	std::string					inputFileName;
	boost::filesystem::path		outputFolder;
	boost::filesystem::path		scaledDownOutputFolder;
	sibr::Vector2i				avgInitialResolution;		// just for statistics and log file
	sibr::Vector2i				cropResolution;
	float						scaleDownFactor = 0.f;
	sibr::Vector2i				targetResolution;

	if (!getParamas(argc, argv, inputFileName, outputFolder, avgInitialResolution, cropResolution, scaleDownFactor, targetResolution)) {
		std::cerr << TAG << " ERROR: wrong parameters.\n";
		printUsage();
		return -1;
	}

	scaledDownOutputFolder = (outputFolder / SCALED_DOWN_SUBFOLDER);

	bool scaleDown = (scaleDownFactor > 0);
	//cv::Size resizedSize (finalResolution[0], cropResolution[1] * ((float)(finalResolution[0]) / cropResolution[0]));
	cv::Size resizedSize(cropResolution[0] * scaleDownFactor, cropResolution[1] * scaleDownFactor);


	if (!boost::filesystem::exists(outputFolder))
	{
		boost::filesystem::create_directory(outputFolder);

		if (scaleDown && !boost::filesystem::exists(scaledDownOutputFolder)) {
			boost::filesystem::create_directory(scaledDownOutputFolder);
		}
	}

	// read input file
	std::vector<std::string> pathToImgs = getPathToImgs(inputFileName);
	std::vector<Image> listOfImages(pathToImgs.size());
	std::vector<Image> listOfImagesScaledDown(scaleDown ? pathToImgs.size() : 0);

	// calculate nr batches
	unsigned nrBatches = static_cast<int>(ceil((float)(pathToImgs.size()) / PROCESSING_BATCH_SIZE));

	std::chrono::time_point <std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	// run batches sequentially
	for (unsigned batchId = 0; batchId < nrBatches; batchId++) {

		unsigned nrItems = (batchId != nrBatches - 1) ? PROCESSING_BATCH_SIZE : ((nrBatches * PROCESSING_BATCH_SIZE != pathToImgs.size()) ? (pathToImgs.size() - (PROCESSING_BATCH_SIZE * batchId)) : PROCESSING_BATCH_SIZE);

		#pragma omp parallel for
		for (int localImgIndex = 0; localImgIndex < nrItems; localImgIndex++) {

			unsigned globalImgIndex = (batchId * PROCESSING_BATCH_SIZE) + localImgIndex;

			// using next code will keep filename in output directory
			boost::filesystem::path boostPath(pathToImgs[globalImgIndex]);
			//std::string outputFileName = (outputFolder / boostPath.filename()).string();

			std::stringstream ss;
			ss << std::setfill('0') << std::setw(8) << globalImgIndex << boostPath.extension().string();
			std::string outputFileName = (outputFolder / ss.str()).string();
			std::string scaledDownOutputFileName = (scaledDownOutputFolder / ss.str()).string();

			cv::Mat img = cv::imread(pathToImgs[globalImgIndex], 1);

			cv::Rect areOfIntererst = cv::Rect((img.cols - cropResolution[0]) / 2, (img.rows - cropResolution[1]) / 2, cropResolution[0], cropResolution[1]);

			cv::Mat croppedImg = img(areOfIntererst);

			cv::imwrite(outputFileName, croppedImg);

			listOfImages[globalImgIndex].filename = ss.str();
			listOfImages[globalImgIndex].width = croppedImg.cols;
			listOfImages[globalImgIndex].height = croppedImg.rows;

			if (scaleDown) {
				cv::Mat resizedImg;
				cv::resize(croppedImg, resizedImg, resizedSize, 0, 0, cv::INTER_LINEAR);

				cv::imwrite(scaledDownOutputFileName, resizedImg);

				listOfImagesScaledDown[globalImgIndex].filename	= ss.str();
				listOfImagesScaledDown[globalImgIndex].width	= resizedImg.cols;
				listOfImagesScaledDown[globalImgIndex].height	= resizedImg.rows;
			}
		}
	}

	end = std::chrono::system_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

	std::cout << TAG << " elapsed time=" << elapsedTime << "s.\n";

	logExecution(avgInitialResolution, pathToImgs.size(), elapsedTime, scaleDown);

	// write list_images.txt
	writeListImages((outputFolder / "list_images.txt").string(), listOfImages);

	// write list_images.txt and scale_factor in scaled down directoy if needed
	if (scaleDown) {
		writeListImages((scaledDownOutputFolder / "list_images.txt").string(), listOfImagesScaledDown);
		writeScaleFactor((scaledDownOutputFolder / SCALED_DOWN_FILENAME).string(), scaleDownFactor);

		if (targetResolution != sibr::Vector2i(0, 0)) {
			writeTargetResolution((scaledDownOutputFolder / "target_resolution.txt").string(), targetResolution);
		}
	}

	return 0;
}

struct CropAppArgs :
	virtual sibr::BasicIBRAppArgs {
	sibr::Arg<bool> whatever = { "whatever", true };
	sibr::Arg<std::string> inputFileArg = { "inputFile", "" };
	sibr::Arg<std::string> outputFolderArg = { "outputPath", "" };
	sibr::Arg<sibr::Vector2i> avgResolutionArg = { "avgResolution", {0, 0} };
	sibr::Arg<sibr::Vector2i> cropResolutionArg = { "cropResolution",{ 0, 0 } };
	sibr::Arg<float> scaleDownFactorArg = { "scaleDownFactor", 0.0f };
	sibr::Arg<sibr::Vector2i> targetResolutionArg = { "targetResolution",{ 0, 0 } };
};

bool getParamas(int argc, const char ** argv,
	std::string & inputFile, boost::filesystem::path & outputPath,
	sibr::Vector2i & avgResolution, sibr::Vector2i & cropResolution, float & scaleDownFactor, sibr::Vector2i & targetResolution)
{

	sibr::CommandLineArgs::parseMainArgs(argc, argv);
	CropAppArgs myArgs;

	inputFile = myArgs.inputFileArg;

	std::string outputFolder = myArgs.outputFolderArg;
	outputPath = outputFolder;

	avgResolution = myArgs.avgResolutionArg;

	cropResolution = myArgs.cropResolutionArg;

	// optional parameters
	if (myArgs.scaleDownFactorArg != 0.0f) {
		scaleDownFactor = myArgs.scaleDownFactorArg;
	}

	if (myArgs.targetResolutionArg.get() != sibr::Vector2i(0, 0)) {
		targetResolution = myArgs.targetResolutionArg;
	}


	if (inputFile.empty() || outputFolder.empty() || avgResolution == sibr::Vector2i(0, 0) || cropResolution == sibr::Vector2i(0, 0)) {
		return false;
	}

	return true;
}

void printUsage()
{
	std::cout << USAGE << std::endl;
}

std::vector<std::string> getPathToImgs(const std::string & inputFileName)
{
	std::ifstream inputFile(inputFileName);
	std::string line;
	std::vector<std::string> pathToImgs;
	while (getline(inputFile, line)) {
		std::stringstream ss(line);
		std::string path;
		unsigned width, height;
		ss >> path >> width >> height;
		pathToImgs.push_back(path);
	}
	inputFile.close();
	return pathToImgs;
}

bool getIsEmptyFile(const char * filename)
{
	std::ifstream testFile(filename);

	bool result = !testFile.good();

	testFile.close();

	return result;
}

void logExecution(const sibr::Vector2i & originalResolution, unsigned nrImages, long long elapsedTime, bool wasTransformed)
{
	// check if file exists
	bool isEmptyFile = getIsEmptyFile(LOG_FILE_NAME);
	std::ofstream outputFile(LOG_FILE_NAME, std::ios::app);

	if (isEmptyFile) {
		outputFile << "date\t\t\tresolution\tnrImgs\telapsedTime\twas transformed?\n";
	}

	time_t now = time(0);
	tm *ltm = localtime(&now);

	std::stringstream dateSS;
	dateSS << "[" << 1900 + ltm->tm_year << "/" << 1 + ltm->tm_mon << "/" << ltm->tm_mday << "] "
		<< ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec;

	outputFile << dateSS.str() << "\t" << originalResolution[0] << "x" << originalResolution[1] << "\t\t" << nrImages << "\t" << elapsedTime << "\t" << wasTransformed << "\n";

	outputFile.close();
}

void writeListImages(const std::string path_to_file, const std::vector<Image> & listOfImages)
{
	std::ofstream outputFile(path_to_file);

	for (unsigned i = 0; i < listOfImages.size(); i++) {
		outputFile << listOfImages[i].filename << " " << listOfImages[i].width << " " << listOfImages[i].height << "\n";
	}

	outputFile.close();
}

sibr::Vector2i parseResolution(const std::string & param)
{
	size_t delimiterPos = param.find('x');
	std::string widthStr = param.substr(0, delimiterPos);
	std::string heightStr = param.substr(delimiterPos + 1);
	return sibr::Vector2i(std::stoi(widthStr), std::stoi(heightStr));
}

void writeScaleFactor(const std::string path_to_file, float scaleFactor)
{
	std::ofstream outputFile(path_to_file);

	outputFile << scaleFactor << "\n";

	outputFile.close();
}

void writeTargetResolution(const std::string path_to_file, const sibr::Vector2i & targetResolution)
{
	std::ofstream outputFile(path_to_file);

	outputFile << targetResolution[0] << " " << targetResolution[1] << "\n";

	outputFile.close();
}