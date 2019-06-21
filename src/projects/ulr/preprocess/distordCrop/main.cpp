#include <core/graphics/Image.hpp>
#include <core/system/Vector.hpp>
#include <core/system/CommandLineArgs.hpp>
#include <core/system/Array2d.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 

#include<fstream>
#include <queue>

typedef boost::filesystem::path Path;
using namespace boost::filesystem;

int threshold_black_color = 10; //10
int thinest_bounding_box_size = 5;
int threshold_bounding_box_size = 500;
float threshold_ratio_bounding_box_size = 0.2;

const int PROCESSING_BATCH_SIZE = 150;	// process PROCESSING_BATCH_SIZE images together

sibr::Vector3i backgroundColor = sibr::Vector3i(0, 0, 0);

/*
	if input image resolution is too different from the avg, it will be discarded automatically at the beginning
*/
float resolutionThreshold = 0.15f;

/*
	tolerance factor is used to allow somehow some black borders in the final images.
	if tolerance factor is zero, then all black borders are remove.
	if tolerance factor is one, then the image keeps its original resolution
*/
float toleranceFactor = 0.0f;

bool debug_viz = false;

struct DistordCropAppArgs :
	virtual sibr::BasicIBRAppArgs {
	sibr::Arg<int> black_threshold = { "black", threshold_black_color };
	sibr::Arg<int> minSizeThresholdArg = { "min", threshold_bounding_box_size};
	sibr::Arg<float> minRatioThresholdArg = { "ratio", threshold_ratio_bounding_box_size };
	sibr::Arg<float> toleranceArg = { "tolerance", toleranceFactor };
	sibr::Arg<bool> vizArg = { "debug" };
	sibr::SwitchArg modeArg = { "modesame", true };
	sibr::Arg<int> avgWidthArg = { "avg_width", 0 };
	sibr::Arg<int> avgHeightArg = { "avg_height", 0 };
};


/*
utility program that determines a new resolution taking into account that some input images have black borders added by reality capture.
the second output of the program [optional] is a excludeImages.txt file containing the id of the images that didn't pass the threshold test
(they would have to be cropped to much). current pipeline (IBR_recons_RC.py) doesn't used that file properly.

we might need to call process_cam_selection manually passing as argument the excludeImages.txt in order to actually remove the cameras that
didn't pass the threshold test

update: reality capture (using the 'fit' option when exporting bundle) sometimes produces datasets that have images not only with black borders
but also with a completely different resolution. We need to take into account those datasets too.
*/

struct Bounds
{
	Bounds() {}
	Bounds(const sibr::ImageRGB & img) {
		xMax = (int)img.w() - 1;
		xMin = 0;
		yMax = (int)img.h() - 1;
		yMin = 0;
		xRatio = 1.0f;
		yRatio = 1.0f;
	}

	Bounds(const sibr::Vector2i & res) {
		xMax = res.x() - 1;
		xMin = 0;
		yMax = res.y() - 1;
		yMin = 0;
		xRatio = 1.0f;
		yRatio = 1.0f;
	}

	std::string display() const {
		std::stringstream s;
		s << "[" << xMin << ", " << xMax << "]x[" << yMin << ", " << yMax << "]";
		return s.str();
	}

	int xMax;
	int xMin;
	int yMax;
	int yMin;
	int width;
	int height;

	float xRatio;
	float yRatio;
};

bool isBlack(const sibr::Vector3ub & pixelColor) {
	sibr::Vector3i c = pixelColor.cast<int>() - backgroundColor;
	return c.squaredNorm() < threshold_black_color;
}

/* 
for checking if a file name is made out only of digits and not letters (like texture file names)
*/
bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

/*
 * add pixel(x,y) to the queue if it is black
 * pd: they are not marked as visited if they are not black. why? a) because it is only used to avoid adding black pixels multiple times
 */
