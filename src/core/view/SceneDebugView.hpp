
#ifndef __SIBR_ASSETS_SCENEDEBUGVIEW_HPP__
# define __SIBR_ASSETS_SCENEDEBUGVIEW_HPP__

# include "core/assets/InputCamera.hpp"
# include "core/assets/Config.hpp"
# include "core/assets/CameraRecorder.hpp"
# include "core/graphics/Texture.hpp"
# include "core/graphics/Camera.hpp"
# include "core/graphics/Window.hpp"
# include "core/graphics/RenderUtility.hpp"
# include "core/view/Resources.hpp"
# include "core/graphics/Shader.hpp"
# include "core/graphics/Mesh.hpp"
# include "core/view/InteractiveCameraHandler.hpp"
# include "core/view/ViewBase.hpp"
# include "core/view/FPSCamera.hpp"
# include "core/view/BasicIBRScene.hpp"
# include "core/system/CommandLineArgs.hpp"
#include "ViewUtils.hpp"

namespace sibr
{
	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT SceneDebugView : public ViewBase
	{
		SIBR_CLASS_PTR(SceneDebugView)

	public:

		SceneDebugView(const std::shared_ptr<sibr::BasicIBRScene> & scene, const sibr::Viewport & viewport, const sibr::InteractiveCameraHandler::Ptr & camHandler, const sibr::BasicIBRAppArgs & myArgs);

		//SceneDebugView(const std::shared_ptr<sibr::AssetStreamer> & streamer, const sibr::Viewport & viewport);

		virtual void onUpdate(const sibr::Input & input, const float deltaTime, const sibr::Viewport & viewport = Viewport(0.0f,0.0f,0.0f,0.0f) );

		virtual void	onUpdate(Input& input) override;

		virtual void	onRender(Window& win) override;

		virtual void	onRender(const sibr::Viewport & viewport) override;

		virtual void	onRenderIBR(IRenderTarget& /*dst*/, const sibr::Camera& /*eye*/) override  {} ;
		
		virtual void onGUI() override;

		void setCameraScale(float scale);
		
		float getCameraScale();

		void save();

		const sibr::InteractiveCameraHandler & getCamera() const { return _topViewCamera; }
		sibr::InteractiveCameraHandler & getCamera() { return _topViewCamera; }

		void setScene(const std::shared_ptr<sibr::BasicIBRScene> & scene);

	protected:

		void setup();

		struct LabelMesh {
			Mesh::Ptr mesh = nullptr;
			unsigned int splitIndex = 0;
		};

		/// Structure used to store information common to bothe scene and streamer but in different layouts.
		struct CameraInfos {
			CameraInfos(const sibr::InputCamera& cam, uint id, bool highlight)
				: cam(cam),
				id(id),
				highlight(highlight) {
			}

			const sibr::InputCamera & cam;
			uint id = 0;
			bool highlight = false;
		};

		std::shared_ptr<sibr::BasicIBRScene> _scene;
		//std::shared_ptr<sibr::AssetStreamer> _streamer;
		std::vector<CameraInfos> _cameras;

		sibr::GLShader				_topviewShader;
		sibr::GLParameter			_topviewShader_proj;
		sibr::GLParameter			_topviewShader_inv_mv;
		sibr::GLParameter			_topviewShader_lightDir;
		sibr::GLParameter			_topviewShader_hasColor;
		sibr::GLParameter			_topviewShader_hasNormal;

		sibr::Mesh					_camStubMesh;
		sibr::GLShader				_camStubShader;
		sibr::GLParameter			_camStubShaderMVP;
		sibr::GLParameter			_camStubShaderColor;

		sibr::Mesh					_axisGizmoMesh;
		sibr::GLShader				_axisGizmoShader;
		sibr::GLParameter			_axisGizmoShaderMVP;
		bool						_meshInitialized;

		std::map<unsigned int, LabelMesh> 		_labelMeshes;
		sibr::GLShader				_labelShader;
		sibr::GLParameter			_labelShaderPosition;
		sibr::GLParameter			_labelShaderScale;
		sibr::GLParameter			_labelShaderViewport;

		void setupCameraImageShader();
		void renderCameraImages(const sibr::InputCamera & eye, const std::vector<CameraInfos> & cameras, const sibr::Viewport & viewport, const std::vector<GLuint> & textureIds);
		void renderCameraImagesArray(const sibr::InputCamera & eye, const std::vector<CameraInfos> & cameras, const sibr::Viewport & viewport, const sibr::Texture2DArrayRGB::Ptr textureArray);
		

		sibr::GLShader					_cameraImageShader;
		sibr::GLuniform<sibr::Matrix4f>	_cameraImage_mvp;
		sibr::GLuniform<float>			_alphaImage;

		sibr::GLShader					_cameraImageShaderArray;
		sibr::GLuniform<sibr::Matrix4f>	_cameraImage_mvpArray;
		sibr::GLuniform<float>			_alphaImageArray = 0.5f;
		sibr::GLuniform<int>			_sliceArray = 0;

		bool							_showImages;
		int								_snapToImage;
		bool							_showLabels;
		float							_labelScale = 1.0f;

		ViewUtils					_viewUtils;
		float				_cameraScaling;

		sibr::InteractiveCameraHandler _topViewCamera;
		sibr::InteractiveCameraHandler::Ptr _userCurrentCam;

		bool			_focus;
		bool _showGizmo;
		bool _wireframeMode;
		bool _useBasicCameraStub;
		bool _showCamStubs;
		bool _showMesh;

		std::string camera_path;

		ViewUtils viewUtils;
	};

} // namespace

#endif
