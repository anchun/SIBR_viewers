
#include "core/graphics/Utils.hpp"

namespace sibr
{


	cv::Scalar jetColor(float gray) {
		const sibr::Vector3ub col = jetColor<uchar>(gray);
		return toOpenCV<uchar, uchar, 3>(col);
	}

	sibr::Vector3ub getLinearColorFromProbaV(double proba) {
		const double scProba = 3.0 * proba;
		const double red = sibr::clamp(scProba, 0.0, 1.0);
		const double green = sibr::clamp(scProba - 1, 0.0, 1.0);
		const double blue = sibr::clamp(scProba - 2, 0.0, 1.0);
		return sibr::Vector3ub(unsigned char(red * 255), unsigned char(green * 255), unsigned char(blue * 255));
	}

	double getProbaFromLinearColor(const sibr::Vector3ub & color) {
		const double red = double(color[0]) / 255.0;
		const double green = double(color[1]) / 255.0;
		const double blue = double(color[2]) / 255.0;
		return (red + green + blue) / 3.0;
	}

	sibr::Vector2d cartesianToSpherical(const sibr::Vector3d & dir)
	{
		double theta = std::acos(dir.z());

		double phi = 0;
		if (dir.x() != 0 && dir.y() != 0) {
			phi = std::atan2(dir.y(), dir.x());
		}

		return sibr::Vector2d(phi, theta);
	}

	sibr::Vector2d cartesianToSphericalUVs(const sibr::Vector3d & dir)
	{
		const sibr::Vector2d angles = cartesianToSpherical(dir);
		const double & phi = angles[0];
		const double & theta = angles[1];

		return sibr::Vector2d(0.5*(phi / M_PI + 1.0), theta / M_PI);
	}

	float sRGB2LinF(float inF) {
		if (inF < 0.04045) {
			return inF / 12.92;
		}
		else {
			return pow((inF + 0.055) / (1.055), 2.4);
		}
	}

	float lin2sRGBF(float inF) {

		if (inF < 0.0031308) {
			return std::max(0.0, std::min(1.0, 12.92*inF));
		}
		else {
			return std::max(0.0, std::min(1.0, 1.055*pow(inF, 1.0 / 2.4) - 0.055));
		}

	}

	void sRGB2Lin(sibr::ImageRGB32F& img) {
#pragma omp parallel for
		for (int j = 0; j < img.h(); j++) {
			for (int i = 0; i < img.w(); i++) {
				for (int c = 0; c < 3; c++) {
					img.pixel(i, j)[c] = sRGB2LinF(img.pixel(i, j)[c]);
				}
			}
		}

	}

	void lin2sRGB(sibr::ImageRGB32F& img) {
#pragma omp parallel for
		for (int j = 0; j < img.h(); j++) {
			for (int i = 0; i < img.w(); i++) {
				for (int c = 0; c < 3; c++) {
					img.pixel(i, j)[c] = lin2sRGBF(img.pixel(i, j)[c]);
				}
			}
		}

	}

} // namespace sibr