void addPixelToQueue(const sibr::Vector2i & pixel, const sibr::ImageRGB & img, std::priority_queue<sibr::Vector2i> & queue, sibr::Array2d<bool> & arrayVisited) {
	if (!arrayVisited(pixel.x(), pixel.y()) && isBlack(img(pixel.x(), pixel.y()))) {
		queue.push(pixel);
		arrayVisited(pixel.x(), pixel.y()) = true;
	}
}

void findBounds(sibr::Array2d<bool> & isBlack, Bounds & bounds)
{
	bool wasUpdated = true;

	while (wasUpdated) {	

		wasUpdated = false;

		for (int x = bounds.xMin; x <= bounds.xMax; ++x) {
			wasUpdated = wasUpdated || isBlack(x, bounds.yMax) || isBlack(x, bounds.yMin);
		}
		for (int y = bounds.yMin; y <= bounds.yMax; ++y) {
			wasUpdated = wasUpdated || isBlack(bounds.xMax, y) || isBlack(bounds.xMin, y);
		}

		if (wasUpdated) {
			--bounds.xMax;
			++bounds.xMin;
			--bounds.yMax;
			++bounds.yMin;
		}

		if (bounds.xMax - bounds.xMin < thinest_bounding_box_size || bounds.yMax - bounds.yMin < thinest_bounding_box_size) {
			break;
		}
	}
}


Bounds getBounds(const sibr::ImageRGB & img) {
	int w = img.w() - 1;
	int h = img.h() - 1;

	sibr::Array2d<bool> wasVisited(img.w(), img.h(), false);
	sibr::Array2d<bool> isBlack(img.w(), img.h(), false);
	std::priority_queue<sibr::Vector2i> pixelsQueue;

	//init with boundary pixel (set initial pixelQueue)
	// add first row and last row of pixels to the pixelsQueue (if they are black) and marked them as visited
	for (int x = 0; x<w; ++x) {
		addPixelToQueue(sibr::Vector2i(x, 0), img, pixelsQueue, wasVisited);
		addPixelToQueue(sibr::Vector2i(x, h - 1), img, pixelsQueue, wasVisited);
	}

	// add left col and right col of pixels to the pixelsQueue (if they are black) and marked them as visited
	for (int y = 0; y<h; ++y) {
		addPixelToQueue(sibr::Vector2i(0, y), img, pixelsQueue, wasVisited);
		addPixelToQueue(sibr::Vector2i(w - 1, y), img, pixelsQueue, wasVisited);
	}

	//neighbors shifts
	sibr::Vector2i shiftsArray[4] = { sibr::Vector2i(1,0), sibr::Vector2i(-1,0), sibr::Vector2i(0,-1), sibr::Vector2i(0,1) };
	std::vector<sibr::Vector2i> shifts(shiftsArray, shiftsArray + sizeof(shiftsArray) / sizeof(sibr::Vector2i));

	//find all black pixels linked to the boundaries
	while (pixelsQueue.size() > 0) {
		sibr::Vector2i currentPix = pixelsQueue.top();
		pixelsQueue.pop();
		// if it was in the queue, then it was black
		isBlack(currentPix.x(), currentPix.y()) = true;

		for (auto & shift : shifts) {
			sibr::Vector2i newPos = currentPix + shift;
			if (img.isInRange(newPos.x(), newPos.y())) {
				addPixelToQueue(newPos, img, pixelsQueue, wasVisited);
			}
		}

	}

	/*
	if( debug_viz ){
	sibr::ImageRGB viz(w,h);
	for(int y=0; y<h; y++){
	for(int x=0; x<w; ++x){
	viz(x,y) = ( isBlack(x,y) ? sibr::Vector3ub(255,255,255)  : sibr::Vector3ub(0,0,0)  );
	}
	}

	sibr::show( viz.resized(1200,800) );
	}
	*/

	//find maximal bounding box not containing black pixels
	Bounds bounds(img);
	findBounds(isBlack, bounds);

	bounds.xRatio = bounds.xMax / (float)img.w() - 0.5f;
	bounds.yRatio = bounds.yMax / (float)img.h() - 0.5f;

	int proposedWidth = bounds.xMax - bounds.xMin;
	int proposedHeight = bounds.yMax - bounds.yMin;

	bounds.width = (img.w() - proposedWidth) * toleranceFactor + proposedWidth;
	bounds.height = (img.h() - proposedHeight) * toleranceFactor + proposedHeight;

	if (debug_viz) {
		//std::cout << bounds.xMin << " " << bounds.xMax << " " <<  bounds.yMin << " " << bounds.yMax << std::endl;
	}

	return bounds;
}

