#include <boost/filesystem/path.hpp>
#include <core/system/Vector.hpp>
#include "core/raycaster/CameraRaycaster.hpp"


namespace sibr
{

	/*static*/ void	CameraRaycaster::computePixelDerivatives( const sibr::InputCamera& cam, 
		sibr::Vector3f& dx, sibr::Vector3f& dy, sibr::Vector3f& upLeftOffset )
	{
		sibr::Vector3f dir = cam.dir();
		sibr::Vector3f up = cam.up();
		float aspect = cam.aspect();

		sibr::Vector2f screenWorldSize;
		{ 
			// screenWorldSize.y = 2*tan(fov/2) because screenDist = 1 (indeed
			// we use normalized cam.dir() to build this derivative)
			float heightWorldSize = 2.f*tanf(cam.fovy()/2.f);
			screenWorldSize = sibr::Vector2f( heightWorldSize*aspect, heightWorldSize ); /// \todo TODO simplify here *remove 2.f*
		}

		sibr::Vector3f right = cross(cam.dir(), up);
		sibr::Vector3f rowSize = right*screenWorldSize[0];
		sibr::Vector3f colSize = -up*screenWorldSize[1];

		dx = rowSize / (float)cam.w();
		dy = colSize / (float)cam.h();

		upLeftOffset = dir - rowSize/2.f - colSize/2.f;
		//upLeftOffset = upLeftOffset + dx/2.f + dy/2.f;	// Used to start from the center of a pixel
		upLeftOffset += cam.position();
	}

	void CameraRaycaster::computeClippingPlanes(const sibr::Mesh & mesh, std::vector<sibr::InputCamera>& cams, std::vector<sibr::Vector2f> & nearsFars)
	{
		
		nearsFars.clear();
		sibr::Raycaster raycaster;
		raycaster.init();
		sibr::Mesh::Ptr localMesh = mesh.invertedFacesMesh2();
		raycaster.addMesh(*localMesh);
		SIBR_LOG << " [CameraRaycaster] computeAutoClippingPlanes() : " << std::flush;

		int deltaPix = 15;

		nearsFars.resize(cams.size());

		#pragma omp parallel for
		for (int cam_id = 0; cam_id < (int)cams.size(); ++cam_id) {
			sibr::InputCamera & cam = cams[cam_id];

			sibr::Vector3f dx, dy, upLeftOffset;
			sibr::CameraRaycaster::computePixelDerivatives(cam, dx, dy, upLeftOffset);
			sibr::Vector3f camZaxis = cam.dir().normalized();
			float maxD = -1.0f, minD = -1.0f;

			for (int i = 0; i < (int)cam.h(); i += deltaPix) {
				for (int j = 0; j < (int)cam.w(); j += deltaPix) {
					sibr::Vector3f worldPos = ((float)j + 0.5f)*dx + ((float)i + 0.5f)*dy + upLeftOffset;
					sibr::Vector3f dir = (worldPos - cam.position()).normalized();

					sibr::RayHit hit = raycaster.intersect(sibr::Ray(cam.position(), dir));

					if (!hit.hitSomething()) { continue; }

					float dist = hit.dist();

					float clipDist = dist * std::abs(dir.dot(camZaxis));

					maxD = (maxD<0 || clipDist > maxD ? clipDist : maxD);
					minD = (minD<0 || clipDist < minD ? clipDist : minD);
				}
			}


			float znear = 0.5f*minD;
			float zfar = 2.0f*maxD;

			while (zfar / znear < 100.0f) {
				zfar *= 1.1f;
				znear *= 0.9f;
			}

			cam.znear(znear);
			cam.zfar(zfar);

			nearsFars[cam_id] = sibr::Vector2f(znear, zfar);

			std::cout << cam_id << " " << std::flush;
		}
		std::cout << " done." << std::endl;

		
	}


	sibr::Vector3f CameraRaycaster::computeRayDir( const sibr::InputCamera& cam, const sibr::Vector2f & pixel )
	{
		sibr::Vector3f dx, dy, upLeftOffset;
		CameraRaycaster::computePixelDerivatives(cam, dx, dy, upLeftOffset);

		sibr::Vector3f worldPos = pixel.x()*dx + pixel.y()*dy + upLeftOffset; //at dist 1 from cam center
		return (worldPos - cam.position()).normalized();
	}

	bool	CameraRaycaster::init( void )
	{
		return _raycaster.init();
	}

	void	CameraRaycaster::addMesh( const sibr::Mesh& mesh )
	{
		_raycaster.addMesh(mesh);
	}

	void	CameraRaycaster::castForEachPixel( const sibr::InputCamera& cam, ICameraRaycasterProcessor* processors[], uint nbProcessors, const std::string& optLogMsg )
	{
		//SIBR_PROFILESCOPE;

		// Check there is no NULL process
		for (uint i = 0; i < nbProcessors; ++i)
			if (processors[i] == nullptr)
			SIBR_ERR << "camera-raycaster process NULL detected" << std::endl;

		sibr::Vector3f dx, dy, upLeftOffset;
		CameraRaycaster::computePixelDerivatives(cam, dx, dy, upLeftOffset);

		//sibr::LoadingProgress	progress(cam.w()*cam.h(), optLogMsg);
		(void)optLogMsg;

		// For each pixel of the camera's image
		for (uint py = 0; py < cam.h(); ++py)
		{
			for (uint px = 0; px < cam.w(); ++px)
			{ 
				//progress.walk();
				sibr::Vector3f worldPos = (float)px*dx + (float)py*dy + upLeftOffset;
				// Cast a ray
				sibr::Vector3f dir =  worldPos - cam.position();
				RayHit hit = _raycaster.intersect(Ray( cam.position(), dir));

				for (uint i = 0; i < nbProcessors; ++i)
					processors[i]->onCast(px, py, hit);
			}
		}

	}

} // namespace sibr
