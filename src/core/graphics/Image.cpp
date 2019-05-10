
#include "core/graphics/Image.hpp"

namespace sibr
{
	namespace opencv
	{


		float			imageTypeCVRange(int cvDepth)
		{
			// keep in mind
			//enum { CV_8U=0, CV_8S=1, CV_16U=2, CV_16S=3, CV_32S=4, CV_32F=5, CV_64F=6 };
			static float ranges[] = {
				imageTypeRange<uint8>(),
				imageTypeRange<int8>(),
				imageTypeRange<uint16>(),
				imageTypeRange<int16>(),
				imageTypeRange<int32>(),
				imageTypeRange<float>(),
				imageTypeRange<double>()
			};
			return ranges[cvDepth];
		}

		void			convertBGR2RGB(cv::Mat& img)
		{
			switch (img.channels())
			{
			case 3:
				cv::cvtColor(img, img, CV_BGR2RGB);
				break;
			case 4:
				cv::cvtColor(img, img, CV_BGRA2RGBA);
				break;
			default:
				break;
			}
		}

		void			convertRGB2BGR(cv::Mat& img)
		{
			switch (img.channels())
			{
			case 3:
				cv::cvtColor(img, img, CV_RGB2BGR);
				break;
			case 4:
				cv::cvtColor(img, img, CV_RGBA2BGRA);
				break;
			default:
				break;
			}
		}

	} // namespace opencv

	sibr::ImageRGBA convertL32FtoRGBA(const sibr::ImageL32F & imgF)
	{
		sibr::ImageRGBA out(imgF.w(), imgF.h());
		for (uint y = 0; y < out.h(); ++y) {
			for (uint x = 0; x < out.w(); ++x) {
				unsigned char const * p = reinterpret_cast<unsigned char const *>(&imgF.pixel(x, y).x());
				for (std::size_t i = 0; i != sizeof(float); ++i) {
					out.pixel(x, y)[i] = p[i];
				}
			}
		}
		return out;
	}

	sibr::ImageL32F convertRGBAtoL32F(const sibr::ImageRGBA & imgRGBA)
	{
		sibr::ImageL32F out(imgRGBA.w(), imgRGBA.h());
		for (uint y = 0; y < out.h(); ++y) {
			for (uint x = 0; x < out.w(); ++x) {
				unsigned char * p = reinterpret_cast<unsigned char *>(&out.pixel(x, y).x());
				for (std::size_t i = 0; i != sizeof(float); ++i) {
					p[i] = imgRGBA.pixel(x, y)[i];
				}
			}
		}
		return out;
	}

	sibr::ImageRGBA convertRGB32FtoRGBA(const sibr::ImageRGB32F & imgF)
	{
		sibr::ImageRGBA out(3*imgF.w(), imgF.h());
		for (uint y = 0; y < imgF.h(); ++y) {
			for (uint x = 0; x < imgF.w(); ++x) {
				for (int k = 0; k < 3; k++) {
					unsigned char const * p = reinterpret_cast<unsigned char const *>(&imgF.pixel(x, y)[k]);
					for (std::size_t i = 0; i != sizeof(float); ++i) {
						out.pixel(k*imgF.w() + x, y)[i] = p[i];
					}
				}
			}
		}
		return out;
	}

	//sibr::ImageRGBA convertRGB32FtoRGBA_2(const sibr::ImageRGB32F & imgF)
	//{
	//	sibr::ImageRGBA out( imgF.w(), 3 * imgF.h());
	//	for (uint y = 0; y < imgF.h(); ++y) {
	//		for (uint x = 0; x < imgF.w(); ++x) {
	//			for (int k = 0; k < 3; k++) {
	//				unsigned char const * p = reinterpret_cast<unsigned char const *>(&imgF.pixel(x, y)[k]);
	//				for (std::size_t i = 0; i != sizeof(float); ++i) {
	//					out.pixel(x, k*imgF.h() + y)[i] = p[i];
	//				}
	//			}
	//		}
	//	}
	//	return out;
	//}