sibr::Vector2i calculateAvgResolution(const std::vector< Path > & imagePaths)
{
	unsigned nrBatches = static_cast<int>(ceil((float)(imagePaths.size()) / PROCESSING_BATCH_SIZE));

	std::vector<std::pair<std::pair<long, long>, unsigned>> sumAndNrItems(nrBatches);

	for (unsigned batchId = 0; batchId < nrBatches; batchId++) {

		unsigned nrItems = (batchId != nrBatches - 1) ? PROCESSING_BATCH_SIZE : ((nrBatches * PROCESSING_BATCH_SIZE != imagePaths.size()) ? (imagePaths.size() - (PROCESSING_BATCH_SIZE * batchId)) : PROCESSING_BATCH_SIZE);
		long sumOfWidths = 0;
		long sumOfHeights = 0;

		std::vector<sibr::ImageRGB> chunkOfInputImages(nrItems);

		//std::cout << "batch id=" << batchId << " size=" << nrItems << "\n";
		// load images in parallel (OpenMP 2.0 doesn't allow unsigned int as index. must be signed integral type)
		#pragma omp parallel for
		for (int localImgIndex = 0; localImgIndex < nrItems; localImgIndex++) {
			unsigned globalImgIndex = (batchId * PROCESSING_BATCH_SIZE) + localImgIndex;
			chunkOfInputImages.at(localImgIndex).load(imagePaths.at(globalImgIndex).string(), false);

			#pragma omp critical
			{
				sumOfWidths += chunkOfInputImages[localImgIndex].w();
				sumOfHeights += chunkOfInputImages[localImgIndex].h();
			}
		}
		std::pair<long, long> sums(sumOfWidths, sumOfHeights);
		std::pair<std::pair<long, long>, unsigned> batch(sums, nrItems);
		sumAndNrItems[batchId] = batch;
	}

	long sumOfWidth = 0;
	long sumOfHeight = 0;
	for (unsigned i = 0; i < sumAndNrItems.size(); i++) {
		sumOfWidth += sumAndNrItems[i].first.first;
		sumOfHeight += sumAndNrItems[i].first.second;
	}

	unsigned globalAvgWidth = sumOfWidth / imagePaths.size();
	unsigned globalAvgHeight = sumOfHeight / imagePaths.size();

	return sibr::Vector2i(globalAvgWidth, globalAvgHeight);
}


