#pragma once

# include "Config.hpp"
# include "core/system/Matrix.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Mesh.hpp"
# include <core/graphics/Camera.hpp>
#include <core/assets/InputCamera.hpp>

namespace sibr
{

	/** Helpers for rendering basic debug objects (cameras, simple meshes,...)
	 *\note Shaders will be lazily initialized.
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT ViewUtils
	{
	public:

		/** Render a basic wireframe pyramidal camera stub.
		\param eye current viewpoint
		\param cam camera to visualize as a stub
		\param color the wireframe color
		\param scaling the pyramid scaling
		*/
		void renderCamStub(const Camera & eye, const Camera & cam, const Vector3f & color = Vector3f(0,1,0), float scaling = 50.0f);

		/** Render a camera precise frustum
		\param eye current viewpoint
		\param cam camera to visualize as a stub
		\param color the wireframe color
		\param alpha the wireframe opacity
		\param mode rendering mode (filled, wireframe,...)
		\param near near value to use for the frustum
		\param far far value to use for the frustum
		*/
		void renderCamFrustum(const Camera & eye, const InputCamera & cam, const Vector3f & color = Vector3f(1, 0, 0), float alpha = 1.0f, Mesh::RenderMode mode = sibr::Mesh::LineRenderMode,
			float near = -1.0f, float far = -1.0f );

		/** Render a mesh with some basic options.
		\param eye current viewpoint
		\param mesh the geometry to render
		\param color constant color to use
		\param alpha opacity to use
		\param options rendnering options (depth test, render mode,...)
		\sa RenderingOptions
		*/
		void renderMeshWithColorAndAlpha(
			const Camera & eye,
			const sibr::Mesh & mesh,
			const sibr::Vector3f & color = { 0,0,1 },
			float alpha = 1.0f,
			const RenderingOptions & options = RenderingOptions() 
		);

		/** Render a specific triangle from a mesh.
		\param eye current viewpoint
		\param mesh the source mesh
		\param triId the ID of the triangle to display
		\param color the color to use
		\param alpha the opacity to use
		*/
		void renderTriangle(const InputCamera & eye, const sibr::Mesh & mesh, int triId, const sibr::Vector3f & color = { 0,0,1 },
			float alpha = 1.0f);

	private:
		bool initialized = false; ///< Are the shaders initialized.

		/** Check if the shaders and uniforms are initialized. */
		void checkInit();
		/** Init camera stub shader. */
		void initCamStubShader();
		/** Init camera frustum shader. */
		void initCamFrustumShader();

		sibr::GLShader camStubShader; ///< Shader for the stub.
		sibr::GLParameter mvpStub; ///< Stub shader MVP.
		sibr::GLParameter colorStubGL; ///< Stub shader color.
		sibr::Mesh camStubMesh; ///< Stub mesh.


		sibr::GLShader camFrustumShader; ///< Shader for the frustum.
		sibr::GLuniform<Matrix4f> mvpFrust; ///< Frustum shader MVP.
		sibr::GLuniform<Vector3f> colorFurstGL; ///< Frustum shader color.
		sibr::GLuniform<float> alphaFurstGL; ///< Frustum shader alpha.
		sibr::Mesh camFrustumMesh; ///< Frustum mesh.
		sibr::Mesh uniqueTriangle; ///< Mesh containing a unique triangle.
		sibr::Mesh uniqueTriangleBorders; ///< Mesh containing the triangle edges.
	};



} // namespace sibr
