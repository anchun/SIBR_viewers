
#ifndef __SIBR_RAYCASTER_RAY_HPP__
# define __SIBR_RAYCASTER_RAY_HPP__

# include <string>
# include <vector>
# include <core/system/Vector.hpp>
# include "core/raycaster/Config.hpp"


// Force export
//template class SIBR_RAYCASTER_EXPORT Vertex<float, 3>;

namespace sibr
{

	///
	/// Represents a simple ray
	/// \ingroup sibr_raycaster
	///
	class SIBR_RAYCASTER_EXPORT Ray
	{
	public:
		/// Construct from params
		Ray( const sibr::Vector3f& orig = sibr::Vector3f(0.f, 0.f, 0.f),
			const sibr::Vector3f& dir = sibr::Vector3f(0.f, 0.f, -1.f) );

		/// Set the position from where the ray starts
		inline void		orig( const sibr::Vector3f& o );
		/// Get the position from where the ray starts
		inline const sibr::Vector3f&	orig( void ) const;

		/// Set the direction to where the ray goes. Additionally,
		/// you can precise if you want this direction to be automatically
		/// normalized or not.
		inline void		dir( const sibr::Vector3f& d, bool normalizeIt=true );
		/// Get the direction to where the ray goes.
		inline const sibr::Vector3f&	dir( void ) const;

		Vector3f at(float t) const;

	private:
		sibr::Vector3f		_orig;	///< position from where the ray starts
		sibr::Vector3f		_dir;	///< direction where the ray goes
	};

	///
	/// Contains information about a ray hit
	/// \ingroup sibr_raycaster
	///
	class SIBR_RAYCASTER_EXPORT RayHit
	{
	public:
		static const float	InfinityDist;// = std::numeric_limits<float>::infinity();

		/// Infos about the object that was hit
		struct Primitive
		{
			uint triID;		///< triangle id of the mesh that was hit
			uint geomID;	///< mesh id loaded in the raycaster
			uint instID;	///< id of the instance loaded in the raycaster
		};

		/// Barycentric coordinates
		struct BCCoord
		{
			float u;	///< u-coordinates (ranging from 0.0 to 1.0)
			float v;	///< v-coordinates (ranging from 0.0 to 1.0)
		};
		
		/// Construct hit from params
		RayHit( const Ray& r, float dist, const BCCoord& coord,
			const sibr::Vector3f& normal, const Primitive& prim );
		RayHit() {};

		/// Return the ray that was casted
		inline const Ray&			ray( void ) const;
		/// Return the distance from the ray origin to the hit
		inline float				dist( void ) const;
		/// Return the barycentric coordinates of the hit point on
		/// the triangle that was hit
		inline const BCCoord&		barycentricCoord( void ) const;
		/// Return 3 interpolation factors for extracting information
		/// at each vertex of a triangle.
		/// e.g: get fragment color using
		///   color = factor[0]*colorVert0 + factor[1]*colorVert1 + factor[2]*colorVert2
		/// It consider the following triangle: https://embree.github.io/images/triangle_uv.png
		///
		/// \todo TODO: explain better this comment (do it yourself if you have an idea!)
		sibr::Vector3f			interpolateUV( void ) const;

		/// Return the normal of the triangle that was hit
		inline const sibr::Vector3f&			normal( void ) const;
		/// Return information about the primitive that was hit
		inline const Primitive&		primitive( void ) const;

		/// Return TRUE if an object was hit
		inline bool	hitSomething( void ) const;

	private:
		Ray			_ray;		///< casted ray
		float		_dist;		///< distance from the ray's origin to the hit
		BCCoord		_coord;		///< barycentric coordinate on the triangle that was hit
		sibr::Vector3f			_normal;	///< normal of the triangle that was hit
		Primitive	_prim;		///< infos about the primitive that was hit
	};

	///// DEFINITION /////
	
	void		Ray::orig( const sibr::Vector3f& o ) {
		_orig = o;
	}
	const sibr::Vector3f&	Ray::orig( void ) const {
		return _orig;
	}

	void		Ray::dir( const sibr::Vector3f& d, bool normalizeIt) {
		_dir = (normalizeIt)? sibr::Vector3f(d.normalized()) : d;
	}
	const sibr::Vector3f&	Ray::dir( void ) const {
		return _dir;
	}

	inline Vector3f Ray::at(float t) const
	{
		return orig() + t * dir();
	}



	const Ray&			RayHit::ray( void ) const {
		return _ray;
	}
	float				RayHit::dist( void ) const {
		return _dist;
	}
	const RayHit::BCCoord&		RayHit::barycentricCoord( void ) const {
		return _coord;
	}
	const sibr::Vector3f&			RayHit::normal( void ) const {
		return _normal;
	}
	const RayHit::Primitive&		RayHit::primitive( void ) const {
		return _prim;
	}

	bool	RayHit::hitSomething( void ) const {
		return (_dist != RayHit::InfinityDist);
	}


} // namespace sibr

#endif // __SIBR_RAYCASTER_RAY_HPP__

