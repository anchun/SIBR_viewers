
#ifndef __SIBR_GRAPHICS_FRUSTUM_HPP__
# define __SIBR_GRAPHICS_FRUSTUM_HPP__

# include <array>
# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"

namespace sibr
{
	class Camera;

	/** IMPORTANT NOTE: This class has not been strongly tested!
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT Frustum
	{
	public:
		enum TestResult
		{
			OUTSIDE = 0,
			INTERSECT,
			INSIDE
		};

		struct Plane
		{
			float A;
			float B;
			float C;
			float D;

			float	distanceWithPoint(const Vector3f& p);
			void	buildFrom(const Vector3f& normal, const Vector3f& point);
		};

	public:
		Frustum(const Camera& cam);

		// Note: I only needed sphere (safer than bbox) but add whatever you need.

		TestResult	testSphere(const Vector3f& sphere, float radius);

	private:
		enum 
		{
			TOP = 0, 
			BOTTOM, 
			LEFT,
			RIGHT, 
			NEARP, 
			FARP,

			COUNT
		};


		std::array<Plane, COUNT> _planes;

	};

} // namespace sibr

#endif // __SIBR_GRAPHICS_FRUSTUM_HPP__
