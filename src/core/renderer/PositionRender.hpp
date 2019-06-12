#pragma once

#include "Config.hpp"

# include <core/graphics/Shader.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Camera.hpp>

# include <core/renderer/Config.hpp>


namespace sibr
{
	class SIBR_EXP_RENDERER_EXPORT PositionRenderer
	{
		SIBR_CLASS_PTR(PositionRenderer);

	public:
		PositionRenderer(int w,int h);

		void render( const sibr::Camera &cam, const Mesh& mesh, bool backFaceCulling=false, bool frontFaceCulling=false);

		const sibr::RenderTargetRGB32F::Ptr & getPositionsRT() { return _RT; }

		
	private:
		sibr::GLShader							_shader;
		sibr::GLuniform<sibr::Matrix4f>			_MVP;
		sibr::RenderTargetRGB32F::Ptr			_RT;
	};

} // namespace

