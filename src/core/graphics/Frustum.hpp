
#ifndef __SIBR_GRAPHICS_FRUSTUM_HPP__
# define __SIBR_GRAPHICS_FRUSTUM_HPP__

# include <array>
# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"

namespace sibr
{
	class Camera;

	/** 
	* Represent a 3D frustum defined by 6 planes.
	* \warning This class has not been strongly tested!
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT Frustum
	{
	public:

		/// Result of intersection test.
		enum TestResult
		{
			OUTSIDE = 0,
			INTERSECT,
			INSIDE
		};

		/// Frustum plane representation.
		struct Plane
		{
			float A;
			float B;
			float C;
			float D;

			/** Get the distance from a point to the plane.
			\param p 3D point
			\return distance
			*/
			float	distanceWithPoint(const Vector3f& p);

			/** Build a plane from a normal and a point.
			\param normal the normal
			\param point a point belonging to the plane
			*/
			void	buildFrom(const Vector3f& normal, const Vector3f& point);
		};

	public:

		/** Construct the furstum associated to a camera.
		\param cam the camera
		*/
		Frustum(const Camera& cam);

		/** Test if a sphere intersects the frustum or is contained in it.
		\param sphere sphere center
		\param radius sphere radis
		\return if the sphere is inside, intersecting or outside the frustum
		*/
		TestResult	testSphere(const Vector3f& sphere, float radius);

	private:

		/// Location of each plane.
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


		std::array<Plane, COUNT> _planes; ///< Frustum planes.

	};

} // namespace sibr

#endif // __SIBR_GRAPHICS_FRUSTUM_HPP__
