#include <projects/ulr/renderer/TexturedMeshView.hpp>
#include <core/graphics/GUI.hpp>

sibr::TexturedMeshView::TexturedMeshView(const sibr::BasicIBRScene::Ptr & ibrScene, uint render_w, uint render_h) :
	_scene(ibrScene),
	sibr::ViewBase(render_w, render_h)
{
	const uint w = render_w;
	const uint h = render_h;

	//  Renderers.
	_textureRenderer.reset(new TexturedMeshRenderer());
	_poissonRenderer.reset(new PoissonRenderer(w, h));
	_poissonRenderer->enableFix() = true;

	// Rendertargets.
	_poissonRT.reset(new RenderTargetRGBA(w, h, SIBR_CLAMP_UVS));
	_blendRT.reset(new RenderTargetRGBA(w, h, SIBR_CLAMP_UVS));

	// Tell the scene we are a priori using all active cameras.
	std::vector<uint> imgs_ulr;
	const auto & cams = ibrScene->cameras()->inputCameras();
	for(size_t cid = 0; cid < cams.size(); ++cid) {
		if(cams[cid].isActive()) {
			imgs_ulr.push_back(uint(cid));
		}
	}
	_scene->cameras()->debugFlagCameraAsUsed(imgs_ulr);
}

void sibr::TexturedMeshView::setScene(const sibr::BasicIBRScene::Ptr & newScene) {
	_scene = newScene;
	const uint w = getResolution().x();
	const uint h = getResolution().y();

	_textureRenderer.reset(new TexturedMeshRenderer());

	// Tell the scene we are a priori using all active cameras.
	std::vector<uint> imgs_ulr;
	const auto & cams = newScene->cameras()->inputCameras();
	for (size_t cid = 0; cid < cams.size(); ++cid) {
		if (cams[cid].isActive()) {
			imgs_ulr.push_back(uint(cid));
		}
	}
	_scene->cameras()->debugFlagCameraAsUsed(imgs_ulr);
}

//void sibr::TexturedMeshView::setMode(const WeightsMode mode) {
//	_weightsMode = mode;
//	if (_weightsMode == VARIANCE_BASED_W) {
//		_ulrRenderer->setupShaders("ulr_v3_alt");
//	}
//	else if (_weightsMode == ULR_FAST) {
//		_ulrRenderer->setupShaders("ulr_v3_fast");
//	}
//	else {
//		_ulrRenderer->setupShaders();
//	}
//}

void sibr::TexturedMeshView::onRenderIBR(sibr::IRenderTarget & dst, const sibr::Camera & eye)
{
	// Perform ULR rendering, either directly to the destination RT, or to the intermediate RT when poisson blending is enabled.
	_textureRenderer->process(
			_scene->proxies()->proxy(),
			eye, _scene->inputMeshTextures()->handle(), 
			dst, false);

	// Perform Poisson blending if enabled and copy to the destination RT.
	if (_poissonBlend) {
		_poissonRenderer->process(_blendRT, _poissonRT);
		blit(*_poissonRT, dst);
	}

}

void sibr::TexturedMeshView::onUpdate(Input & input)
{
}

void sibr::TexturedMeshView::onGUI()
{
	if (ImGui::Begin("Textured Mesh Renderer Settings")) {

		// Poisson settings.
		ImGui::Checkbox("Poisson ", &_poissonBlend); ImGui::SameLine();
		ImGui::Checkbox("Poisson fix", &_poissonRenderer->enableFix());

	}
	ImGui::End();
}

void sibr::TexturedMeshView::updateCameras(bool allowResetToDefault) {
	// If we are here, the rendering mode or the selected index have changed, we need to update the enabled cameras.
	std::vector<uint> imgs_ulr;
	const auto & cams = _scene->cameras()->inputCameras();

	// Only update if there is at least one camera enabled.
	if(!imgs_ulr.empty()) {
		// Update the shader informations in the renderer.
		//_ulrRenderer->updateCameras(imgs_ulr);
		// Tell the scene which cameras we are using for debug visualization.
		_scene->cameras()->debugFlagCameraAsUsed(imgs_ulr);
	}
	
}
