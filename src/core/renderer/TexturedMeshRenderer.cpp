
#include <core/renderer/TexturedMeshRenderer.hpp>

namespace sibr { 
	TexturedMeshRenderer::TexturedMeshRenderer( void )
	{
		_shader.init("TexturedMesh",
			sibr::loadFile("./shaders_rsc/textured_mesh.vert"),
			sibr::loadFile("./shaders_rsc/textured_mesh.frag"));
		_paramMVP.init(_shader,"MVP");
	}

	void	TexturedMeshRenderer::process( const Mesh& mesh, const Camera& eye, uint textureID, IRenderTarget& dst )
	{
		dst.bind();
		_shader.begin();
		_paramMVP.set(eye.viewproj());
		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, textureID );
		mesh.render(true, true);
		_shader.end();
		dst.unbind();

	}

	void	TexturedMeshRenderer::process(const Mesh& mesh, const Camera& eye, const sibr::Matrix4f & model, uint textureID, IRenderTarget& dst)
	{
		dst.bind();
		_shader.begin();
		_paramMVP.set(sibr::Matrix4f(eye.viewproj() * model));
		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, textureID);
		mesh.render(true, true);
		_shader.end();
		dst.unbind();

	}

} /*namespace sibr*/ 
