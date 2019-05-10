#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 

#include "globalmatting.h"

// code adapted from https://github.com/atilimcetin/global-matting

#include "guidedfilter.h"

typedef boost::filesystem::path Path;
using namespace boost::filesystem;

int main(int argc, char *argv[])
{
	std::cout << "[GLobalMatting] use : GlobalMatting.exe <datasetpath> " << std::endl;
	if( argc < 2 ) { 
		std::cout << "[GLobalMatting] ERROR : need to give a datasetpath " << std::endl;
		return 0;
	}

	Path root( argv[1] );

	Path mattingFolder(  root.string() + "/fences/out/" );

	Path outputPath( mattingFolder.string() + "/matting_results/" );

	//Path inpaintingMaskPath ( root.string()  + "/masks/");

	std::cout << "[GlobalMatting] input dataset root path : " <<  root.string() << std::endl;

	if( ! boost::filesystem::exists(outputPath) ){
		boost::filesystem::create_directory(outputPath);
		std::cout << "[GlobalMatting] creating folder : " << outputPath.string() << std::endl;
	}

	//if( ! boost::filesystem::exists(inpaintingMaskPath) ){
	//	boost::filesystem::create_directory(inpaintingMaskPath);
	//	std::cout << "[GlobalMatting] creating folder : " << inpaintingMaskPath.string() << std::endl;
	//}

	std::cout << "[GlobalMatting] looking for input images : " << std::endl;
	std::vector< Path > imagePaths;
	directory_iterator it(root), eod;
	BOOST_FOREACH(Path const &p, std::make_pair(it, eod)){ 
		if(is_regular_file(p) && p.extension() == ".jpg" ){
			std::cout <<  "\t " << p.filename().string() << std::endl;
			imagePaths.push_back(p);
		}
	}

	std::cout << "[GlobalMatting] computing matting : " << std::flush;
#pragma omp parallel for
	for(int img_id = 0; img_id<imagePaths.size(); ++img_id){
		Path & p = imagePaths.at(img_id);

		Path imgName = p.stem();

		int id = std::stoi(imgName.string());

		Path imgPathMatting = mattingFolder.string() + "/trimaps/matting_mask_" + imgName.string() + ".png";
		//std::cout << " input trimap : " << imgPathMatting.string() << std::endl;

		Path outputPathAlpha (outputPath.string()+ "/alpha_h_" +  imgName.string() + ".png");
		Path outputPathFore (outputPath.string() + "/foreground_h_" +  imgName.string() + ".png");
		Path outputPathConfidence (outputPath.string() + "/confidence_h_" +  imgName.string() + ".png");
		//Path outputPathMask ( inpaintingMaskPath.string() + std::to_string(id) + "mask.png" );

		cv::Mat image = cv::imread(p.string(), CV_LOAD_IMAGE_COLOR);
		cv::Mat trimap = cv::imread(imgPathMatting.string(), CV_LOAD_IMAGE_GRAYSCALE);

		// (optional) exploit the affinity of neighboring pixels to reduce the 
		// size of the unknown region. please refer to the paper
		// 'Shared Sampling for Real-Time Alpha Matting'.

		cv::Mat mask(trimap.rows,trimap.cols,CV_8UC3);
		for (int x = 0; x < trimap.cols; ++x){
			for (int y = 0; y < trimap.rows; ++y){
				//if( alpha.at<uchar>(y,x) == 0 ){
				if( trimap.at<uchar>(y,x) == 0 ){
					mask.at<cv::Vec3b>(y,x) = cv::Vec3b(255,255,255);
				} else {
					mask.at<cv::Vec3b>(y,x) = cv::Vec3b(0,0,0);
				}
			}
		}
		//cv::imwrite(outputPathMask.string(), mask);

		expansionOfKnownRegions(image, trimap, 9);

		cv::Mat foreground, alpha, conf;
		globalMatting(image, trimap, foreground, alpha, conf);

		// filter the result with fast guided filter
		alpha = guidedFilter(image, alpha, 10, 1e-5);
		for (int x = 0; x < trimap.cols; ++x){
			for (int y = 0; y < trimap.rows; ++y){
				if (trimap.at<uchar>(y, x) == 0)
					alpha.at<uchar>(y, x) = 0;
				else if (trimap.at<uchar>(y, x) == 255)
					alpha.at<uchar>(y, x) = 255;
			}
		}

		cv::imwrite(outputPathAlpha.string(), alpha);
		cv::imwrite(outputPathFore.string(), foreground);
		cv::imwrite(outputPathConfidence.string(), conf);

		std::cout << " " << id << std::flush;

	}

	std::cout << " >>> done " << std::endl;
	std::cout << "[GlobalMatting] job's done" << std::endl;
	return 0;
}