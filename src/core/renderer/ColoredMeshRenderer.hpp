
#ifndef __SIBR_EXP_RENDERER_COLOREDMESHRENDERER_HPP__
# define __SIBR_EXP_RENDERER_COLOREDMESHRENDERER_HPP__

# include <core/graphics/Shader.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/graphics/Camera.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 
	class SIBR_EXP_RENDERER_EXPORT ColoredMeshRenderer
	{
	public:
		typedef std::shared_ptr<ColoredMeshRenderer>	Ptr;

	public:
		ColoredMeshRenderer( void );

		void	process(
			/*input*/	const Mesh& mesh,
			/*input*/	const Camera& eye,
			/*output*/	IRenderTarget& dst,
			/*mode*/    sibr::Mesh::RenderMode mode = sibr::Mesh::FillRenderMode,
			/*BFC*/     bool backFaceCulling = true);

	private:
		GLShader			_shader;
		GLParameter			_paramMVP;
	};

} /*namespace sibr*/ 

#endif // __SIBR_EXP_RENDERER_COLOREDMESHRENDERER_HPP__
