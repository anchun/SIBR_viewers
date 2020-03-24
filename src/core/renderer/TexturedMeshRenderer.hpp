
#ifndef __SIBR_EXP_RENDERER_TEXTUREDMESHRENDERER_HPP__
# define __SIBR_EXP_RENDERER_TEXTUREDMESHRENDERER_HPP__

# include <core/graphics/Shader.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Camera.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 

	/** Render a textured mesh, using per-vertex texture coordinates.
	\ingroup sibr_renderer
	*/
	class SIBR_EXP_RENDERER_EXPORT TexturedMeshRenderer
	{
	public:
		typedef std::shared_ptr<TexturedMeshRenderer>	Ptr;

	public:

		/** Constructor.
		\param flipY if set to true, UV coordinates will be flipped vertically.
		*/
		TexturedMeshRenderer(bool flipY = false );

		/** Render the textured mesh.
		\param mesh the mesh to render (should have UV attribute)
		\param eye the viewpoint to use
		\param textureID handle of the texture to use
		\param dst destination rendertarget
		\param backfaceCull should backface culling be performed
		*/
		void	process(const Mesh& mesh, const Camera& eye, uint textureID, IRenderTarget& dst, bool backfaceCull = true);

		/** Render the textured mesh.
		\param mesh the mesh to render (should have UV attribute)
		\param eye the viewpoint to use
		\param model additional transformation matrix
		\param textureID handle of the texture to use
		\param dst destination rendertarget
		\param backfaceCull should backface culling be performed
		*/
		void process(const Mesh & mesh, const Camera & eye, const sibr::Matrix4f & model, uint textureID, IRenderTarget & dst, bool backfaceCull = true);

	private:

		GLShader			_shader; ///< The texture mesh shader.
		GLParameter			_paramMVP; ///< MVP uniform.
	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDERER_TEXTUREDMESHRENDERER_HPP__
