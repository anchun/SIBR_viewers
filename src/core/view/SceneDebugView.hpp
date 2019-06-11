
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

#include <core/view/MultiMeshManager.hpp>

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

		void setScene(const std::shared_ptr<sibr::BasicIBRScene> & scene, const bool preserveCamera = false);

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


	//cam self near and far are used if parameters are < 0
	sibr::Mesh::Ptr SIBR_VIEW_EXPORT generateCamFrustum(const InputCamera & cam, float znear = -1, float zfar = -1);

	sibr::Mesh::Ptr SIBR_VIEW_EXPORT generateCamFrustumColored(const InputCamera & cam, const Vector3f & col, float znear = -1, float zfar = -1);

	sibr::Mesh::Ptr SIBR_VIEW_EXPORT generateCamQuadWithUvs(const sibr::InputCamera & cam, float dist);

	class SIBR_VIEW_EXPORT ShaderImageArraySlice : public ShaderAlphaMVP {
	public:
		virtual void initShader(const std::string & name, const std::string & vert, const std::string & frag);
		virtual void render(const sibr::Camera & eye, const MeshData & data) {}
		void render(const sibr::Camera & eye, const MeshData & data, GLuint textureArrayId, int image_id);
	protected:
		sibr::GLuniform<int> slice = 1;
	};

	class SIBR_VIEW_EXPORT ShaderImageSlice : public ShaderAlphaMVP {
	public:
		virtual void render(const sibr::Camera & eye, const MeshData & data) {}
		void render(const sibr::Camera & eye, const MeshData & data, GLuint textureId);
	};

	struct SIBR_VIEW_EXPORT CameraInfos {
		CameraInfos(const sibr::InputCamera& cam, uint id, bool highlight);

		sibr::InputCamera cam;
		uint id = 0;
		bool highlight = false;
	};

	struct SIBR_VIEW_EXPORT LabelsManager {
		struct LabelMesh {
			Mesh::Ptr mesh;
			unsigned int splitIndex = 0;
		}; 
		
		void setupLabelsManagerShader();
		void setupLabelsManagerMeshes(const std::vector<InputCamera> & cams);
		void renderLabels(const Camera & eye, const Viewport & vp, const std::vector<CameraInfos> & cams_info);

		std::map<unsigned int, LabelMesh> 		_labelMeshes;
		sibr::GLShader							_labelShader;
		sibr::GLuniform<sibr::Vector3f>			_labelShaderPosition;
		sibr::GLuniform<float>					_labelShaderScale;
		sibr::GLuniform<sibr::Vector2f>			_labelShaderViewport;
		float									_labelScale = 1.0f;

	};

	struct SIBR_VIEW_EXPORT ImageCamViewer {
		void initImageCamShaders();

		void renderImage(const Camera & eye, const InputCamera & cam, const std::vector<RenderTargetRGBA32F::Ptr> rts, int cam_id);
		void renderImage(const Camera & eye, const InputCamera & cam, uint tex2Darray_handle, int cam_id);

		ShaderImageArraySlice		_cameraImageShaderArray;
		ShaderImageSlice			_cameraImageShader;
		float						_alphaImage = 0.5f;
		float						_cameraScaling = 0.8f;
	};


	class SIBR_VIEW_EXPORT TopView : public MultiMeshManager, public ImageCamViewer, public LabelsManager
	{
		SIBR_CLASS_PTR(TopView);

	public:
		TopView(const std::shared_ptr<BasicIBRScene> & scene, const Viewport & viewport, const InteractiveCameraHandler::Ptr & camHandler, const BasicIBRAppArgs & myArgs);

		virtual void onUpdate(Input & input, const float deltaTime, const Viewport & viewport = Viewport(0.0f, 0.0f, 0.0f, 0.0f));

		virtual void onUpdate(Input & input, const Viewport & viewport);

		virtual void onUpdate(Input& input) override;

		virtual void	onRender(Window& win) override;

		virtual void	onRender(const Viewport & viewport) override;

		virtual void	onRenderIBR(IRenderTarget& /*dst*/, const Camera& /*eye*/) override {};

		virtual void onGUI() override;

		void save();

		const sibr::InteractiveCameraHandler & getCamera() const { return camera_handler; }
		sibr::InteractiveCameraHandler & getCamera() { return camera_handler; }


		void setScene(const BasicIBRScene::Ptr & scene, bool preserveCamera = false);

	//	void updateActiveCams(const std::vector<int> & cams_id);

	protected:
	
		void gui_options();
		void gui_cameras();

		void setup();
		void setupMeshes();

		sibr::InteractiveCameraHandler::Ptr _userCurrentCam;

		std::shared_ptr<sibr::BasicIBRScene> _scene;
		std::vector<CameraInfos> _cameras;

		std::string camera_path;

		int					_snapToImage = 0;
		bool				_showImages = true;
		bool				_showLabels = false;

	};

} // namespace

#endif
