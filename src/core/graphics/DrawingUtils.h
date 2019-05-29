#ifndef __SIBR_GRAPHICS_DRAWING_UTILS_HPP__
# define __SIBR_GRAPHICS_DRAWING_UTILS_HPP__

#include <core/graphics/Image.hpp>

/**
* \ingroup sibr_graphics
*/
class SIBR_GRAPHICS_EXPORT DrawingUtils 
{
public :
	void pixel( sibr::ImageRGB & img , const sibr::Vector2i & pos, const sibr::Vector3ub & color , float alpha = 0 );

	virtual void updatePixel( sibr::ImageRGB & img , const sibr::Vector2i & pos, const sibr::Vector3ub & color , float alpha = 0 );

	virtual void patch( sibr::ImageRGB & img , const sibr::Vector2i & position , const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int radius=5, int thickness=1 );

	virtual void line(sibr::ImageRGB & img, const sibr::Vector2i & pA, const sibr::Vector2i & pB, const sibr::Vector3ub & color = sibr::Vector3ub(255, 0, 0), int thickness = 1);

	virtual void rectangle( sibr::ImageRGB & img, const sibr::Vector2i & cornerA, const sibr::Vector2i & cornerB, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int thickness=1 );
	
	virtual void rectangleFilled( sibr::ImageRGB & img, const sibr::Vector2i & cornerA, const sibr::Vector2i & cornerB, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0) );
	
	virtual void plusSign( sibr::ImageRGB & img, const sibr::Vector2i & center, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int radius=5, int thickness=1 );

	virtual void crossSign( sibr::ImageRGB & img, const sibr::Vector2i & center, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int radius=5, int thickness=1 );

	virtual void diamond( sibr::ImageRGB & img, const sibr::Vector2i & center, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int radius=5 );

	virtual void ellipse( sibr::ImageRGB & img, const sibr::Vector2i & center, const sibr::Vector2i & axesLength, float angle, const sibr::Vector3ub & color=sibr::Vector3ub(255,0,0), int thickness=1 );


	template<typename T_Type, unsigned int T_NumComp>
	static Eigen::Matrix<T_Type, T_NumComp, 1, Eigen::DontAlign> clampPix(
		const Eigen::Matrix<float, T_NumComp, 1, Eigen::DontAlign> & pix
	)
	{
		return (pix.unaryExpr([](float f) { return sibr::clamp(f, 0.0f, sibr::opencv::imageTypeRange<T_Type>()); })).cast<T_Type>();
	}

	template<unsigned int T_NumComp>
	static Eigen::Matrix<float, T_NumComp, 1, Eigen::DontAlign> uniDimensionalMonocubic(
		float t,
		const Eigen::Matrix<float, T_NumComp, 4, Eigen::DontAlign> & fs
	)
	{
		static const Eigen::Matrix<float, 4, 4> m = 0.5f* (Eigen::Matrix<float, 4, 4>() <<
			0, 2, 0, 0,
			-1, 0, 1, 0,
			2, -5, 4, -1,
			-1, 3, -3, 1
			).finished().transpose();

		return fs*(m*Eigen::Matrix<float, 4, 1>(1, t, t*t, t*t*t));
	}

	template<typename T_Type, unsigned int T_NumComp>
	static Eigen::Matrix<T_Type, T_NumComp, 1, Eigen::DontAlign> bicubic(
		const sibr::Image<T_Type, T_NumComp>& img,
		const sibr::Vector2f& position
	)
	{
		typedef Eigen::Matrix<float, T_NumComp, 4, Eigen::DontAlign> Pxls;

		static const std::vector<std::vector<sibr::Vector2i> > offsets = {
			{ { -1,-1 },{ 0,-1 } ,{ 1,-1 },{ 2,-1 } },
			{ { -1,0 },{ 0,0 } ,{ 1,0 },{ 2,0 } },
			{ { -1,1 },{ 0,1 } ,{ 1,1 },{ 2,1 } },
			{ { -1,2 },{ 0,2 } ,{ 1,2 },{ 2,2 } }
		};

		static std::vector<Pxls> vs(4);

		if (img.w() < 4 || img.h() < 4 ) {
			return Eigen::Matrix<T_Type, T_NumComp, 1, Eigen::DontAlign>();
		}

		sibr::Vector2i cornerPixel ( (position - 0.5f*sibr::Vector2f(1, 1)).unaryExpr(
			[](float f) { return std::floor(f); }).cast<int>() );

		sibr::Vector2f ts (position - ( cornerPixel.cast<float>() + 0.5f*sibr::Vector2f(1, 1) ) );

		for (int i = 0; i < 4; ++i) {
			vs[i] <<
				img(sibr::clamp(cornerPixel[0] + offsets[i][0][0], 0, (int)img.w() - 1), sibr::clamp(cornerPixel[1] + offsets[i][0][1], 0, (int)img.h() - 1)).cast<float>(),
				img(sibr::clamp(cornerPixel[0] + offsets[i][1][0], 0, (int)img.w() - 1), sibr::clamp(cornerPixel[1] + offsets[i][1][1], 0, (int)img.h() - 1)).cast<float>(),
				img(sibr::clamp(cornerPixel[0] + offsets[i][2][0], 0, (int)img.w() - 1), sibr::clamp(cornerPixel[1] + offsets[i][2][1], 0, (int)img.h() - 1)).cast<float>(),
				img(sibr::clamp(cornerPixel[0] + offsets[i][3][0], 0, (int)img.w() - 1), sibr::clamp(cornerPixel[1] + offsets[i][3][1], 0, (int)img.h() - 1)).cast<float>();
		}

		Pxls bs;
		bs <<
			uniDimensionalMonocubic<T_NumComp>(ts[0], vs[0]),
			uniDimensionalMonocubic<T_NumComp>(ts[0], vs[1]),
			uniDimensionalMonocubic<T_NumComp>(ts[0], vs[2]),
			uniDimensionalMonocubic<T_NumComp>(ts[0], vs[3]);

		return clampPix<T_Type,T_NumComp>(uniDimensionalMonocubic<T_NumComp>(ts[1],bs));
	}

	template<typename T_Type>
	static Eigen::Matrix<T_Type, 3, 1, Eigen::DontAlign> jetColor(float gray)
	{
		sibr::Vector3f output(1, 1, 1);
		float g = std::min(1.0f, std::max(0.0f, gray));
		float dg = 0.25f;
		float d = 4.0f;
		if (g < dg) { output.x() = 0.0f; output.y() = d*g; }
		else if (g < 2.0f*dg) { output.x() = 0.0f; output.z() = 1.0f + d*(dg - g); }
		else if (g < 3.0f*dg) { output.x() = d*(g - 0.5f);  output.z() = 0.0f; }
		else { output.y() = 1.0f + d*(0.75f - g);  output.z() = 0.0f; }

		return output.unaryExpr([](float f) { return f*sibr::opencv::imageTypeRange<T_Type>(); } ).cast<T_Type>();
	}
};

#endif // __SIBR_GRAPHICS_DRAWING_UTILS_HPP__