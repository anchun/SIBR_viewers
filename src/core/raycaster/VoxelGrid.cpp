#include "VoxelGrid.hpp"

namespace sibr {

	VoxelGridBase::VoxelGridBase(const Box & boundingBox, int n, bool forceCube)
		: box(boundingBox), dims(sibr::Vector3i(n,n,n))
	{
		if (forceCube) {
			float maxDimSize = (box.sizes().array() / dims.cast<float>().array()).maxCoeff();
			for (int c = 0; c < 3; ++c) {
				dims[c] = (int)std::round(box.sizes()[c] / maxDimSize);
			}
		}
		cellSize = box.sizes().array() / dims.cast<float>().array();

		std::cout << dims << std::endl;
		std::cout << getCellSize() << std::endl;

		static const sibr::Mesh::Triangles triangles =
		{
			{0,4, 4},
			{5,1, 1},
			{4,5, 5},
			{0,1, 1},
			{2,6, 6},
			{7,3, 3},
			{6,7, 7},
			{2,3, 3},
			{0,2, 2},
			{1,3, 3},
			{4,6, 6},
			{5,7, 7}
		};


		Box baseCell;
		baseCell.extend(box.min());
		baseCell.extend(box.min() + getCellSize());


		sibr::Mesh::Vertices vs(8);
		for (int i = 0; i < 8; ++i) {
			vs[i] = baseCell.corner((Box::CornerType)i);
		}
		
		baseCellMesh.reset(new sibr::Mesh(false));
		baseCellMesh->vertices(vs);
		baseCellMesh->triangles(triangles);
		
	}

	bool VoxelGridBase::isInside(const sibr::Vector3f & worldPos) const
	{
		return box.contains(worldPos);
	}

	int VoxelGridBase::getNumCells() const
	{
		return dims.prod();
	}

	const sibr::Vector3i & VoxelGridBase::getDims() const
	{
		return dims;
	}

	sibr::Vector3i VoxelGridBase::getCell(size_t cellId) const
	{
		sibr::Vector3i cell;
	
		std::div_t div;
		for (int i = 2; i > 0; --i) {
			div = std::div((int)cellId, dims[i]);
			cell[i] = div.rem;
			cellId = div.quot;
		}
		cell[0] = (int)cellId;

		return cell;
	}

	sibr::Vector3i VoxelGridBase::getCell(const sibr::Vector3f & worldPos) const
	{
		sibr::Vector3f posUV = (worldPos - box.min()).array() / (box.sizes().array());
		sibr::Vector3i cellCoord = (dims.cast<float>().array()*posUV.array()).unaryExpr(
			[](float f) { return std::floor(f); }
		).cast<int>();

		//border of last cell is still inside
		//for (int d = 0; d < 3; ++d) {
		//	if (cellCoord[d] == dims[d]) {
		//		--cellCoord[d];
		//	}
		//}

		return cellCoord;
	}

	std::vector<size_t> VoxelGridBase::rayMarch(const Ray & ray) const
	{
		sibr::Vector3f start = ray.orig();

		sibr::Vector3i currentVoxel;
		if (isInside(start)) {
			currentVoxel = getCell(start);
		} else {
			sibr::Vector3f intersection;
			if (intersectionWithBox(ray, intersection)) {
				currentVoxel = getCell(intersection);
				start = intersection;

				//because of the floor function, a pixel exactly at the boundary would be outside
				for (int c = 0; c < 3; c++) {
					if (currentVoxel[c] == dims[c]) {
						--currentVoxel[c];
					}
				}
			} else {
				return {};
			}
		}

		sibr::Vector3i steps = ray.dir().unaryExpr([](float f) { return f >= 0 ? 1 : -1; }).cast<int>();

		const  sibr::Vector3f cellSizes = getCellSize();
		const sibr::Vector3f deltas = cellSizes.array() / ray.dir().cwiseAbs().array();
		const sibr::Vector3f frac = ((start - box.min()).array() / cellSizes.array()).unaryExpr([](float f) { return f - std::floor(f); });
		sibr::Vector3i finalVoxels; 
		sibr::Vector3f ts;
		for (int c = 0; c < 3; c++) {
			ts[c] = deltas[c] * (ray.dir()[c] >= 0 ? 1.0f - frac[c] : frac[c]);
			finalVoxels[c] = (ray.dir()[c] >= 0 ? dims[c] : -1);
		}

		std::vector<size_t> visitedCellsIds;
		while (true) {
			visitedCellsIds.push_back(getCellId(currentVoxel));
		
			int c = getMinIndex(ts);
			currentVoxel[c] += steps[c];
			if (currentVoxel[c] == finalVoxels[c]) {
				break;
			}
			ts[c] += deltas[c];
		}

		return visitedCellsIds;
	}