sibr::Vector2i findBiggestImageCenteredBox(const Path & root, const std::vector< Path > & imagePaths, const std::vector<sibr::Vector2i> & resolutions, int avgWidth = 0, int avgHeight = 0)
{
	// check if avg resolution needs to be calculated
	if (avgWidth == 0 || avgHeight == 0) {
		std::cout << "about to calculate avg resolution. use python get_image_size script if dataset has too many images\n";
		sibr::Vector2i avgResolution = calculateAvgResolution(imagePaths);
		avgWidth = avgResolution.x();
		avgHeight = avgResolution.y();
	}

	std::cout << "[distordCrop] average resolution " << avgWidth << "x" << avgHeight << " and nr resolutions given: " << resolutions.size() << "\n";

	// discard images with different resolution
	std::vector<uint> preExcludedCams;
	for (unsigned i = 0; i < resolutions.size(); i++) {
		bool shrinkHorizontally = ((resolutions[i].x() < avgWidth) && ((avgWidth - resolutions[i].x()) > avgWidth * resolutionThreshold)) ? true : false;
		bool shrinkVertically = ((resolutions[i].y() < avgHeight) && ((avgHeight - resolutions[i].y()) > avgHeight * resolutionThreshold)) ? true : false;
		if (shrinkHorizontally || shrinkVertically) {
			preExcludedCams.push_back(i);
			std::cout << "[distordCrop] excluding input image " << i << " resolution=" << resolutions[i].x() << "x" << resolutions[i].y() << "\n";
		}
	}

	std::cout << "[distordCrop] nr pre excluded images " << preExcludedCams.size() << "\n";

	// compute bounding boxes for all non-discarded images
	std::vector<Bounds> allBounds(imagePaths.size());

	unsigned nrBatches = static_cast<int>(ceil((float)(imagePaths.size()) / PROCESSING_BATCH_SIZE));

	// processs batches sequentially (we don't want to run out of memory)
	for (unsigned batchId = 0; batchId < nrBatches; batchId++) {

		unsigned nrItems = (batchId != nrBatches - 1) ? PROCESSING_BATCH_SIZE : ((nrBatches * PROCESSING_BATCH_SIZE != imagePaths.size()) ? (imagePaths.size() - (PROCESSING_BATCH_SIZE * batchId)) : PROCESSING_BATCH_SIZE);

		std::vector<sibr::ImageRGB> chunkOfInputImages(nrItems);

		// load images in parallel (OpenMP 2.0 doesn't allow unsigned int as index. must be signed integral type)
		#pragma omp parallel for
		for (int localImgIndex = 0; localImgIndex < nrItems; localImgIndex++) {
			unsigned globalImgIndex = (batchId * PROCESSING_BATCH_SIZE) + localImgIndex;
			// if cam was discarded, do nothing
			if (std::find(preExcludedCams.begin(), preExcludedCams.end(), globalImgIndex) == preExcludedCams.end()) {
				// only now load the img
				chunkOfInputImages.at(localImgIndex).load(imagePaths.at(globalImgIndex).string(), false);
				allBounds.at(globalImgIndex) = getBounds(chunkOfInputImages.at(localImgIndex));
				//#pragma omp critical
				//{
				//	std::cout << globalImgIndex << " " << allBounds[globalImgIndex].xMin << " " << allBounds[globalImgIndex].xMax << " " << allBounds[globalImgIndex].yMin << " " << allBounds[globalImgIndex].yMax << std::endl;
				//}
			}
		}
	}

	Bounds finalBounds(resolutions.at(0));

	int im_id = 0;

	// generate exclude file based on x and y ratios
	std::string excludeFilePath = root.string() + "/exclude_images.txt";
	std::ofstream excludeFile(excludeFilePath, std::ios::trunc);

	int minWidth = -1;
	int minHeight = -1;

	for (auto & bounds : allBounds) {
		bool wasPreExcluded = std::find(preExcludedCams.begin(), preExcludedCams.end(), im_id) != preExcludedCams.end();

		if (!wasPreExcluded && bounds.xRatio > threshold_ratio_bounding_box_size && bounds.yRatio > threshold_ratio_bounding_box_size) {
			// get global x and y ratios
			bool check = false;
			if (bounds.xRatio < finalBounds.xRatio) {
				finalBounds.xRatio = bounds.xRatio;
				check = true;
			}
			if (bounds.yRatio < finalBounds.yRatio) {
				finalBounds.yRatio = bounds.yRatio;
				check = true;
			}
			if (check) {
				//std::cout << "\t" << im_id << " : " << bounds.xRatio << " " << bounds.yRatio  << " " << finalBounds.xRatio << " " << finalBounds.yRatio 
				//	<< " : " << allBounds[im_id].display() << " : " << finalBounds.display() << std::endl;
			}

			minWidth = (minWidth < 0 || bounds.width < minWidth) ? bounds.width : minWidth;
			minHeight = (minHeight < 0 || bounds.height < minHeight) ? bounds.height : minHeight;
		}
		else {
			std::cerr << im_id << " ";
			excludeFile << im_id << " ";

			std::cout << wasPreExcluded << " " << bounds.xRatio << " " << threshold_ratio_bounding_box_size << " " << bounds.yRatio << " " << threshold_ratio_bounding_box_size << std::endl;
		}

		++im_id;

	}
	excludeFile.close();
	std::cout << std::endl;

	return sibr::Vector2i(minWidth, minHeight);

//	// load input images and get avg resolution
//	std::vector<sibr::ImageRGB> inputImgs(imagePaths.size());
//	//int avgWidth = 0;
//	//int avgHeight = 0;
//	std::cout << "[distordCrop] loading input images : " << std::flush;
//
//	std::chrono::time_point <std::chrono::system_clock> start, end;
//	start = std::chrono::system_clock::now();
//
//#pragma omp parallel for
//	for (int id = 0; id<(int)inputImgs.size(); ++id) {
//		inputImgs.at(id).load(imagePaths.at(id).string(), false);
//#pragma omp critical
//		{
//			avgWidth += inputImgs[id].w();
//			avgHeight += inputImgs[id].h();
//		}
//		//std::cerr << id << " ";
//	}
//
//	avgWidth = (int)((float)(avgWidth) / inputImgs.size());
//	avgHeight = (int)((float)(avgHeight) / inputImgs.size());
//
//	end = std::chrono::system_clock::now();
//
//	std::cout << "[distordCrop] average resolution " << avgWidth << "x" << avgHeight << " elapsed time=" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.\n";

	

	//// discard images with different resolution
	//for (int id = 0; id<(int)inputImgs.size(); ++id) {
	//	bool shrinkHorizontally = ((inputImgs[id].w() < avgWidth) && ((avgWidth - inputImgs[id].w()) > avgWidth * resolutionThreshold)) ? true : false;
	//	bool shrinkVertically = ((inputImgs[id].h() < avgHeight) && ((avgHeight - inputImgs[id].h()) > avgHeight * resolutionThreshold)) ? true : false;
	//	if (shrinkHorizontally || shrinkVertically) {
	//		preExcludedCams.push_back(id);
	//		std::cout << "[distordCrop] excluding input image " << id << " resolution=" << inputImgs[id].w() << "x" << inputImgs[id].h() << "\n";
	//	}
	//}


	//// compute bounding boxes for all non-discarded images
	//std::vector<Bounds> allBounds(imagePaths.size());
	//std::cout << "[distordCrop] computing bounding boxes : " << std::flush;
//
//#pragma omp parallel for
//	for (int id = 0; id<(int)inputImgs.size(); ++id) {
//		if (std::find(preExcludedCams.begin(), preExcludedCams.end(), id) == preExcludedCams.end()) {
//			allBounds.at(id) = getBounds(inputImgs.at(id));
//#pragma omp critical
//			{
//				std::cout << id << " " << allBounds[id].xMin << " " << allBounds[id].xMax << " " << allBounds[id].yMin << " " << allBounds[id].yMax << std::endl;
//			}
//			
//		}
//		//std::cerr << id << " ";
//	}
//	std::cerr << " done " << std::endl;
//
//
//	Bounds finalBounds(inputImgs.at(0));

}

