#ifndef CROPSCALEIMAGEUTILITY_H
#define CROPSCALEIMAGEUTILITY_H

#include "Config.hpp"
#include <core/graphics/Image.hpp>

#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>


namespace sibr {

	/** \brief Performs gradient integration fro tasks such as Poisson-based inpainting, smooth filling, ...
	* \ingroup sibr_imgproc
	*/
	class SIBR_IMGPROC_EXPORT CropScaleImageUtility
	{


	public:


		struct Image {
			std::string	filename;
			unsigned	width;
			unsigned	height;
		};

		std::vector<std::string> getPathToImgs(const std::string & inputFileName);

		bool getIsEmptyFile(const char * filename);

		void logExecution(const sibr::Vector2i & resolution, unsigned nrImages, long long elapsedTime, bool wasTransformed, const char* log_file_name);

		void writeListImages(const std::string path_to_file, const std::vector<Image> & listOfImages);

		void writeScaleFactor(const std::string path_to_file, float scaleFactor);

		void writeTargetResolution(const std::string path_to_file, const sibr::Vector2i & targetResolution);

		sibr::Vector2i parseResolution(const std::string & param);

	};
}
#endif