	SIBR_GRAPHICS_EXPORT sibr::ImageRGB32F convertRGBAtoRGB32F(const sibr::ImageRGBA & imgRGBA)
	{
		sibr::ImageRGB32F out(imgRGBA.w()/3, imgRGBA.h());
		for (uint y = 0; y < out.h(); ++y) {
			for (uint x = 0; x < out.w(); ++x) {
				for (int k = 0; k < 3; k++) {
					unsigned char * p = reinterpret_cast<unsigned char *>(&out.pixel(x, y)[k]);
					for (std::size_t i = 0; i != sizeof(float); ++i) {
						p[i] = imgRGBA.pixel(k*out.w() + x, y)[i];
					}
				}
			}
		}
		return out;
	}

	SIBR_GRAPHICS_EXPORT sibr::ImageRGBA convertNormalMapToSphericalHalf(const sibr::ImageRGB32F & imgF)
	{
		uint phi_uint; 
		uint theta_uint;
		unsigned char * phi_ptr = reinterpret_cast<unsigned char *>(&phi_uint);
		unsigned char * theta_ptr = reinterpret_cast<unsigned char *>(&theta_uint);

		ImageRGBA out(imgF.w(),imgF.h());

		for (uint i = 0; i < out.h(); ++i) {
			for (uint j = 0; j < out.w(); ++j) {		
				const double phi = std::acos((double)imgF.pixel(j, i)[2]);
				const double theta = std::atan2((double)imgF.pixel(j, i)[1], (double)imgF.pixel(j, i)[0]);
				phi_uint = (uint)((phi / M_PI) * (1 << 16));
				theta_uint = (uint)((0.5*(theta / M_PI + 1.0)) * (1 << 16));

				unsigned char * out_ptr = reinterpret_cast<unsigned char *>(&out.pixel(j, i)[0]);
				out_ptr[0] = phi_ptr[0];
				out_ptr[1] = phi_ptr[1];
				out_ptr[2] = theta_ptr[0];
				out_ptr[3] = theta_ptr[1];
			}
		}

		return out;
	}

	SIBR_GRAPHICS_EXPORT sibr::ImageRGB32F convertSphericalHalfToNormalMap(const sibr::ImageRGBA & imgRGBA)
	{	
		uint phi_uint;
		uint theta_uint; 
		unsigned char * phi_ptr = reinterpret_cast<unsigned char *>(&phi_uint);
		unsigned char * theta_ptr = reinterpret_cast<unsigned char *>(&theta_uint);

		ImageRGB32F out(imgRGBA.w(), imgRGBA.h());

		for (uint i = 0; i < out.h(); ++i) {
			for (uint j = 0; j < out.w(); ++j) {	
				unsigned char const * out_ptr = reinterpret_cast<unsigned char const *>(&imgRGBA.pixel(j, i)[0]);
				phi_ptr[0] = out_ptr[0];
				phi_ptr[1] = out_ptr[1];
				theta_ptr[2] = out_ptr[0];
				theta_ptr[3] = out_ptr[1];

				float theta = ((float)phi_uint*2.0f / (1 << 16) - 1.0f)*float(M_PI);
				float phi = ((float)theta_uint /(1 << 16))*float(M_PI);
				float sin_t = std::sin(theta);
				float cos_t = std::cos(theta);
				float sin_p = std::sin(phi);
				float cos_p = std::cos(phi);
				out.pixel(j, i) = sibr::Vector3f(sin_t*cos_p, sin_t*sin_p, cos_t);
			}
		}

		return out;
	}

