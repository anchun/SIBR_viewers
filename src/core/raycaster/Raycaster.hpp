
#ifndef __SIBR_RAYCASTER_RAYCASTER_HPP__
# define __SIBR_RAYCASTER_RAYCASTER_HPP__

# pragma warning(push, 0)
#  include <embree2/rtcore.h>
#  include <embree2/rtcore_ray.h>
#  include <xmmintrin.h>	// functions for setting the control register
#  include <pmmintrin.h>	// functions for setting the control register
# pragma warning(pop)

# include <core/graphics/Mesh.hpp>
# include <core/system/Matrix.hpp>
# include "core/raycaster/Config.hpp"
# include "core/raycaster/Ray.hpp"

// Force export
//template class SIBR_RAYCASTER_EXPORT std::shared_ptr<__RTCDevice>;
//template class SIBR_RAYCASTER_EXPORT std::shared_ptr<__RTCScene>;



namespace sibr
{
	//template class SIBR_RAYCASTER_EXPORT std::shared_ptr<__RTCDevice>;

	///
	/// This class can be used to cast rays in scene containing triangulate
	/// meshes. Thus you can see if rays intersect geometry and get
	/// information about the hit (such as coordinates, distance, triangle id).
	///
	/// You should have one or few instance of this class (for performance
	/// purposes). Each instance can run in parallel.
	///
	/// DevNote: This abstract the underlying embree library.
	/// \ingroup sibr_raycaster
	///
	class SIBR_RAYCASTER_EXPORT Raycaster
	{
	public:
		typedef std::shared_ptr<__RTCDevice>	RTCDevicePtr;
		typedef std::shared_ptr<__RTCScene>		RTCScenePtr;
		typedef std::shared_ptr<Raycaster>		Ptr;

		typedef	uint	geomId;
		/// stores a number representing an invalid geom id
		static const geomId InvalidGeomId; 

		~Raycaster( void );


		/// Init the raycaster and return TRUE if everything
		/// is good.
		/// Called automatically whenever you call a member that need this
		/// instance to be init. However, you can call it manually to check
		/// error on init.
		bool	init( RTCSceneFlags sceneType = RTC_SCENE_STATIC, RTCAlgorithmFlags intersectType = RTC_INTERSECT1);

		/// Add a triangulate mesh to the raycast scene. Return the id
		/// of the geometry added so you can track your mesh (and compare
		/// its id to the one stored in RayHits).
		/// Return Raycaster::InvalidGeomId if it fails.
		geomId	addMesh( const sibr::Mesh& mesh );

		/// Like addMesh but for dynamic geometry
		geomId	addDynamicMesh( const sibr::Mesh& mesh );

		/// Like addMesh but with a switch for DYNAMIC or STATIC geometry
		geomId	addGenericMesh( const sibr::Mesh& mesh, RTCGeometryFlags type );

		/// Transform the vertices of a mesh by sibr::Matrix4f mat
		/// Note that the original positions are always stored *unchanged* in mesh.vertices -- 
		/// we only xform the vertices in the embree buffer
		void xformRtcMeshOnly(sibr::Mesh& mesh, geomId mesh_id, sibr::Matrix4f& mat, sibr::Vector3f&, float&);

		/// Launch a ray into the raycast scene. Return information about
		/// this cast in RayHit.
		/// To simply know if something has been hit,
		/// 'use RayHit::hitSomething()'.
		/// Optionally you can define \param minDist that is how far from
		/// the ray's origin the raycasting actually begins. (e.g. you
		/// use to avoid colliding with the triangle where you start or
		/// to walk through the ray, collecting even occluded triangles).
		RayHit	intersect( const Ray& ray, float minDist=0.f  );

		std::array<RayHit, 8>	intersect8(const std::array<Ray, 8>& inray,const std::vector<int> & valid8=std::vector<int>(8,-1), float minDist = 0.f );

		/// Optimized ray-cast that only informs you either the ray
		/// hit something or not. Return TRUE if the ray hit something.
		/// (This is the implementation of rtcOccluded)
		/// Optionally you can define \param minDist that is how far from
		/// the ray's origin the raycasting actually begins. (e.g. you
		/// use to avoid colliding with the triangle where you start or
		/// to walk through the ray, collecting even occluded triangles).
		bool	hitSomething( const Ray& ray, float minDist=0.f );

		std::array<bool, 8>	hitSomething8(const std::array<Ray, 8>& inray, float minDist = 0.f);

		/// Disable geometry to avoid ray tracing it (eg background
		/// when only intersecting a foreground object). \todo TODO -- doesnt work
		void	disableGeom(geomId id) { rtcDisable((_scene.get()), id); rtcUpdate(_scene.get(), id); rtcCommit(_scene.get()); }

		/// Enable geometry to avoid ray tracing it (eg background
		/// when only intersecting a foreground object). \todo TODO -- doesnt work
		void	enableGeom(geomId id) { rtcEnable((_scene.get()), id); rtcUpdate(_scene.get(), id); rtcCommit(_scene.get());}

		/// Delete geometry
		void	deleteGeom(geomId id) { rtcDeleteGeometry((_scene.get()), id); rtcUpdate(_scene.get(), id); rtcCommit(_scene.get());} 

		/// clears RTCScenePtr
		void clearGeometry();

		/// returns the normalized smooth normal (shading normal) from a hit, assuming the mesh has normals
		/// \param mesh sibr::Mesh used by raycaster
		/// \param hit intersection basic information
		static sibr::Vector3f smoothNormal(const sibr::Mesh & mesh, const RayHit & hit);

		/// \return smooth color from a hit (barycentric interpolation), assuming the mesh has colors
		/// \param mesh sibr::Mesh used by raycaster
		/// \param hit intersection basic information
		static sibr::Vector3f smoothColor(const sibr::Mesh & mesh, const RayHit & hit);

		/// \return smooth texcoords from a hit (barycentric interpolation), assuming the mesh has UVs
		/// \param mesh sibr::Mesh used by raycaster
		/// \param hit intersection basic information
		static sibr::Vector2f smoothUV(const sibr::Mesh & mesh, const RayHit & hit);

	private: // STATIC //

		/// Will be called by embree whenever an error occurs
		static void rtcErrorCallback(void* userPtr, RTCError code, const char* msg);

		/// Used to initialize flag of registers used by SSE
		static bool g_initRegisterFlag; // = false
		static RTCDevicePtr	g_device;	///< embree device (context for a raycaster)

	private: // MEMBER //

		RTCScenePtr		_scene;		///< scene storing raycastable meshes
		RTCScenePtr	scene() 	{ return _scene; }
		RTCDevicePtr	_devicePtr;	///< embree device (context for a raycaster)
	};

	///// DEFINITION /////

} // namespace sibr

#endif // __SIBR_RAYCASTER_RAYCASTER_HPP__

