
#ifndef __SIBR_GRAPHICS_RENDERUTILITY_HPP__
# define __SIBR_GRAPHICS_RENDERUTILITY_HPP__

# include "core/graphics/Config.hpp"
# include "core/system/Matrix.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Mesh.hpp"

namespace sibr
{

	/**
	* \ingroup sibr_graphics
	*/
	class SIBR_GRAPHICS_EXPORT RenderUtility
	{
	public:

		RenderUtility() { _camStubSize = 0.1f;  }

		static Mesh		createCameraStub( void );
		static Mesh		createScreenQuad( void );
		static Mesh		createAxisGizmo( void );

		static Mesh::Ptr createAxisGizmoPtr();

		static void  setCamStubDrawSize(float size) { _camStubSize = size; }
		static float  camStubDrawSize() { return _camStubSize; }

		static void		useDefaultVAO( void );

		static void		renderScreenQuad();

		static void		renderScreenQuad( bool reverse, GLfloat texCoor[] = NULL );

		static void sendVertsTexToGPU(GLuint vertTexVBO, GLfloat vert[], GLfloat tcoord[], int svert, int stcoord);

	private:
		static float _camStubSize;

	};

	///// DEFINITIONS /////


} // namespace sibr

#endif // __SIBR_GRAPHICS_RENDERUTILITY_HPP__
