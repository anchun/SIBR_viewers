
#include "Raycaster.hpp"

namespace sibr
{
	/*static*/ SIBR_RAYCASTER_EXPORT const Raycaster::geomId		Raycaster::InvalidGeomId = RTC_INVALID_GEOMETRY_ID;
	/*static*/ bool													Raycaster::g_initRegisterFlag = false;
	/*static*/ Raycaster::RTCDevicePtr								Raycaster::g_device = nullptr;

	/*static*/ void Raycaster::rtcErrorCallback(RTCError code, const char* msg)
	{
		std::string err;

		switch (code)
		{
		case RTC_UNKNOWN_ERROR: err = std::string("RTC_UNKNOWN_ERROR"); break;
		case RTC_INVALID_ARGUMENT: err = std::string("RTC_INVALID_ARGUMENT"); break;
		case RTC_INVALID_OPERATION: err = std::string("RTC_INVALID_OPERATION"); break;
		case RTC_OUT_OF_MEMORY: err = std::string("RTC_OUT_OF_MEMORY"); break;
		case RTC_UNSUPPORTED_CPU: err = std::string("RTC_UNSUPPORTED_CPU"); break;
		case RTC_CANCELLED: err = std::string("RTC_CANCELLED"); break;
		default: err = std::string("invalid error code"); break;
		}

		SIBR_ERR << "Embree reported the following issue - "
			<< "[" << err << "]'" << msg << "'" << std::endl;
	}

	Raycaster::~Raycaster(void)
	{
		_scene = nullptr;
		_devicePtr = nullptr;
		if (g_device && g_device.use_count() == 1)
			g_device = nullptr; // if nobody use it, free it
	}

	bool	Raycaster::init(RTCSceneFlags sceneType, RTCAlgorithmFlags intersectType)
	{
		if (!g_device)
		{
			// The two following macros set flagbits on the control register
			// used by SSE (see http://softpixel.com/~cwright/programming/simd/sse.php)
			_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);			// Enable 'Flush Zero' bit
			_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);	// Enable 'Denormals Zero'bit

			SIBR_LOG << "Initializing Raycaster" << std::endl;

			g_device = std::shared_ptr<__RTCDevice>( // define a new embree device
				rtcNewDevice(NULL), // create it
				rtcDeleteDevice); // set a custom deleter

			if (g_device == nullptr) {
				SIBR_LOG << "Cannot create an embree device : " << rtcDeviceGetError(g_device.get()) << std::endl;
			}

