#ifndef __SIBR_ASSETS_NORMALRENDER_HPP__
#define __SIBR_ASSETS_NORMALRENDER_HPP__

# include <core/renderer/Config.hpp>

# include "core/assets/InputCamera.hpp"
# include "core/graphics/Texture.hpp"
# include "core/graphics/Camera.hpp"
# include "core/graphics/RenderUtility.hpp"
# include "core/view/Resources.hpp"
# include "core/graphics/Shader.hpp"
# include "core/graphics/Mesh.hpp"


namespace sibr
{
	class SIBR_EXP_RENDERER_EXPORT NormalRenderer
	{

	public:
		NormalRenderer(int w,int h, bool generate = true, bool useFloats = false, bool imSpace = false) ;
		~NormalRenderer();

	void render( const sibr::InputCamera &cam, const Mesh& mesh, const Matrix4f &modelMat = Matrix4f::Identity(), bool clear=true);
		void setWH(int w, int h);
		std::shared_ptr<sibr::RenderTargetRGB> _normal_RT;
		std::shared_ptr<sibr::RenderTargetRGBA32F> _normal_RT_32F;

	private:
		sibr::GLShader				_normalShader;
		sibr::GLParameter			_normalShader_proj;
		sibr::GLParameter			_normalShader_view;
		sibr::GLParameter			_normalShader_model;
		sibr::GLParameter			_normalShader_projInv;
		sibr::GLParameter			_normalShader_imSpace;
		bool _generate;
		bool _useFloats;
	};


} // namespace

#endif
