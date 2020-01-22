
#ifndef __SIBR_GRAPHICS_UTILS_HPP__
# define __SIBR_GRAPHICS_UTILS_HPP__

# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Image.hpp"


// Add your own utility functions here.

namespace sibr
{
	/**
	* \addtogroup sibr_graphics
	* @{
	*/

	// e.g.:
	// Matrix4f SIBR_GRAPHICS_EXPORT perspective( float fovRadian, float ratio, float zn, float zf );

	// Color map helpers.

	// Jet color map.
	//SIBR_GRAPHICS_EXPORT sibr::Vector3ub getJetColorFromProbaV(double proba);

	template<typename T_Type>
	static Eigen::Matrix<T_Type, 3, 1, Eigen::DontAlign> jetColor(float gray)
	{
		sibr::Vector3f output(1, 1, 1);
		float g = std::min(1.0f, std::max(0.0f, gray));
		float dg = 0.25f;
		float d = 4.0f;
		if (g < dg) {
			output.x() = 0.0f; 
			output.y() = d*g;
		} else if (g < 2.0f*dg) {
			output.x() = 0.0f; 
			output.z() = 1.0f + d*(dg - g);
		} else if (g < 3.0f*dg) {
			output.x() = d*(g - 0.5f); 
			output.z() = 0.0f;
		} else {
			output.y() = 1.0f + d*(0.75f - g);  
			output.z() = 0.0f;
		}

		return output.unaryExpr([](float f) { return f*sibr::opencv::imageTypeRange<T_Type>(); }).cast<T_Type>();
	}

	template<typename T_Type>
	static Eigen::Matrix<T_Type, 3, 1, Eigen::DontAlign> randomColor(){
		// We just use rand here, we don't need 'proper' PRNG.
		const uint8_t r = uint8((std::rand() % 255 + 192) * 0.5f);
		const uint8_t g = uint8((std::rand() % 255 + 192) * 0.5f);
		const uint8_t b = uint8((std::rand() % 255 + 192) * 0.5f);
		const sibr::Vector3ub output(r, g,b);
		return output.unaryExpr([](float f) { return f * sibr::opencv::imageTypeRange<T_Type>(); }).cast<T_Type>();
	}

	SIBR_GRAPHICS_EXPORT cv::Scalar jetColor(float gray);

	// Simpler but reversible colormap.
	SIBR_GRAPHICS_EXPORT sibr::Vector3ub getLinearColorFromProbaV(double proba);

	// There will be a precision loss.
	SIBR_GRAPHICS_EXPORT double getProbaFromLinearColor(const sibr::Vector3ub & color);

	//dir assumed normalized, returns [phi,theta] in [-pi,pi]x[0,pi]
	SIBR_GRAPHICS_EXPORT sibr::Vector2d cartesianToSpherical(const sibr::Vector3d & dir); 

	//dir assumed normalized, returns [u,v] in [0,1]^2
	SIBR_GRAPHICS_EXPORT sibr::Vector2d cartesianToSphericalUVs(const sibr::Vector3d & dir);

	//Inplace conversion of float image from sRGB space to linear.
	SIBR_GRAPHICS_EXPORT void sRGB2Lin(sibr::ImageRGB32F& img);

	//Inplace conversion of float image from linear space to sRGB.
	SIBR_GRAPHICS_EXPORT void lin2sRGB(sibr::ImageRGB32F& img);

	template<typename FunType, typename ...ArgsType>
	void renderTask(const std::string & s, FunType && f, ArgsType && ... args) {
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, s.c_str());
		f(args...);
		glPopDebugGroup();
	};


	inline float		lerp(float from, float to, float fac);
	inline float		inverseLerp(float from, float to, float current);

	///// INLINES /////

	float		lerp( float A, float B, float fac ) {
		return A*(1.f-fac)+B*fac;
	}

	float		inverseLerp( float from, float to, float current ) {
		return (current - from)/(to - from);
	}

	/*** @} */

} // namespace sibr


#endif // __SIBR_GRAPHICS_UTILS_HPP__
