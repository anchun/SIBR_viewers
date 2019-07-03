#ifndef MESH_TEXTURING_H
#define MESH_TEXTURING_H

#include "Config.hpp"
#include <core/graphics/Image.hpp>
#include <core/graphics/Mesh.hpp>
#include <core/assets/InputCamera.hpp>
#include "core/raycaster/Raycaster.hpp"


namespace sibr {

	/** \brief 
	 * \ingroup sibr_imgproc
	 */
	class SIBR_IMGPROC_EXPORT MeshTexturing
	{
	public:

		enum Options : uint {
			NONE = 0,
			FLIP_VERTICAL = 1,
			FLOOD_FILL = 2,
			POISSON_FILL = 4
		};

		MeshTexturing(unsigned int sideSize);

		void setMesh(const sibr::Mesh::Ptr mesh);

		void reproject(const std::vector<InputCamera> & cameras, const std::vector<sibr::ImageRGB::Ptr> & images);

		sibr::ImageRGB::Ptr getTexture(uint options = NONE) const;

	private:

		static sibr::ImageRGB32F::Ptr floodFill(const sibr::ImageRGB32F & image, const sibr::ImageL8 & mask);

		static sibr::ImageRGB32F::Ptr poissonFill(const sibr::ImageRGB32F & image, const sibr::ImageL8 & mask);

		bool hitTest(int px, int py, RayHit & finalHit);

		bool sampleNeighborhood(int px, int py, RayHit& hit);

		void interpolate(const sibr::RayHit & hit, sibr::Vector3f & vertex, sibr::Vector3f & normal) const;

		sibr::ImageRGB32F _accum;
		sibr::ImageL8 _mask;

		sibr::Mesh::Ptr _mesh;
		sibr::Raycaster _worldRaycaster;
		sibr::Raycaster _uvsRaycaster;


	};

}

#endif // MESH_TEXTURING_H