# include "PositionRender.hpp"

#include <core/view/Resources.hpp>

namespace sibr
{
	PositionRenderer::PositionRenderer(int w,int h)
	{
		_shader.init("positionRendere", 
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("positionRenderer.vert")), 
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("positionRenderer.frag")));

		_MVP.init(_shader,"MVP");
		_RT.reset(new sibr::RenderTargetRGB32F(w,h));
	}

	void PositionRenderer::render( const sibr::Camera& cam, const Mesh& mesh, bool backFaceCulling, bool frontFaceCulling)
	{
		glViewport(0, 0, _RT->w(), _RT->h());
		_RT->bind();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_shader.begin();
		_MVP.set(cam.viewproj());

		mesh.render(true, backFaceCulling, sibr::Mesh::FillRenderMode, frontFaceCulling);

		_shader.end();
		_RT->unbind();
	}

} // namespace