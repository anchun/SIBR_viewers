#ifndef DISTORDCROPUTILITY_H
#define DISTORDCROPUTILITY_H

#include "Config.hpp"
#include <core/graphics/Image.hpp>
#include <core/system/Vector.hpp>
#include <core/system/Array2d.hpp>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp> 

#include<fstream>
#include <queue>


namespace sibr {

	/** \brief Performs gradient integration fro tasks such as Poisson-based inpainting, smooth filling, ...
	* \ingroup sibr_imgproc
	*/
	class SIBR_IMGPROC_EXPORT DistordCropUtility
	{
	public:
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

		bool isBlack(const sibr::Vector3ub & pixelColor, Vector3i backgroundColor, int threshold_black_color);


		/*
		* for checking if a file name is made out only of digits and not letters (like texture file names)
		*/
		bool is_number(const std::string& s);

		/*
		* add pixel(x,y) to the queue if it is black
		* pd: they are not marked as visited if they are not black. why? a) because it is only used to avoid adding black pixels multiple times
		*/
		void addPixelToQueue(const sibr::Vector2i & pixel, const sibr::ImageRGB & img, std::priority_queue<sibr::Vector2i> & queue, sibr::Array2d<bool> & arrayVisited, Vector3i backgroundColor, int threshold_black_color);

		void findBounds(sibr::Array2d<bool> & isBlack, Bounds & bounds, int thinest_bounding_box_size);

		Bounds getBounds(const sibr::ImageRGB & img, Vector3i backgroundColor, int threshold_black_color, int thinest_bounding_box_size, float toleranceFactor);

		sibr::Vector2i calculateAvgResolution(const std::vector< Path > & imagePaths, std::vector<sibr::Vector2i> & resolutions, const int batch_size = 150);

		sibr::Vector2i findBiggestImageCenteredBox(const Path & root, const std::vector< Path > & imagePaths, std::vector<sibr::Vector2i> & resolutions, int avgWidth = 0, int avgHeight = 0,
			const int batch_size = 150,
			float resolutionThreshold = 0.15f,
			float threshold_ratio_bounding_box_size = 0.2f,
			Vector3i backgroundColor = Vector3i(0, 0, 0),
			int threshold_black_color = 10,
			int thinest_bounding_box_size = 5,
			float toleranceFactor = 0.0f);

		sibr::Vector2i findMinImageSize(const Path & root, const std::vector< Path > & imagePaths);


	};
}

#endif
