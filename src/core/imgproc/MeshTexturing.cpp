#include "MeshTexturing.hpp"
#include "PoissonReconstruction.hpp"

namespace sibr {

	MeshTexturing::MeshTexturing(unsigned int sideSize) :
		_accum(sideSize, sideSize, Vector3f(0.0f, 0.0f, 0.0f)),
		_mask(sideSize, sideSize, 0)
	{

	}

	void MeshTexturing::setMesh(const sibr::Mesh::Ptr mesh) {
		_mesh = mesh;

		// We need UVs else we can't do anything.
		if (!_mesh->hasTexCoords()) {
			SIBR_ERR << "[Texturing] The mesh to texture needs to have UVs" << std::endl;
			return;
		}
		// We also need normals.
		if (!_mesh->hasNormals()) {
			_mesh->generateNormals();
		}

		// Create a mesh in UV space, using the UVs as 2D coordinates.
		SIBR_LOG << "[Texturing] Generating UV mesh..." << std::endl;
		const int verticesCount = int(_mesh->vertices().size());
		std::vector<sibr::Vector3f> uvVertices(verticesCount);

#pragma omp parallel for
		for (int vid = 0; vid < verticesCount; ++vid) {
			const sibr::Vector2f & uvs = _mesh->texCoords()[vid];
			uvVertices[vid] = sibr::Vector3f(uvs[0], uvs[1], 0.0f);
		}
		Mesh uvMesh(false);
		uvMesh.vertices(uvVertices);
		uvMesh.triangles(_mesh->triangles());

		// Init both raycasters.
		// For the world raycaster, we need to have no backface culling. 
		// Our version of Embree being compiled with backface culling, we have to 'duplicate and flip' the mesh.
		Mesh::Ptr doubleMesh = _mesh->clone();
		doubleMesh->merge(_mesh->invertedFacesMesh());
		_worldRaycaster.addMesh(*doubleMesh);
		_uvsRaycaster.addMesh(uvMesh);

	}

	void MeshTexturing::interpolate(const sibr::RayHit & hit, sibr::Vector3f & vertex, sibr::Vector3f & normal) const {
		const Mesh::Vertices & vertices = _mesh->vertices();
		const Mesh::Normals & normals = _mesh->normals();

		const sibr::Vector3u& tri = _mesh->triangles()[hit.primitive().triID];

		const float uCoord = hit.barycentricCoord().u;
		const float vCoord = hit.barycentricCoord().v;
		const float wCoord = sibr::clamp(1.f - uCoord - vCoord, 0.0f, 1.0f);

		vertex = wCoord * vertices[tri[0]] + uCoord * vertices[tri[1]] + vCoord * vertices[tri[2]];
		normal = (wCoord * normals[tri[0]] + uCoord * normals[tri[1]] + vCoord * normals[tri[2]]).normalized();
	}

	void MeshTexturing::reproject(const std::vector<InputCamera> & cameras, const std::vector<sibr::ImageRGB> & images) {
		// We need a mesh for reprojection.
		if (!_mesh) {
			SIBR_WRG << "[Texturing] No mesh available." << std::endl;
			return;
		}


		struct SampleInfos {
			sibr::Vector3f color;
			float weight;
		};

		SIBR_LOG << "[Texturing] Gathering color samples from " << cameras.size() << " cameras ..." << std::endl;

		const int w = _accum.w();
		const int h = _accum.h();

#pragma omp parallel for
		for (int py = 0; py < h; ++py) {
			for (int px = 0; px < w; ++px) {
				// Check if we fall inside a triangle in the UV map.
				RayHit hit;
				const bool hasHit = sampleNeighborhood(px, py, hit);

				// We really have no triangle in the neighborhood to use, skip.
				if (!hasHit) {
					continue;
				}

				// Need the smooth position and normal in the initial mesh.
				sibr::Vector3f vertex, normal;
				interpolate(hit, vertex, normal);

				sibr::Vector3f avgColor(0.0f, 0.0f, 0.0f);
				float totalWeight = 0.0f;

				std::vector<SampleInfos> samples;

				for (int cid = 0; cid < cameras.size(); ++cid) {
					const auto & cam = cameras[cid];
					if (!cam.frustumTest(vertex)) {
						continue;
					}

					// Check for occlusions.
					sibr::Vector3f occDir = (vertex - cam.position());
					const float dist = occDir.norm();
					if (dist > 0.0f) {
						occDir /= dist;
					}
					const RayHit hitOcc = _worldRaycaster.intersect(Ray(cam.position(), occDir));
					if (hitOcc.hitSomething() && (hitOcc.dist() + 0.0001f) < dist) {
						continue;
					}

					// Reproject, read color.
					const sibr::Vector2f pos = cam.projectImgSpaceInvertY(vertex).xy();
					const sibr::Vector3f col = images[cid].bilinear(pos).cast<float>().xyz();
					// Angle-based weight for now.
					const float angleWeight = std::max(-occDir.dot(normal), 0.0f);
					const float weight = angleWeight;
					//avgColor += weight * col;
					//totalWeight += weight;
					samples.emplace_back();
					samples.back().color = col;
					samples.back().weight = weight;
				}
				if (samples.empty()) {
					continue;
				}
				
				std::sort(samples.begin(), samples.end(), [](const SampleInfos & a, const SampleInfos & b)
				{
					return a.weight < b.weight;
				});

				// Re-weight and accumulate the samples.
				// The code is written this way to support 'best XX% samples' approaches in the future.
				for(int i = 0; i < samples.size(); ++i) {
					float w = samples[i].weight;
					w = w * w;
					totalWeight += w;
					avgColor += w * samples[i].color;
				}

				if (totalWeight > 0.0f) {
					_accum(px, py) = avgColor / totalWeight;
					_mask(px, py)[0] = 255;
				}
			}
		}
	}

