
#ifndef __SIBR_RAYCASTER_CAMERARAYCASTER_HPP__
# define __SIBR_RAYCASTER_CAMERARAYCASTER_HPP__

# include <array>
# include <core/graphics/Image.hpp>
# include <core/assets/InputCamera.hpp>
# include "core/raycaster/Config.hpp"
# include "core/raycaster/Raycaster.hpp"

namespace sibr
{

	///
	/// Used to process casted ray from image pixels. Implement
	/// this interface and write your custom behavior.
	/// (e.g. see CameraRaycasterProcessor.hpp for built-in processor)
	/// \ingroup sibr_raycaster
	///
	class SIBR_RAYCASTER_EXPORT ICameraRaycasterProcessor
	{
	public:

		/// Destructor.
		virtual ~ICameraRaycasterProcessor( void ) {}

		/// Called for each casted ray (that hit or not). (px, py) are
		/// pixel coordinates.
		virtual void	onCast( uint px, uint py, const RayHit& hit ) = 0;

	};

	///
	/// Used for casting each pixel of an image into a raycast scene.
	/// \ingroup sibr_raycaster
	///
	class SIBR_RAYCASTER_EXPORT CameraRaycaster
	{
	public:

		/// Build from given raycaster
		CameraRaycaster( void ) { }

		/// Initialize (will be done when adding a mesh).
		bool	init( void );

		/// Add a mesh to the raycaster
		/// \param mesh the mesh
		void	addMesh( const sibr::Mesh& mesh );

		/// For each image pixel, send a ray and compute data using the provided processors
		void	castForEachPixel( const sibr::InputCamera& cam, ICameraRaycasterProcessor* processors[], uint nbProcessors,
									const std::string& optLogMessage="Executing camera raycasting");

		/// This function returns the step (in both x- and y-coordinates) between each pixel in the world
		/// space. Thus, if go through each pixel of an can image but you need their 3d world position,
		/// using this function you can get it:
		///
		///		pixel3d = dx*pixel2d.x + dy*pixel2d.y + upLeftOffset
		///
		/// where   dx is the step between each horizontal pixel,
		///         dy is the step between each vertical pixel,
		///
		static void	computePixelDerivatives( const sibr::InputCamera& cam, sibr::Vector3f& dx, sibr::Vector3f& dy, sibr::Vector3f& upLeftOffset );

		/// \return the ray direction from the camera position to the center of the input pixel (input pixel in [0,w-1]x[0,h-1])
		static sibr::Vector3f computeRayDir( const sibr::InputCamera& cam, const sibr::Vector2f & pixel );

		static void computeClippingPlanes(const sibr::Mesh & mesh, std::vector<sibr::InputCamera>& cams, std::vector<sibr::Vector2f> & nearsFars);

		Raycaster&			raycaster( void )			{ return _raycaster; }
		const Raycaster&	raycaster( void ) const 	{ return _raycaster; }

	private:

		Raycaster									_raycaster;
	};


	class SIBR_RAYCASTER_EXPORT RaycastingCamera : public sibr::InputCamera {
		SIBR_CLASS_PTR(RaycastingCamera);
	public:
		using HPlane = Eigen::Hyperplane<float, 3>;
		using Line3 = Eigen::ParametrizedLine<float, 3>;

		RaycastingCamera(const sibr::InputCamera & cam);

		sibr::Vector3f rayDirNotNormalized(const sibr::Vector2f & pixel) const;

		//return the ray direction from the camera position to the center of the input pixel (input pixel in [0,w-1]x[0,h-1])
		sibr::Vector3f rayDir(const sibr::Vector2f & pixel) const;

		Ray getRay(const sibr::Vector2f & pixel) const;

		sibr::Vector2f rayProjection(const Line3 & line) const;

		bool isInsideFrustum(const sibr::Vector3f & pt, float eps = 0.0001) const;

		sibr::Vector2f projectImg_outside_frustum_correction(const Vector3f& pt3d) const;

		sibr::Vector3f dx, dy, upLeftOffsetMinusPos;

		std::vector<HPlane> frustum_planes; // !<near>, far, top, bottom, left, right	
	};

	///// DEFINITIONS /////

} // namespace sibr

#endif // __SIBR_RAYCASTER_CAMERARAYCASTER_HPP__

