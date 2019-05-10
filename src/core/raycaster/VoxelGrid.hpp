#pragma once


# include <vector>
#include <core/raycaster/Ray.hpp>

# include <core/raycaster/Config.hpp>
#include <core/graphics/Mesh.hpp>

namespace sibr
{
	class SIBR_RAYCASTER_EXPORT VoxelGridBase {
		SIBR_CLASS_PTR(VoxelGridBase);

	public:

		typedef Eigen::AlignedBox<float, 3> Box;

		VoxelGridBase(const Box & boundingBox, int numPerDim, bool forceCube = true);

		bool isInside(const sibr::Vector3f & worldPos) const;
		int getNumCells() const;

		const sibr::Vector3i & getDims() const;

		sibr::Vector3i getCell(size_t cellId) const;
		size_t getCellId(const sibr::Vector3i & cell) const;
		
		sibr::Vector3f getCellCenter(const sibr::Vector3i & cell) const;

		std::vector<size_t> rayMarch(const Ray & ray) const;

		sibr::Mesh::Ptr getCellMesh(const sibr::Vector3i & cell) const;
		sibr::Mesh::Ptr getAllCellMesh() const;

	public:
		static int getMinIndex(const sibr::Vector3f & v);
		
		sibr::Vector3i getCell(const sibr::Vector3f & worldPos) const;
		
		bool intersectionWithBox(const Ray & ray, sibr::Vector3f & intersection) const;
		const sibr::Vector3f & getCellSize() const;
		

		sibr::Vector3i dims;
		sibr::Vector3f cellSize;
		Box box;
		sibr::Mesh::Ptr baseCellMesh;
	};


	template<typename CellType> class VoxelGrid : public VoxelGridBase {

		SIBR_CLASS_PTR(VoxelGrid);

	public:

		VoxelGrid(const Box & boundingBox, int numPerDim) : VoxelGridBase(boundingBox, numPerDim) {
			data.resize(getNumCells());
		}

		CellType & operator[](int cell_id) {
			return data[cell_id];
		}

		const CellType & operator[](size_t cell_id) const {
			return data[cell_id];
		}

		CellType & operator()(int x, int y, int z) {
			return data[getCellId({ x,y,z })];
		}

		const CellType & operator()(int x, int y, int z) const {
			return data[getCellId({ x,y,z })];
		}

		CellType & operator[](const sibr::Vector3i & v) {
			return data[getCellId(v)];
		}

		const CellType & operator[](const sibr::Vector3i & v) const {
			return data[getCellId(v)];
		}

	protected:
		std::vector<CellType> data;
		//std::vector<int> mapping;
	};


} // namespace sibr