	sibr::ImageRGB::Ptr MeshTexturing::getTexture(uint options) const {

		ImageRGB32F output;
		if (options & Options::FLOOD_FILL) {
			output = floodFill(_accum, _mask)->clone();
		}
		else if (options & Options::POISSON_FILL) {
			output = poissonFill(_accum, _mask)->clone();
		}
		else {
			output = _accum.clone();
		}

		// Convert as-is to uchar.
		ImageRGB::Ptr result(new ImageRGB());
		const cv::Mat3f outputF = output.toOpenCV();
		const cv::Mat3b outputB = cv::Mat3b(outputF);
		result->fromOpenCV(outputB);

		/// \todo For extra large images, this might crash because of internal openCV indexing limitations.
		if (options & Options::FLIP_VERTICAL) {
			result->flipH();
		}

		return result;
	}

	sibr::ImageRGB32F::Ptr MeshTexturing::poissonFill(const sibr::ImageRGB32F & image, const sibr::ImageL8 & mask) {
		SIBR_LOG << "[Texturing] Poisson filling..." << std::endl;

		const cv::Mat3f guideF = cv::Mat3f(image.toOpenCV()) / 255.0f;
		cv::Mat1f maskF;
		mask.toOpenCV().convertTo(maskF, CV_32FC1, 1.0f / 255.0f);

		const cv::Mat3f gradX = cv::Mat3f::zeros(guideF.rows, guideF.cols);
		const cv::Mat3f gradY = gradX.clone();

		PoissonReconstruction poisson(gradX, gradY, maskF, guideF);
		poisson.solve();
		const cv::Mat3f resultF = 255.0f * poisson.result();

		ImageRGB32F::Ptr filled(new ImageRGB32F());
		filled->fromOpenCV(resultF);
		return filled;
	}

	sibr::ImageRGB32F::Ptr MeshTexturing::floodFill(const sibr::ImageRGB32F & image, const sibr::ImageL8 & mask) {

		ImageRGB32F::Ptr filled(new ImageRGB32F(image.w(), image.h()));

		SIBR_LOG << "[Texturing] Flood filling..." << std::endl;
		// Perform filling.
		// We need the zeros pixels marked as non zeros.
		cv::Mat1b flipMask = mask.toOpenCV();
		flipMask = 255 - flipMask;
		cv::Mat1f dummyDist(flipMask.rows, flipMask.cols, 0.0f);
		cv::Mat1i labels(flipMask.rows, flipMask.cols, 0);

		// Run distance transform to obtain the IDs.
		cv::distanceTransform(flipMask, dummyDist, labels, cv::DIST_L2, cv::DIST_MASK_5, cv::DIST_LABEL_PIXEL);

		// Build a pixel ID to source pixel table, using the pixels in the mask.
		const sibr::Vector2i basePos(-1, -1);
		std::vector<sibr::Vector2i> colorTable(flipMask.rows*flipMask.cols, basePos);
#pragma omp parallel
		for (int py = 0; py < flipMask.rows; ++py) {
			for (int px = 0; px < flipMask.cols; ++px) {
				if (flipMask(py, px) != 0) {
					continue;
				}
				const int label = labels(py, px);
				colorTable[label] = { px,py };
			}
		}

		// Now we can turn the label image into a color image again.
#pragma omp parallel
		for (int py = 0; py < flipMask.rows; ++py) {
			for (int px = 0; px < flipMask.cols; ++px) {
				// Don't touch existing pixels.
				if (flipMask(py, px) == 0) {
					filled(px, py) = image(px, py);
					continue;
				}
				const int label = labels(py, px);
				filled(px, py) = image(colorTable[label]);
			}
		}
		return filled;
	}

	bool MeshTexturing::hitTest(int px, int py, RayHit & finalHit)
	{
		// From the UVs find the world space position.
		const float u = (float(px) + 0.5f) / float(_accum.w());
		const float v = (float(py) + 0.5f) / float(_accum.h());
		// Spawn a ray from (u,v,0) in the z direction.
		const RayHit hit = _uvsRaycaster.intersect(Ray({ u, v, 1.0f }, { 0.0f,0.0f,-1.0f }));
		if (hit.hitSomething()) {
			finalHit = hit;
			return true;
		}
		// Just in case of backface culling, try the other side.
		const RayHit hitBack = _uvsRaycaster.intersect(Ray({ u, v, -1.0f }, { 0.0f,0.0f,1.0f }));
		if (hitBack.hitSomething()) {
			finalHit = hitBack;
			return true;
		}
		return false;
	}

	bool MeshTexturing::sampleNeighborhood(int px, int py, RayHit & hit)
	{
		bool hasHit = hitTest(px, py, hit);
		// Sample a 3x3 neighborhood to counter-act aliasing/interpolation later on, as long as we don't get a hit.
		// The order is important, to first fetch in line/column and then in diagonal. Sorry for the cache...
		std::vector<int> dxs = { px, px - 1, px + 1 };
		std::vector<int> dys = { py, py - 1, py + 1 };
		for (const int dx : dxs) {
			if (hasHit) {
				break;
			}
			for (const int dy : dys) {
				if (hasHit) {
					break;
				}
				// Ignore center pixel, already tested.
				if (dx == px && dy == py) {
					continue;
				}
				hasHit = hitTest(dx, dy, hit);
			}
		}
		return hasHit;
	}

}