sibr::Vector2i findMinImageSize(const Path & root, const std::vector< Path > & imagePaths) {
	std::vector<sibr::ImageRGB> inputImgs(imagePaths.size());
	std::vector<sibr::Vector2i> imSizes(imagePaths.size());

	std::cout << "[distordCrop] loading input images : " << std::flush;

#pragma omp parallel for
	for (int id = 0; id < (int)inputImgs.size(); ++id) {
		inputImgs.at(id).load(imagePaths.at(id).string(), false);
		imSizes[id] = inputImgs[id].size().cast<int>();
	}

	sibr::Vector2i minSize = imSizes[0];
	for (const auto & size : imSizes) {
		minSize = minSize.cwiseMin(size);
	}

	// generate exclude file based on x and y ratios
	std::string excludeFilePath = root.string() + "/excludeImages.txt";
	std::ofstream excludeFile(excludeFilePath, std::ios::trunc);
	excludeFile.close();

	return minSize;
}



int main(const int argc, const char* const* argv)
{
	// parameters stuff
	sibr::CommandLineArgs::parseMainArgs(argc, argv);
	DistordCropAppArgs myArgs;

	std::string datasetPath = myArgs.dataset_path;

	threshold_black_color = myArgs.black_threshold;
	threshold_bounding_box_size = myArgs.minSizeThresholdArg;
	threshold_ratio_bounding_box_size = myArgs.minRatioThresholdArg;
	toleranceFactor = myArgs.toleranceArg;

	if( myArgs.vizArg.get()) {
		debug_viz = true;
	}
	
	int avgWidth = myArgs.avgWidthArg;
	int avgHeight = myArgs.avgHeightArg;
	
	bool sameSize = myArgs.modeArg;
	// end parameters stuff

	Path root(datasetPath);

	std::cout << "[distordCrop] looking for input images : " << std::endl;
	std::vector< Path > imagePaths;
	directory_iterator it(root), eod;
	std::vector<sibr::Vector2i> resolutions;

	BOOST_FOREACH(Path const &p, std::make_pair(it, eod)) {
		if (is_regular_file(p) && ( p.extension() == ".jpg" || p.extension() == ".png" ) && is_number(p.stem().string())) {

			std::cout << "\t " << p.filename().string() << std::endl;
			imagePaths.push_back(p);
		}
		else if (is_regular_file(p) && p.extension() == ".txt" && p.stem().string() == "resolutions") {
			//std::cout << "there is a resolution file at " << p.string() << "\n";

			// read resolutions file
			ifstream inputFile(p.string());

			std::string line;
			while (getline(inputFile, line)) {
				std::stringstream iss(line);
				std::string pathToImg;
				std::string widthStr;
				std::string heightStr;

				getline(iss, pathToImg, '\t');
				getline(iss, widthStr, '\t');
				getline(iss, heightStr, '\n');

				sibr::Vector2i res(std::stoi(widthStr), std::stoi(heightStr));

				resolutions.push_back(res);

				//std::cout << "paht to img=" << pathToImg << " " << widthStr << "x" << heightStr << "\n";
			}

			inputFile.close();
		}
	}

	if (resolutions.size() == 0) {
		std::cout << "[distordCrop] WARNING : no resolution.txt file found" << std::endl;
		return 0;
	}

	if (imagePaths.size() == 0) {
		std::cout << "[distordCrop] WARNING : no images found" << std::endl;
		return 0;
	}

	if (resolutions.size() != imagePaths.size()) {
		std::cout << "[distordCrop] WARNING : different number of input images and resolutions written in resolutions.txt" << std::endl;
		return 0;
	}

	int minWidth, minHeight, new_half_w, new_half_h;
	
	if (sameSize) {
		std::cout << " ALL IMG SHOULD HAVE SAME SIZE " << std::endl;
		sibr::Vector2i minSize = findBiggestImageCenteredBox(root, imagePaths, resolutions, avgWidth, avgHeight);
		std::cout << "[distordCrop] minSize " << minSize[0] << "x" << minSize[1] << std::endl;
		minWidth = minSize[0];
		minHeight = minSize[1];
	} else {
		std::cout << " ALL IMG SHOULD NOT HAVE SAME SIZE " << std::endl;
		sibr::Vector2i minSize = findMinImageSize(root, imagePaths);
		minWidth = minSize[0];
		minHeight = minSize[1];
	}

	new_half_w = (minWidth % 2 == 0) ? (minWidth / 2) : (--minWidth / 2);
	new_half_h = (minHeight % 2 == 0) ? (minHeight / 2) : (--minHeight / 2);

	while ((new_half_w % 4) != 0) { --new_half_w; }
	while ((new_half_h % 4) != 0) { --new_half_h; }

	std::string outputFilePath = root.string() + "/cropNewSize.txt";
	std::ofstream file(outputFilePath, std::ios::trunc);
	if (file) {
		file << 2 * new_half_w << " " << 2 * new_half_h;
		file.close(); 
	}
	else {
		std::cout << "[distordCrop]  ERROR cant open file : " << outputFilePath << std::endl;
		return 1;
	}

	std::cout << "[distordCrop] done, new size is " << 2 * new_half_w << " x " << 2 * new_half_h << std::endl;

	return 0;
}


