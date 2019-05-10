#pragma once

# include "Config.hpp"
# include "core/system/Matrix.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Mesh.hpp"
# include <core/graphics/Camera.hpp>
#include <core/assets/InputCamera.hpp>

namespace sibr
{

	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT ViewUtils
	{
	public:
		ViewUtils();

		void renderCamStub(const Camera & eye, const Camera & cam, const Vector3f & color = Vector3f(0,1,0), float scaling = 50.0f);
		void renderCamFrustum(const Camera & eye, const InputCamera & cam, const Vector3f & color = Vector3f(1, 0, 0), float alpha = 1.0f, Mesh::RenderMode mode = sibr::Mesh::LineRenderMode,
			float near = -1.0f, float far = -1.0f );

		void renderMeshWithColorAndAlpha(
			const Camera & eye,
			const sibr::Mesh & mesh,
			const sibr::Vector3f & color = { 0,0,1 },
			float alpha = 1.0f,
			const RenderingOptions & options = RenderingOptions() 
		);

		void renderTriangle(const InputCamera & eye, const sibr::Mesh & mesh, int triId, const sibr::Vector3f & color = { 0,0,1 },
			float alpha = 1.0f);

	private:
		bool initialized;
		void checkInit();
		void initCamStubShader();
		void initCamFrustumShader();

		sibr::GLShader camStubShader;
		sibr::GLParameter mvpStub;
		sibr::GLParameter colorStubGL;
		sibr::Mesh camStubMesh;


		sibr::GLShader camFrustumShader;
		sibr::GLuniform<Matrix4f> mvpFrust;
		sibr::GLuniform<Vector3f> colorFurstGL;
		sibr::GLuniform<float> alphaFurstGL;
		sibr::Mesh camFrustumMesh;
		sibr::Mesh uniqueTriangle;
		sibr::Mesh uniqueTriangleBorders;
	};

	///// DEFINITIONS /////


} // namespace sibr
