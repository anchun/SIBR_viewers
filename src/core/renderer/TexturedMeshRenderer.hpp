
#ifndef __SIBR_EXP_RENDERER_TEXTUREDMESHRENDERER_HPP__
# define __SIBR_EXP_RENDERER_TEXTUREDMESHRENDERER_HPP__

# include <core/graphics/Shader.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Camera.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 
	class SIBR_EXP_RENDERER_EXPORT TexturedMeshRenderer
	{
	public:
		typedef std::shared_ptr<TexturedMeshRenderer>	Ptr;

	public:
		TexturedMeshRenderer(bool flipY = false );

		void	process(
			/*input*/	const Mesh& mesh,
			/*input*/	const Camera& eye,
			/*input*/	uint textureID,
			/*output*/	IRenderTarget& dst,
			bool backfaceCull = true);

		void process(const Mesh & mesh, const Camera & eye, const sibr::Matrix4f & model, uint textureID, IRenderTarget & dst, bool backfaceCull = true);

	private:
		GLShader			_shader;
		GLParameter			_paramMVP;
		GLParameter			_paramTex;
	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDERER_TEXTUREDMESHRENDERER_HPP__
