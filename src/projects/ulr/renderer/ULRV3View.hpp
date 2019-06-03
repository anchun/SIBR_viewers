#pragma once

# include "Config.hpp"
# include <core/system/Config.hpp>
# include <core/graphics/Mesh.hpp>
# include <core/view/ViewBase.hpp>
# include <core/renderer/CopyRenderer.hpp>
# include <projects/ulr/renderer/ULRV3Renderer.hpp>
# include <core/renderer/PoissonRenderer.hpp>

namespace sibr { 

	/**
	 * \class ULRV3View
	 * \brief Wrap a ULR renderer with additional parameters and information.
	 */
	class SIBR_EXP_ULR_EXPORT ULRV3View : public sibr::ViewBase
	{
		SIBR_CLASS_PTR(ULRV3View);

		/// Rendering mode: default, use only one camera, use all cameras but one.
		enum RenderMode { ALL_CAMS, ONE_CAM, LEAVE_ONE_OUT, EVERY_N_CAM };

		/// Blending mode: keep the four best values per pixel, or aggregate them all.
		enum WeightsMode { ULR_W , VARIANCE_BASED_W };

	public:

		/**
		 * Constructor
		 * \param ibrScene The scene to use for rendering.
		 */
		ULRV3View(const sibr::BasicIBRScene::Ptr& ibrScene, uint render_w, uint render_h);

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
		const ULRV3Renderer::Ptr & getULRrenderer() const { return _ulrRenderer; }

	protected:

		/**
		 * Update the camera informations in the ULR renderer based on the current rendering mode and selected index.
		 * \param allowResetToDefault If true, when the rendering mode is ALL_CAMS, the cameras information will be updated.
		 */
		void updateCameras(bool allowResetToDefault);

		std::shared_ptr<sibr::BasicIBRScene> _scene;
		ULRV3Renderer::Ptr		_ulrRenderer;
		PoissonRenderer::Ptr	_poissonRenderer;

		RenderTargetRGBA::Ptr	_blendRT;
		RenderTargetRGBA::Ptr	_poissonRT;

		bool					_poissonBlend = false;

		RenderMode				_renderMode = ALL_CAMS;
		WeightsMode				_weightsMode = ULR_W;
		int						_singleCamId = 0;
		int						_everyNCamStep = 1;
	};

} /*namespace sibr*/ 