//for( auto & bounds : allBounds ){

//	bool checkBB = false;
//	if (
//		finalBounds.xMax > bounds.xMax ||
//		finalBounds.xMin < bounds.xMin ||
//		finalBounds.yMax > bounds.yMax ||
//		finalBounds.xMin < bounds.xMin
//		) {
//		checkBB = true;
//		//std::cout << " !!! " << std::endl;
//	}

//	finalBounds.xMax = std::min( finalBounds.xMax , bounds.xMax);
//	finalBounds.yMax = std::min( finalBounds.yMax , bounds.yMax);
//	finalBounds.xMin = std::max( finalBounds.xMin , bounds.xMin);
//	finalBounds.yMin = std::max( finalBounds.yMin , bounds.yMin);

//	if (checkBB) {
//		std::cout << "\t" << im_id << " : " << (bounds.xMax - bounds.xMin) * (bounds.yMax - bounds.yMin) << " : " << (bounds.xMax - bounds.xMin)  << " : " << (bounds.yMax - bounds.yMin) << 
//			" : " << allBounds[im_id].display() << " : " << finalBounds.display() << std::endl;
//	}

//	++im_id;
//}


// NOTE: not all input images have the same width and height. We are only using the first one as reference
/*
theo's code

int center_x = (int)inputImgs.at(0).w() / 2;
int center_y = (int)inputImgs.at(0).h() / 2;

finalBounds.xMax = center_x + (int)(finalBounds.xRatio*inputImgs.at(0).w());
finalBounds.xMin = center_x - (int)(finalBounds.xRatio*inputImgs.at(0).w());
finalBounds.yMax = center_y + (int)(finalBounds.yRatio*inputImgs.at(0).h());
finalBounds.yMin = center_y - (int)(finalBounds.yRatio*inputImgs.at(0).h());

int new_half_w = std::min(finalBounds.xMax - center_x, center_x - finalBounds.xMin);
int new_half_h = std::min(finalBounds.yMax - center_y, center_y - finalBounds.yMin);
while ((new_half_w % 2) != 0) { --new_half_w; }
while ((new_half_h % 2) != 0) { --new_half_h; }

if (debug_viz) {
for (int id = 0; id<(int)inputImgs.size(); ++id) {
sibr::ImageRGB img(inputImgs.at(id).w(), inputImgs.at(id).h());
for (int y = 0; y<(int)inputImgs.at(id).h(); y++) {
for (int x = 0; x<(int)inputImgs.at(id).w(); x++) {
img(x, y) = inputImgs.at(id)(x, y);
}
}
cv::Scalar red(255, 0, 0);
cv::rectangle(img.toOpenCVnonConst(), cv::Rect(finalBounds.xMin, finalBounds.yMin, finalBounds.xMax - finalBounds.xMin, finalBounds.yMax - finalBounds.yMin), red, 3);
show(img.resized(1200, 800));
}
}
*/