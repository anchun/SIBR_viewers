
#pragma once

# include "core/assets/InputCamera.hpp"
# include "core/assets/Config.hpp"
# include "core/assets/CameraRecorder.hpp"
# include "core/graphics/Texture.hpp"
# include "core/graphics/Camera.hpp"
# include "core/graphics/Window.hpp"
# include "core/graphics/RenderUtility.hpp"
# include "core/assets/Resources.hpp"
# include "core/graphics/Shader.hpp"
# include "core/graphics/Mesh.hpp"
# include "core/view/InteractiveCameraHandler.hpp"
# include "core/view/ViewBase.hpp"
# include "core/view/FPSCamera.hpp"
# include "core/scene/BasicIBRScene.hpp"
# include "core/system/CommandLineArgs.hpp"
#include "ViewUtils.hpp"

#include <core/view/MultiMeshManager.hpp>

namespace sibr
{

	//cam self near and far are used if parameters are < 0
	Mesh::Ptr SIBR_VIEW_EXPORT generateCamFrustum(const InputCamera & cam, float znear = -1, float zfar = -1);

	Mesh::Ptr SIBR_VIEW_EXPORT generateCamFrustumColored(const InputCamera & cam, const Vector3f & col, float znear = -1, float zfar = -1);

	Mesh::Ptr SIBR_VIEW_EXPORT generateCamQuadWithUvs(const InputCamera & cam, float dist);

	class SIBR_VIEW_EXPORT ShaderImageArraySlice : public ShaderAlphaMVP {
	public:
		virtual void initShader(const std::string & name, const std::string & vert, const std::string & frag);
		virtual void render(const Camera & eye, const MeshData & data) {}
		void render(const Camera & eye, const MeshData & data, GLuint textureArrayId, int image_id);
	protected:
		GLuniform<int> slice = 1;
	};

	class SIBR_VIEW_EXPORT ShaderImageSlice : public ShaderAlphaMVP {
	public:
		virtual void render(const Camera & eye, const MeshData & data) {}
		void render(const Camera & eye, const MeshData & data, GLuint textureId);
	};

	struct SIBR_VIEW_EXPORT CameraInfos {
		CameraInfos(const InputCamera& cam, uint id, bool highlight);

		const InputCamera & cam;
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

		std::map<unsigned int, LabelMesh> 	_labelMeshes;
		GLShader							_labelShader;
		GLuniform<Vector3f>					_labelShaderPosition;
		GLuniform<float>					_labelShaderScale;
		GLuniform<Vector2f>					_labelShaderViewport;
		float								_labelScale = 1.0f;

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


	class SIBR_VIEW_EXPORT SceneDebugView : public MultiMeshManager, public ImageCamViewer, public LabelsManager
	{
		SIBR_CLASS_PTR(SceneDebugView);

	public:

		enum CameraInfoDisplay { SIZE, FOCAL, FOV_Y, ASPECT };

		SceneDebugView(const std::shared_ptr<BasicIBRScene> & scene, const Viewport & viewport, const InteractiveCameraHandler::Ptr & camHandler, const BasicDatasetArgs & myArgs);


		virtual void onUpdate(Input & input, const float deltaTime, const Viewport & viewport = Viewport(0.0f, 0.0f, 0.0f, 0.0f));

		virtual void onUpdate(Input & input, const Viewport & viewport);

		virtual void onUpdate(Input& input) override;

		virtual void	onRender(Window& win) override;

		virtual void	onRender(const Viewport & viewport) override;

		using MultiMeshManager::onRender;

		virtual void	onRenderIBR(IRenderTarget& /*dst*/, const Camera& /*eye*/) override {};

		virtual void onGUI() override;

		void save();

		const InteractiveCameraHandler & getCamera() const { return camera_handler; }
		InteractiveCameraHandler & getCamera() { return camera_handler; }


		void setScene(const BasicIBRScene::Ptr & scene, bool preserveCamera = false);

		void updateActiveCams(const std::vector<uint> & cams_id);

	protected:
	
		void gui_options();
		void gui_cameras();

		void setup();
		void setupMeshes();

		InteractiveCameraHandler::Ptr _userCurrentCam;

		std::shared_ptr<BasicIBRScene> _scene;
		std::vector<CameraInfos> _cameras;

		CameraInfoDisplay cam_info_option = SIZE;

		std::string camera_path;

		int					_snapToImage = 0;
		int					_camera_id_info_gui = 0;
		bool				_showImages = true;
		bool				_showLabels = false;

	};

} // namespace
