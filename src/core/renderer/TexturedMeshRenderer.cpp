
#include <core/renderer/TexturedMeshRenderer.hpp>

namespace sibr { 
	TexturedMeshRenderer::TexturedMeshRenderer( bool flipY )
	{	
		if(flipY)
		_shader.init("TexturedMesh",
			sibr::loadFile(sibr::getBinDirectory() + "/shaders_rsc/textured_mesh_flipY.vert"),
			sibr::loadFile(sibr::getBinDirectory() + "/shaders_rsc/textured_mesh.frag"));
		else
		_shader.init("TexturedMesh",
			sibr::loadFile(sibr::getBinDirectory() + "/shaders_rsc/textured_mesh.vert"),
			sibr::loadFile(sibr::getBinDirectory() + "/shaders_rsc/textured_mesh.frag"));
		_paramMVP.init(_shader,"MVP");
	}

	void	TexturedMeshRenderer::process(const Mesh& mesh, const Camera& eye, uint textureID, IRenderTarget& dst, bool backfaceCull)
	{
		dst.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_shader.begin();
		_paramMVP.set(eye.viewproj());
		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, textureID);
		mesh.render(true, backfaceCull);
		_shader.end();
		dst.unbind();

	}

	void	TexturedMeshRenderer::process(const Mesh& mesh, const Camera& eye, const sibr::Matrix4f& model, uint textureID, IRenderTarget& dst, bool backfaceCull)
	{
		dst.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_shader.begin();
		_paramMVP.set(sibr::Matrix4f(eye.viewproj() * model));
		glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, textureID);
		mesh.render(true, backfaceCull);
		_shader.end();
		dst.unbind();

	}

} /*namespace sibr*/