	Image<unsigned char, 3> coloredClass(const Image<unsigned char, 1>::Ptr imClass) { // note that 255 is black
		//SIBR_ASSERT(T_NumComp==1 && typdeId(T_Type)==typeId(unsigned char));
		const int color_list[25][3] = {
			{255, 179, 0},{128, 62, 117},{166, 189, 215} ,{193, 0, 32},{0,128,255},{0, 125, 52},
			{246, 118, 142},{0, 83, 138},{255, 122, 92} ,{0, 255, 0},{255, 142, 0},{179, 40, 81},
			{244, 200, 0},{127, 24, 13},{147, 170, 0} ,{89, 51, 21},{241, 58, 19},{35, 44, 22},
			{83, 55, 122},{255,0,128},{128,255,0} ,{128,0,255},{206, 162, 98},{128,128,128},{255,255,255}
		};

		std::vector<Vector3ub> colors(256);
		for (int i = 0; i < 255; i++) {
			colors[i] = Vector3ub(color_list[i % 25][0], color_list[i % 25][1], color_list[i % 25][2]);
		}
		colors[255] = Vector3ub(0, 0, 0);
		Image<unsigned char, 3> imClassColor(imClass->w(), imClass->h());

		for (unsigned int i = 0; i < imClass->w(); i++) {
			for (unsigned int j = 0; j < imClass->h(); j++) {
				imClassColor.pixel(i, j) = colors[imClass->pixel(i, j).x() % 256];
			}
		}
		return imClassColor;
	}

	Image<unsigned char, 3> coloredClass(const Image<int, 1>::Ptr imClass) { // note that 255 is black
																					   //SIBR_ASSERT(T_NumComp==1 && typdeId(T_Type)==typeId(unsigned char));
		const int color_list[25][3] = {
			{ 255, 179, 0 },{ 128, 62, 117 },{ 166, 189, 215 } ,{ 193, 0, 32 },{ 0,128,255 },{ 0, 125, 52 },
			{ 246, 118, 142 },{ 0, 83, 138 },{ 255, 122, 92 } ,{ 0, 255, 0 },{ 255, 142, 0 },{ 179, 40, 81 },
			{ 244, 200, 0 },{ 127, 24, 13 },{ 147, 170, 0 } ,{ 89, 51, 21 },{ 241, 58, 19 },{ 35, 44, 22 },
			{ 83, 55, 122 },{ 255,0,128 },{ 128,255,0 } ,{ 128,0,255 },{ 206, 162, 98 },{ 128,128,128 },{ 255,255,255 }
		};

		std::vector<Vector3ub> colors(256);
		for (int i = 0; i < 255; i++) {
			colors[i] = Vector3ub(color_list[i % 25][0], color_list[i % 25][1], color_list[i % 25][2]);
		}
		colors[255] = Vector3ub(0, 0, 0);
		Image<unsigned char, 3> imClassColor(imClass->w(), imClass->h());

		for (unsigned int j = 0; j < imClass->h(); j++) {
			for (unsigned int i = 0; i < imClass->w(); i++) {

				Vector3ub color;
				if (imClass->pixel(i, j).x() < 0)
					color = colors[255];
				else
					color = colors[imClass->pixel(i, j).x() % 256];

				imClassColor.pixel(i, j) = color;
			}
		}
		return imClassColor;
	}

	void showFloat(const Image<float, 1> & im, bool logScale, double min, double max) {
		Image<unsigned char, 1> imIntensity(im.w(), im.h());
		Image<unsigned char, 3> imColor(im.w(), im.h());

		if (min == -DBL_MAX && max == DBL_MAX) {
			cv::minMaxLoc(im.toOpenCV(), &min, &max);
		}
		else if (min == -DBL_MAX) {
			double drop;
			cv::minMaxLoc(im.toOpenCV(), &min, &drop);
		}
		else if (max == DBL_MAX) {
			double drop;
			cv::minMaxLoc(im.toOpenCV(), &drop, &max);
		}

		if (logScale) {
			min = log(min);
			max = log(max);
		}

		std::cout << max << " & " << min << std::endl;

		for (unsigned int j = 0; j < im.h(); j++) {
			for (unsigned int i = 0; i < im.w(); i++) {
				if (logScale)
					imIntensity.pixel(i, j).x() = static_cast<unsigned char>(std::max(0.0, std::min((log(im.pixel(i, j).x()) - min) * 255 / (max - min), 255.0)));
				else
					imIntensity.pixel(i, j).x() = static_cast<unsigned char>(std::max(0.0, std::min((im.pixel(i, j).x() - min) * 255 / (max - min), 255.0)));
			}
		}
		cv::Mat colorMat;
		cv::applyColorMap(imIntensity.toOpenCV(), colorMat, cv::COLORMAP_PARULA);
		imColor.fromOpenCVBGR(colorMat);
		show(imColor);
	}


} // namespace sibr
