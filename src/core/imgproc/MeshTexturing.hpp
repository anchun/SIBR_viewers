#ifndef MESH_TEXTURING_H
#define MESH_TEXTURING_H

#include "Config.hpp"
#include <core/graphics/Image.hpp>
#include <core/graphics/Mesh.hpp>
#include <core/assets/InputCamera.hpp>
#include "core/raycaster/Raycaster.hpp"


namespace sibr {

	/** \brief Reproject images onto a mesh using the associated camera poses, 
	 * and accumulate colors in UV-space to generate a texture map.
	 * \ingroup sibr_imgproc
	 */
	class SIBR_IMGPROC_EXPORT MeshTexturing
	{
	public:

		/** \brief Export options
		 */
		enum Options : uint {
			NONE = 0,
			FLIP_VERTICAL = 1, ///< Flip the final result.
			FLOOD_FILL = 2, ///< Perform flood filling.
			POISSON_FILL = 4 ///< Perform poisson filling (slow).
		};

		/** Constructor.
		* \param sideSize dimension of the texture
		*/
		MeshTexturing(unsigned int sideSize);

		/** Set the current mesh to texture.
		 * \param mesh the mesh to use.
		 * \warn The mesh MUST have texcoords.
		 * \note If the mesh has no normals, they will be computed.
		 */
		void setMesh(const sibr::Mesh::Ptr mesh);

		/** Reproject a set of images into the texture map, using the associated cameras.
		* \param cameras the cameras poses
		* \param images the images to reproject
		*/
		void reproject(const std::vector<InputCamera> & cameras, const std::vector<sibr::ImageRGB::Ptr> & images);

		/** Get the final result. 
		* \param options the options to apply to the generated texture map.
		*/
		sibr::ImageRGB::Ptr getTexture(uint options = NONE) const;

		/** Performs flood fill of an image, following a mask.
		* \param image the image to fill
		* \param mask mask where the zeros regions will be filled
		* \return the filled image.
		*/
		static sibr::ImageRGB32F::Ptr floodFill(const sibr::ImageRGB32F & image, const sibr::ImageL8 & mask);

		/** Performs poisson fill of an image, following a mask.
		* \param image the image to fill
		* \param mask mask where the zeros regions will be filled
		* \return the filled image.
		* \warn This is slow for large images (>8k).
		*/
		static sibr::ImageRGB32F::Ptr poissonFill(const sibr::ImageRGB32F & image, const sibr::ImageL8 & mask);

	private:

		/** Test if the UV-space mesh covers a pixel of the texture map.
		* \param px pixel x coordinate
		* \param py pixel y coordinate
		* \param finalHit the hit information if there is coverage
		* \return true if there is coverage.
		*/
		bool hitTest(int px, int py, RayHit & finalHit);

		/** Test if the UV-space mesh approximately covers a pixel of the texture map, by sampling a neighborhood in uv-space.
		* \param px pixel x coordinate
		* \param py pixel y coordinate
		* \param hit the hit information if there is coverage
		* \return true if there is coverage.
		*/
		bool sampleNeighborhood(int px, int py, RayHit& hit);

		/** Compute the interpolated position and normal at the intersection point on the initial mesh.
		* \param hit the intersection information
		* \param vertex will contain the interpolated position
		* \param normal will contain the interpolated normal
		*/
		void interpolate(const sibr::RayHit & hit, sibr::Vector3f & vertex, sibr::Vector3f & normal) const;

		sibr::ImageRGB32F _accum; ///< Color accumulator.
		sibr::ImageL8 _mask; ///< Mask indicating which regions of the texture map have been covered.

		sibr::Mesh::Ptr _mesh; ///< The original world-space mesh.
		sibr::Raycaster _worldRaycaster; ///< The world-space mesh raycaster.
		sibr::Raycaster _uvsRaycaster; ///< The uv-space mesh raycaster.


	};

}

#endif // MESH_TEXTURING_H