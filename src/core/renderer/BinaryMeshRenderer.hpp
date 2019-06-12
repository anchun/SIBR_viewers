#pragma once

# include <core/graphics/Shader.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/graphics/Texture.hpp>
# include <core/graphics/Camera.hpp>

# include <core/renderer/Config.hpp>

namespace sibr { 
	class SIBR_EXP_RENDERER_EXPORT BinaryMeshRenderer
	{
		SIBR_CLASS_PTR(BinaryMeshRenderer);

	public:
		BinaryMeshRenderer();

		void	process(
			/*input*/	const Mesh& mesh,
			/*input*/	const Camera& eye,
			/*output*/	IRenderTarget& dst );

		float & getEpsilon() {
			return epsilon.get();
		}

	private:
		GLShader					_shader;
		GLuniform<sibr::Matrix4f>	_paramMVP;
		GLuniform<float>			epsilon = 0;
	};

} /*namespace sibr*/ 
