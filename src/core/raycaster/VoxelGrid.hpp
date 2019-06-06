#pragma once


# include <vector>
#include <random>

# include <core/raycaster/Config.hpp>

#include <core/raycaster/Ray.hpp>
#include <core/graphics/Mesh.hpp>

namespace sibr
{
	sibr::Vector3f orthoVector(const sibr::Vector3f & v);


	struct BasicVoxelType {
		operator bool() const { return used; }
		bool used = true;
	};

	template<typename T, unsigned int N>
	class SimpleRNG {

	public:
		SimpleRNG(double a = 0, double b = 1) : generator(0), distribution(a,b) { }

		sibr::Vector<T,N> sample() { 
			sibr::Vector<T, N> out;
			for (unsigned int i = 0; i < N; ++i) {
				out[i] = (T)distribution(generator);
			}
			return out;
		}

		std::mt19937 generator;
		std::uniform_real_distribution<> distribution;
	};

	using Box3f = Eigen::AlignedBox<float, 3>;

	class SIBR_RAYCASTER_EXPORT VoxelGridBase {
		SIBR_CLASS_PTR(VoxelGridBase);

	public:

		typedef Eigen::AlignedBox<float, 3> Box;

		VoxelGridBase(const Box & boundingBox, int numPerDim, bool forceCube = true);
		VoxelGridBase(const Box & boundingBox, const sibr::Vector3i & numsPerDim, bool forceCube = true);

		bool isInside(const sibr::Vector3f & worldPos) const;
		bool outOfBounds(const sibr::Vector3i & cell) const;

		size_t getNumCells() const;

		const sibr::Vector3i & getDims() const;

		sibr::Vector3i getCell(size_t cellId) const;
		size_t getCellId(const sibr::Vector3i & cell) const;
		size_t getCellId(const sibr::Vector3f & world_pos) const;

		sibr::Vector3f getCellCenter(const sibr::Vector3i & cell) const;
		sibr::Vector3f getCellCenter(size_t cellId) const;

		std::vector<size_t> rayMarch(const Ray & ray) const;

		sibr::Mesh::Ptr getCellMesh(const sibr::Vector3i & cell) const;
		sibr::Mesh::Ptr getAllCellMesh() const;

		sibr::Mesh::Ptr getCellMeshFilled(const sibr::Vector3i & cell) const;
		sibr::Mesh::Ptr getAllCellMeshFilled() const;

		Box3f getCellBox(size_t cellId) const;

		std::vector<size_t> getNeighbors(size_t cellId) const;

		VoxelGridBase extend(int numCells) const;

		const Box & getBBox() const { return box; }

	public:
		static int getMinIndex(const sibr::Vector3f & v);
		
		sibr::Vector3i getCell(const sibr::Vector3f & worldPos) const;
		sibr::Vector3i getCellInclusive(const sibr::Vector3f & worldPos) const;

		bool intersectionWithBox(const Ray & ray, sibr::Vector3f & intersection) const;
		const sibr::Vector3f & getCellSize() const;
		float getCellSizeNorm() const;

		sibr::Vector3f sampleCell(size_t cellId);

	protected:
		sibr::Mesh::Ptr getCellMeshInternal(const sibr::Vector3i & cell, bool filled) const;
		sibr::Mesh::Ptr getAllCellMeshInternal(bool filled) const;

		sibr::Vector3i dims;
		sibr::Vector3f cellSize;
		float cellSizeNorm;
		Box box;
		sibr::Mesh::Ptr baseCellMesh, baseCellMeshFilled;

		SimpleRNG<float, 3> rng = { -1,1 };

	};


	template<typename CellType = BasicVoxelType> class VoxelGrid : public VoxelGridBase {

		SIBR_CLASS_PTR(VoxelGrid);
	public:
		using VoxelType = CellType;

	public:

		VoxelGrid(const Box & boundingBox, int numPerDim, bool forceCube = true)
			: VoxelGrid(boundingBox, sibr::Vector3i(numPerDim, numPerDim, numPerDim) , forceCube)
		{
		}

		VoxelGrid(const Box & boundingBox, const sibr::Vector3i & numsPerDim, bool forceCube = true)
		: VoxelGridBase(boundingBox, numsPerDim, forceCube) {
			//std::cout << "GridBase : " << dims << " " << getCellSize() << std::endl;
			data.resize(getNumCells());
		}

		CellType & operator[](size_t cell_id) {
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

		template<typename FuncType>
		sibr::Mesh::Ptr getAllCellMeshWithCond(
			bool filled,
			const FuncType & func
		) const;
		
		//const std::vector<sibr::Vector3i> & getUsedCells();

		//void updateUsedCells() {
		//	used_cell_updated = false;
		//}

		template<typename FuncType>
		std::vector<std::size_t> detect_non_empty_cells(const FuncType & func) const;

		//template<typename FuncType>
		//void fix_non_empty_cells(const FuncType & func);

		//const std::vector<std::size_t> & getNonEmptyCellsIds() const {
		//	return non_empty_cells_ids;
		//}

		const std::vector<CellType> & getData() const {
			return data;
		}

	protected:
		//void computeUsedCells();
		//std::vector<sibr::Vector3i> used_cells;
		//bool used_cell_updated = false;

		std::vector<CellType> data;
		
		//std::vector<std::size_t> non_empty_cells_ids;
		
		//std::vector<int> mapping;
	};


	template<typename CellType> template<typename FuncType>
	inline std::vector<std::size_t> VoxelGrid<CellType>::detect_non_empty_cells(const FuncType & func) const {
		std::vector<std::size_t> out_ids;
		for (size_t i = 0; i < data.size(); ++i) {
			if (func(data[i])) {
				out_ids.push_back(i);
			}
		}
		return out_ids;
	}

	//template<typename CellType> template<typename FuncType>
	//inline void VoxelGrid<CellType>::fix_non_empty_cells(const FuncType & func) {
	//	non_empty_cells_ids = detect_non_empty_cells(func);
	//}

	template<typename CellType> template<typename FuncType>
	inline sibr::Mesh::Ptr VoxelGrid<CellType>::getAllCellMeshWithCond(bool filled, const FuncType & f) const
	{
		std::vector<std::size_t> cell_ids = detect_non_empty_cells(f);
		
		//std::cout << "ratio ketp : " << cell_ids.size() / (double)data.size() << std::endl;

		int numNonZero = (int)cell_ids.size();

		auto out = std::make_shared<sibr::Mesh>();

		sibr::Mesh::Ptr baseMesh = filled ? baseCellMeshFilled : baseCellMesh;

		const int numT = (int)baseMesh->triangles().size();
		const int numTtotal = numNonZero *numT;
		const int numV = (int)baseMesh->vertices().size();
		const int numVtotal = numNonZero *numV;
		const sibr::Vector3u offsetT = sibr::Vector3u(numV, numV, numV);

		sibr::Mesh::Vertices vs(numVtotal);
		sibr::Mesh::Triangles ts(numTtotal);
		for (int i = 0; i < numNonZero; ++i ) {
			const auto cell = getCell(cell_ids[i]);
			const sibr::Vector3f offsetV = cell.cast<float>().array()*getCellSize().array();

			for (int v = 0; v < numV; ++v) {
				vs[i*numV + v] = baseMesh->vertices()[v] + offsetV;
			}
			for (int t = 0; t < numT; ++t) {
				ts[i*numT + t] = baseMesh->triangles()[t] + i * offsetT;
			}
		}

		out->vertices(vs);
		out->triangles(ts);
		return out;
	}


} // namespace sibr

