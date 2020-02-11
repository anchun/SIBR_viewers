
#ifndef __SIBR_GRAPHICS_RENDERUTILITY_HPP__
# define __SIBR_GRAPHICS_RENDERUTILITY_HPP__

# include "core/graphics/Config.hpp"
# include "core/system/Matrix.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Mesh.hpp"

namespace sibr
{

	/**
		Helpers for rendering basic debug objects (cameras, simple meshes,...)
		\todo Clarify duplication with functionality in SceneDebugView.
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT RenderUtility
	{
	public:

		/** Create a basic cmaera stub.
		\param camStubSize the stub scale
		\return the mesh
		*/
		static Mesh		createCameraStub( float camStubSize = 0.1f);

		/** Create a screenquad.
		\return the mesh
		*/
		static Mesh		createScreenQuad();

		/** Create a gizmo with X,Y,Z axis and labels (using R,G,B respectively).
		\return the gizmo mesh
		*/
		static Mesh::Ptr createAxisGizmo();

		/** Bind a static VAO for which you can redefine vertices or do vertex pulling. */
		static void		useDefaultVAO( void );

		/** Draw a screenquad. */
		static void		renderScreenQuad();

		/** Draw a screenquad.
		\param reverse should the triangles be flipped
		\param texCoor custom texture coordinates to use.
		*/
		static void		renderScreenQuad( bool reverse, GLfloat texCoor[] = NULL );

	private:

		/** Send vertices to the GPU.
		\param vertTexVBO the VBO id
		\param vert the vertices data
		\param tcoord the UV data
		\param svert the vert size
		\param stcoord the tcoord size
		*/
		static void sendVertsTexToGPU(GLuint vertTexVBO, GLfloat vert[], GLfloat tcoord[], int svert, int stcoord);


	};

	///// DEFINITIONS /////


} // namespace sibr

#endif // __SIBR_GRAPHICS_RENDERUTILITY_HPP__
