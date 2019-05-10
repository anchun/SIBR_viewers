
#ifndef __SIBR_RAYCASTER_CAMERARAYCASTER_HPP__
# define __SIBR_RAYCASTER_CAMERARAYCASTER_HPP__

# include <array>
# include <core/graphics/Image.hpp>
# include <core/assets/InputCamera.hpp>
# include "core/raycaster/Config.hpp"
# include "core/raycaster/Raycaster.hpp"

namespace sibr
{
	//template class SIBR_RAYCASTER_EXPORT Vertex<unsigned, 2>;
	//template class SIBR_RAYCASTER_EXPORT Vertex<float, 3>;

	///
	/// Used to process casted ray from image pixels. Implement
	/// this interface and write your custom behavior.
	/// (e.g. see CameraRaycasterProcessor.hpp for built-in processor)
	/// \ingroup sibr_raycaster
	///
	class SIBR_RAYCASTER_EXPORT ICameraRaycasterProcessor
	{
	public:
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

		bool	init( void );

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

		//return the ray direction from the camera position to the center of the input pixel (input pixel in [0,w-1]x[0,h-1])
		static sibr::Vector3f computeRayDir( const sibr::InputCamera& cam, const sibr::Vector2f & pixel );

		static void computeClippingPlanes(const sibr::Mesh & mesh, std::vector<sibr::InputCamera>& cams, std::vector<sibr::Vector2f> & nearsFars);

		Raycaster&			raycaster( void )			{ return _raycaster; }
		const Raycaster&	raycaster( void ) const 	{ return _raycaster; }

	private:

		Raycaster									_raycaster;
	};

	///// DEFINITIONS /////

} // namespace sibr

#endif // __SIBR_RAYCASTER_CAMERARAYCASTER_HPP__