			rtcDeviceSetErrorFunction(g_device.get(), &Raycaster::rtcErrorCallback); // Set callback error function
			_devicePtr = g_device; //Moved in the init
		}

		if (_scene)
			return true;

		_scene = std::shared_ptr<__RTCScene>( // define a new scene
			/// \todo create a new static scene optimized for primary rays (TODO: test perf with RTC_SCENE_ROBUST)
			rtcDeviceNewScene(g_device.get(), sceneType | RTC_SCENE_COHERENT, intersectType | RTC_INTERPOLATE),
			rtcDeleteScene); // set a custom deleter

		if (_scene == nullptr)
			SIBR_LOG << "Cannot create an embree scene" << std::endl;
		else {
			SIBR_LOG << "Embree device and scene created" << std::endl;
			return true; // Success
		}
		return false; // Fail
	}

	Raycaster::geomId	Raycaster::addMesh(const sibr::Mesh& mesh)
	{
		return addGenericMesh(mesh, RTC_GEOMETRY_STATIC);
	}

	Raycaster::geomId	Raycaster::addDynamicMesh(const sibr::Mesh& mesh)
	{
		return addGenericMesh(mesh, RTC_GEOMETRY_DYNAMIC);
	}

	Raycaster::geomId	Raycaster::addGenericMesh(const sibr::Mesh& mesh, RTCGeometryFlags type)
	{
		if (init() == false)
			return Raycaster::InvalidGeomId;

		const sibr::Mesh::Vertices& vertices = mesh.vertices();
		const sibr::Mesh::Triangles& triangles = mesh.triangles();

		geomId id = rtcNewTriangleMesh(_scene.get(),
			type, triangles.size(), vertices.size());

		if (id == Raycaster::InvalidGeomId)
			return Raycaster::InvalidGeomId;

		struct Vertex { float x, y, z, a; };
		struct Triangle { int v0, v1, v2; };

		{ // Fill vertices of the geometry
			Vertex* vert = (Vertex*)rtcMapBuffer(_scene.get(), id, RTC_VERTEX_BUFFER);
			for (uint i = 0; i < mesh.vertices().size(); ++i)
			{
				vert[i].x = vertices[i][0];
				vert[i].y = vertices[i][1];
				vert[i].z = vertices[i][2];
				vert[i].a = 1.f;
			}
			rtcUnmapBuffer(_scene.get(), id, RTC_VERTEX_BUFFER);
		}

		{ // Fill triangle indices of the geometry
			Triangle* tri = (Triangle*)rtcMapBuffer(_scene.get(), id, RTC_INDEX_BUFFER);
			for (uint i = 0; i < triangles.size(); ++i)
			{
				tri[i].v0 = triangles[i][0];
				tri[i].v1 = triangles[i][1];
				tri[i].v2 = triangles[i][2];
			}
			rtcUnmapBuffer(_scene.get(), id, RTC_INDEX_BUFFER);
		}

		// Commit all changes on the scene
		rtcCommit(_scene.get());

		return id;
	}

	// xform a mesh by transformation matrix "mat". Note that the original positions
	// are always stored in mesh.vertices -- we only xform the vertices in the embree buffer
	void	Raycaster::xformRtcMeshOnly(sibr::Mesh& mesh, geomId mesh_id, sibr::Matrix4f& mat, sibr::Vector3f& centerPt, float& maxlen)
	{
		struct Vertex { float x, y, z, a; };
		Vertex* vert = (Vertex*)rtcMapBuffer(_scene.get(), mesh_id, RTC_VERTEX_BUFFER);
		sibr::Vector4f averagePt = sibr::Vector4f(0, 0, 0, 1);
		maxlen = 0;

		const sibr::Mesh::Vertices& vertices = mesh.vertices();
		//const sibr::Mesh::Normals& normals = mesh.normals();
		for (uint i = 0; i < mesh.vertices().size(); ++i)
		{
			sibr::Vector4f v;

			// reset to original position
			v[0] = vert[i].x = vertices[i][0];
			v[1] = vert[i].y = vertices[i][1];
			v[2] = vert[i].z = vertices[i][2];
			v[3] = vert[i].a = 1.f;

			v = mat * v;
			vert[i].x = v[0], vert[i].y = v[1], vert[i].z = v[2];
			averagePt += v;
			float d = sibr::Vector3f(sibr::Vector4f(averagePt / (float)((i == 0) ? 1 : i)).xyz() - v.xyz()).norm();
			if (d > maxlen)
				maxlen = d;
		}

		sibr::Vector4f cp = averagePt / (float)mesh.vertices().size();
		centerPt = sibr::Vector3f(cp[0], cp[1], cp[2]);
		rtcUnmapBuffer(_scene.get(), mesh_id, RTC_VERTEX_BUFFER);
		// Update mesh
		rtcUpdate(_scene.get(), mesh_id);
		// Commit changes to scene
		rtcCommit(_scene.get());
	}

	bool	Raycaster::hitSomething(const Ray& inray, float minDist)
	{
		assert(minDist >= 0.f);

		RTCRay ray;
		ray.org[0] = inray.orig()[0];
		ray.org[1] = inray.orig()[1];
		ray.org[2] = inray.orig()[2];
		ray.dir[0] = inray.dir()[0];
		ray.dir[1] = inray.dir()[1];
		ray.dir[2] = inray.dir()[2];

		ray.tnear = minDist;
		ray.tfar = RayHit::InfinityDist;
		ray.geomID = RTC_INVALID_GEOMETRY_ID;

		if (init() == false)
			SIBR_ERR << "cannot initialize embree, failed cast rays." << std::endl;
		else
			rtcOccluded(_scene.get(), ray);
		return ray.geomID == 0;
	}

	std::array<bool, 8>	Raycaster::hitSomething8(const std::array<Ray, 8>& inray, float minDist)
	{
		assert(minDist >= 0.f);

		RTCRay8 ray;
		for (int r = 0; r < 8; r++) {
			ray.orgx[r] = inray[r].orig()[0];
			ray.orgy[r] = inray[r].orig()[1];
			ray.orgz[r] = inray[r].orig()[2];
			ray.dirx[r] = inray[r].dir()[0];
			ray.diry[r] = inray[r].dir()[1];
			ray.dirz[r] = inray[r].dir()[2];

			ray.tnear[r] = minDist;
			ray.tfar[r] = RayHit::InfinityDist;
			ray.geomID[r] = RTC_INVALID_GEOMETRY_ID;
		}

		int valid8[8] = { -1,-1,-1,-1, -1, -1, -1, -1 };
		if (init() == false)
			SIBR_ERR << "cannot initialize embree, failed cast rays." << std::endl;
		else
			rtcOccluded8(valid8, _scene.get(), ray);

		std::array<bool, 8> res;
		for (int r = 0; r < 8; r++) {
			bool hit = (ray.geomID[r] == 0);
			res[r] = hit;
		}

		return res;
	}

	RayHit	Raycaster::intersect(const Ray& inray, float minDist)
	{
		assert(minDist >= 0.f);

		RTCRay ray;
		ray.org[0] = inray.orig()[0];
		ray.org[1] = inray.orig()[1];
		ray.org[2] = inray.orig()[2];
		ray.dir[0] = inray.dir()[0];
		ray.dir[1] = inray.dir()[1];
		ray.dir[2] = inray.dir()[2];

		ray.tnear = minDist;
		ray.tfar = RayHit::InfinityDist;
		ray.geomID = RTC_INVALID_GEOMETRY_ID;

		if (init() == false)
			SIBR_ERR << "cannot initialize embree, failed cast rays." << std::endl;
		else
			rtcIntersect(_scene.get(), ray);

		// Convert to the RayHit struct (used for abstract embree)

		RayHit::Primitive prim;
		prim.geomID = ray.geomID;
		prim.instID = ray.instID;
		prim.triID = ray.primID;

		RayHit::BCCoord coord;
		coord.u = ray.u;
		coord.v = ray.v;

		sibr::Vector3f normal = sibr::Vector3f(ray.Ng[0], ray.Ng[1], ray.Ng[2]);

		// Return the result.
		return RayHit(inray, ray.tfar, coord, normal, prim);
	}

	std::array<RayHit, 8>	Raycaster::intersect8(const std::array<Ray, 8>& inray, float minDist)
	{
		assert(minDist >= 0.f);

		RTCRay8 ray;
		for (int r = 0; r < 8; r++) {
			ray.orgx[r] = inray[r].orig()[0];
			ray.orgy[r] = inray[r].orig()[1];
			ray.orgz[r] = inray[r].orig()[2];
			ray.dirx[r] = inray[r].dir()[0];
			ray.diry[r] = inray[r].dir()[1];
			ray.dirz[r] = inray[r].dir()[2];

			ray.tnear[r] = minDist;
			ray.tfar[r] = RayHit::InfinityDist;
			ray.geomID[r] = RTC_INVALID_GEOMETRY_ID;
		}

		int valid8[8] = { -1,-1,-1,-1, -1, -1, -1, -1 };
		if (init() == false)
			SIBR_ERR << "cannot initialize embree, failed cast rays." << std::endl;
		else
			rtcIntersect8(valid8, _scene.get(), ray);



		std::array<RayHit, 8> res = { {
		{ inray[0],ray.tfar[0],RayHit::BCCoord{ ray.u[0],ray.v[0] },sibr::Vector3f(ray.Ngx[0],ray.Ngy[0],ray.Ngz[0]),RayHit::Primitive{ (uint)ray.primID[0] ,(uint)ray.geomID[0],(uint)ray.instID[0] } },
		{ inray[1],ray.tfar[1],RayHit::BCCoord{ ray.u[1],ray.v[1] },sibr::Vector3f(ray.Ngx[1],ray.Ngy[1],ray.Ngz[1]),RayHit::Primitive{ (uint)ray.primID[1] ,(uint)ray.geomID[1],(uint)ray.instID[1] } },
		{ inray[2],ray.tfar[2],RayHit::BCCoord{ ray.u[2],ray.v[2] },sibr::Vector3f(ray.Ngx[2],ray.Ngy[2],ray.Ngz[2]),RayHit::Primitive{ (uint)ray.primID[2] ,(uint)ray.geomID[2],(uint)ray.instID[2] } },
		{ inray[3],ray.tfar[3],RayHit::BCCoord{ ray.u[3],ray.v[3] },sibr::Vector3f(ray.Ngx[3],ray.Ngy[3],ray.Ngz[3]),RayHit::Primitive{ (uint)ray.primID[3] ,(uint)ray.geomID[3],(uint)ray.instID[3] } },
		{ inray[4],ray.tfar[4],RayHit::BCCoord{ ray.u[4],ray.v[4] },sibr::Vector3f(ray.Ngx[4],ray.Ngy[4],ray.Ngz[4]),RayHit::Primitive{ (uint)ray.primID[4] ,(uint)ray.geomID[4],(uint)ray.instID[4] } },
		{ inray[5],ray.tfar[5],RayHit::BCCoord{ ray.u[5],ray.v[5] },sibr::Vector3f(ray.Ngx[5],ray.Ngy[5],ray.Ngz[5]),RayHit::Primitive{ (uint)ray.primID[5] ,(uint)ray.geomID[5],(uint)ray.instID[5] } },
		{ inray[6],ray.tfar[6],RayHit::BCCoord{ ray.u[6],ray.v[6] },sibr::Vector3f(ray.Ngx[6],ray.Ngy[6],ray.Ngz[6]),RayHit::Primitive{ (uint)ray.primID[6] ,(uint)ray.geomID[6],(uint)ray.instID[6] } },
		{ inray[7],ray.tfar[7],RayHit::BCCoord{ ray.u[7],ray.v[7] },sibr::Vector3f(ray.Ngx[7],ray.Ngy[7],ray.Ngz[7]),RayHit::Primitive{ (uint)ray.primID[7] ,(uint)ray.geomID[7],(uint)ray.instID[7] } },
			} };

		return res;
	}

	sibr::Vector3f Raycaster::smoothNormal(const sibr::Mesh & mesh, const RayHit & hit)
	{
		if (!mesh.hasNormals()) {
			SIBR_ERR << " cannot compute smoothed normals if the mesh does not have normals " << std::endl;
		}
		const sibr::Mesh::Normals& normals = mesh.normals();
		const sibr::Vector3u& tri = mesh.triangles()[hit.primitive().triID];

		float ucoord = hit.barycentricCoord().u;
		float vcoord = hit.barycentricCoord().v;
		float wcoord = 1.f - ucoord - vcoord;
		wcoord = (wcoord >= 0.0f ? (wcoord <= 1.0f ? wcoord : 1.0f) : 0.0f);

		return (wcoord*normals[tri[0]] + ucoord * normals[tri[1]] + vcoord * normals[tri[2]]).normalized();
	}

	sibr::Vector3f Raycaster::smoothColor(const sibr::Mesh & mesh, const RayHit & hit)
	{
		if (!mesh.hasColors()) {
			SIBR_ERR << " cannot compute smoothed color if the mesh does not have colors " << std::endl;
		}
		const sibr::Mesh::Colors& colors = mesh.colors();
		const sibr::Vector3u& tri = mesh.triangles()[hit.primitive().triID];

		float ucoord = hit.barycentricCoord().u;
		float vcoord = hit.barycentricCoord().v;
		float wcoord = 1.f - ucoord - vcoord;
		wcoord = (wcoord >= 0.0f ? (wcoord <= 1.0f ? wcoord : 1.0f) : 0.0f);

		return wcoord * colors[tri[0]] + ucoord * colors[tri[1]] + vcoord * colors[tri[2]];
	}

} // namespace sibr