	sibr::Mesh::Ptr VoxelGridBase::getCellMesh(const sibr::Vector3i & cell) const
	{
		const sibr::Vector3f offset = cell.cast<float>().array()*getCellSize().array();
		
		auto out = std::make_shared<sibr::Mesh>(true);
		out->triangles(baseCellMesh->triangles());
		sibr::Mesh::Vertices vs(8);
		for (int i = 0; i < 8; ++i) {
			vs[i] = baseCellMesh->vertices()[i] + offset;
		}
		out->vertices(vs);
		return out;
	}

	sibr::Mesh::Ptr VoxelGridBase::getAllCellMesh() const
	{
		auto out = std::make_shared<sibr::Mesh>();

		const int numT = baseCellMesh->triangles().size();
		const int numTtotal = getNumCells()*numT;
		const int numV = baseCellMesh->vertices().size();
		const int numVtotal = getNumCells()*numV;
		const sibr::Vector3u offsetT = sibr::Vector3u( numV,numV,numV );

		sibr::Mesh::Vertices vs(numVtotal);
		sibr::Mesh::Triangles ts(numTtotal);
		for (int i = 0; i < getNumCells(); ++i) {
			const auto cell = getCell(i);
			const sibr::Vector3f offsetV = cell.cast<float>().array()*getCellSize().array();
			
			for (int v = 0; v < numV; ++v) {
				vs[i*numV + v] = baseCellMesh->vertices()[v] + offsetV;
			}
			for (int t = 0; t < numT; ++t) {
				ts[i*numT + t] = baseCellMesh->triangles()[t] + i * offsetT;
			}
		}

		out->vertices(vs);
		out->triangles(ts);
		return out;
	}

	bool VoxelGridBase::intersectionWithBox(const Ray & ray, sibr::Vector3f & intersection) const
	{
		//adpated from https://github.com/papaboo/smalldacrt/

		sibr::Vector3f minTs = (box.min() - ray.orig()).array() / ray.dir().array();
		sibr::Vector3f maxTs = (box.max() - ray.orig()).array() / ray.dir().array();

		float nearT = (minTs.cwiseMin(maxTs)).maxCoeff();
		float farT = (minTs.cwiseMax(maxTs)).minCoeff();

		if (nearT <= farT && 0 < farT) {
			intersection = ray.orig() + nearT*ray.dir();
			return true;
		} else {
			return false;
		}
	}

	const sibr::Vector3f & VoxelGridBase::getCellSize() const
	{
		return cellSize;
	}

	size_t VoxelGridBase::getCellId(const sibr::Vector3i & v) const
	{
		return v[2] + dims[2] * (v[1] + dims[1] * v[0]);
	}

	sibr::Vector3f VoxelGridBase::getCellCenter(const sibr::Vector3i & cell) const
	{
		return box.min() + ((0.5f*sibr::Vector3f(1,1,1)+cell.cast<float>()).array()*getCellSize().array()).matrix();
	}

	int VoxelGridBase::getMinIndex(const sibr::Vector3f & v)
	{
		if (v.x() < v.y()) {
			return v.x() < v.z() ? 0 : 2;
		} else {
			return v.y() < v.z() ? 1 : 2;
		}
	}

}