
#include "core/graphics/Shader.hpp"
#include "core/graphics/RenderUtility.hpp"
#include "core/graphics/Window.hpp"
#include <core/view/Resources.hpp>

#include "ViewUtils.hpp"
#include "interface/InterfaceUtils.h"
#include <core/raycaster/CameraRaycaster.hpp>
namespace sibr
{
	ViewUtils::ViewUtils() : initialized(false) {}

	void ViewUtils::renderCamStub(const sibr::Camera & eye, const sibr::Camera & cam, const sibr::Vector3f & color, float scaling)
	{
		checkInit();

		camStubShader.begin();

		Matrix4f scalingM = Matrix4f::Identity();
		scalingM(0, 0) = scalingM(1, 1) = scalingM(2, 2) = scaling;
		mvpStub.set(Matrix4f(eye.viewproj() * cam.model()* scalingM));
		colorStubGL.set(color);
		camStubMesh.render(false, false, Mesh::LineRenderMode);

		camStubShader.end();
	}

	void ViewUtils::renderCamFrustum(const Camera & eye, const InputCamera & cam, const Vector3f & color, float alpha, Mesh::RenderMode mode, float near, float far)
	{

		static bool first = true;
		if (first) {
			camFrustumMesh = sibr::Mesh(true);
			camFrustumMesh.triangles({
				{ 0,1,2 },{ 0,2,3 },{ 4,5,6 },{ 4,6,7 },
				{ 0,1,4 },{ 1,4,5 },
				{ 1,2,5 },{ 2,5,6 },
				{ 2,3,6 },{ 3,6,7 },
				{ 3,0,7 },{ 0,7,4 }
			});
			first = false;
		}

		static sibr::RenderingOptions options;
		options.backFaceCulling = false;
		options.mode = sibr::Mesh::LineRenderMode;

		const std::vector<Vector2f> corners2D = { {0,0}, {cam.w() - 1,0},{cam.w() - 1,cam.h() - 1},{0,cam.h() - 1 } };
		std::vector<Vector3f> dirs;
		for (const auto & c : corners2D) {
			dirs.push_back(sibr::CameraRaycaster::computeRayDir(cam, c + 0.5f*sibr::Vector2f(1,1)));
		}

		float znear = ( near >= 0 ? near : cam.znear() );
		float zfar = ( far >= 0 ? far : cam.zfar() );
		std::vector<Vector3f> vertices;
		for (int k = 0; k < 2; k++) {
			float dist = (k == 0 ? znear : zfar);
			for (const auto & d : dirs) {
				vertices.push_back(cam.position() + dist*d);
			}
		}
		camFrustumMesh.vertices(vertices);
		
		renderMeshWithColorAndAlpha(eye, camFrustumMesh, color, alpha, options);
	}

	void ViewUtils::renderMeshWithColorAndAlpha(const Camera & eye, const sibr::Mesh & mesh, const sibr::Vector3f & color, float alpha, const RenderingOptions & options)
	{
		checkInit();

		camFrustumShader.begin();

		mvpFrust.set(eye.viewproj());
		colorFurstGL.set(color);
		alphaFurstGL.set(alpha);

		mesh.render(options.depthTest, options.backFaceCulling, options.mode, options.frontFaceCulling, options.invertDepthTest);

		camFrustumShader.end();
	}

	void ViewUtils::renderTriangle(const InputCamera & eye, const sibr::Mesh & mesh, int triId, const sibr::Vector3f & color, float alpha)
	{
		const auto & tris = mesh.triangles();
		if (triId < 0 || triId >= (int)tris.size()) {
			return;
		}
		const auto & tri = tris[triId];

		checkInit();

		static bool first = true;
		if (first) {
			uniqueTriangle = sibr::Mesh(true);
			uniqueTriangle.triangles({ sibr::Vector3u(0,1,2) } );
			uniqueTriangleBorders = sibr::Mesh(true);
			uniqueTriangleBorders.triangles({ { 0,0,1 }, { 1,1,2 }, { 2,2,0 } });
			first = false;
		}

		sibr::Mesh::Vertices vertices(3);
		for (int i = 0; i < 3; ++i) {
			vertices[i] = mesh.vertices()[tri[i]];
		}
		uniqueTriangle.vertices(vertices);
		uniqueTriangleBorders.vertices(vertices);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		camFrustumShader.begin();

		mvpFrust.set(eye.viewproj());
		colorFurstGL.set(color);
		alphaFurstGL.set(alpha);
		uniqueTriangle.render(false, false);

		alphaFurstGL.set(1.0f);
		uniqueTriangleBorders.render(false, false, sibr::Mesh::LineRenderMode);

		camFrustumShader.end();

		glDisable(GL_BLEND);
	}

	void ViewUtils::checkInit()
	{
		if (!initialized) {
			initCamStubShader();
			initCamFrustumShader();

			initialized = true;
		}
	}

	void ViewUtils::initCamStubShader()
	{
		camStubShader.init("camstub",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("camstub.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("camstub.fp")));
		mvpStub.init(camStubShader, "MVP");
		colorStubGL.init(camStubShader, "color");
		camStubMesh = sibr::RenderUtility::createCameraStub();
	}

	void ViewUtils::initCamFrustumShader()
	{
		camFrustumShader.init("camFrustum",
			InterfaceUtilities::meshVertexShader,
			InterfaceUtilities::colorAlphaFragmentShader);
		mvpFrust.init(camFrustumShader, "mvp");
		colorFurstGL.init(camFrustumShader, "color");
		alphaFurstGL.init(camFrustumShader, "alpha");

	}


} // namespace sibr
