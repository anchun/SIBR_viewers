#pragma once

# include "Config.hpp"
# include <core/system/Config.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/view/ViewBase.hpp>
# include <core/renderer/CopyRenderer.hpp>
# include <core/renderer/TexturedMeshRenderer.hpp>
# include <core/scene/BasicIBRScene.hpp>
# include <core/renderer/PoissonRenderer.hpp>

namespace sibr { 

	/**
	 * \class ULRV3View
	 * \brief Wrap a ULR renderer with additional parameters and information.
	 */
	class SIBR_EXP_ULR_EXPORT TexturedMeshView : public sibr::ViewBase
	{
		SIBR_CLASS_PTR(TexturedMeshView);


	public:

		/**
		 * Constructor
		 * \param ibrScene The scene to use for rendering.
		 */
		TexturedMeshView(const sibr::BasicIBRScene::Ptr& ibrScene, uint render_w, uint render_h);

		/** Replace the current scene. */
		void setScene(const sibr::BasicIBRScene::Ptr & newScene);

		/**
		 * Perform rendering. Called by the view manager or rendering mode.
		 * \param dst The destination rendertarget.
		 * \param eye The novel viewpoint.
		 */
		void onRenderIBR(sibr::IRenderTarget& dst, const sibr::Camera& eye) override;

		/**
		 * Update inputs (do nothing).
		 * \param input The inputs state.
		 */
		void onUpdate(Input& input) override;

		/**
		 * Update the GUI.
		 */
		void onGUI() override;

		/// Obtain a reference to the renderer.
		const TexturedMeshRenderer::Ptr & getULRrenderer() const { return _textureRenderer; }

		/// Get a ref to the scene.
		const std::shared_ptr<sibr::BasicIBRScene> getScene() const { return _scene; }

	protected:

		/**
		 * Update the camera informations in the ULR renderer based on the current rendering mode and selected index.
		 * \param allowResetToDefault If true, when the rendering mode is ALL_CAMS, the cameras information will be updated.
		 */
		void updateCameras(bool allowResetToDefault);

		std::shared_ptr<sibr::BasicIBRScene> _scene;
		TexturedMeshRenderer::Ptr		_textureRenderer;
		PoissonRenderer::Ptr	_poissonRenderer;

		RenderTargetRGBA::Ptr	_blendRT;
		RenderTargetRGBA::Ptr	_poissonRT;

		bool					_poissonBlend = false;

		int						_singleCamId = 0;
		int						_everyNCamStep = 1;
	};

} /*namespace sibr*/ 
