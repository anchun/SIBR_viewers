# include "core/view/SceneDebugView.hpp"
# include "core/graphics/RenderUtility.hpp"
# include "core/graphics/Input.hpp"
# include "core/graphics/GUI.hpp"
#include "ViewUtils.hpp"
#include <core/raycaster/CameraRaycaster.hpp>

#include <sstream>




namespace sibr
{

	SceneDebugView::SceneDebugView(const std::shared_ptr<BasicIBRScene> & scene, const Viewport & viewport, const InteractiveCameraHandler::Ptr & camHandler, const BasicIBRAppArgs & myArgs)
	{
		_scene = scene;
		_userCurrentCam = camHandler;
		_topViewCamera.setup(_scene->proxies()->proxy().clone(), viewport);
		if (!_scene->cameras()->inputCameras().empty()) {
			_topViewCamera.updateView(_scene->cameras()->inputCameras()[0]);
		}
		_topViewCamera.setupInterpolationPath(_scene->cameras()->inputCameras());
		_showImages = true;
		_meshInitialized = true;
		_showMesh = true;
		camera_path = myArgs.dataset_path.get() + "/cameras";
		setup();
	}


	void SceneDebugView::onUpdate(Input & input)
	{
		// Update camera with a fixed timestep.
		onUpdate(input, 1.0f / 60.0f);
	}

	void SceneDebugView::onRender(Window & win)
	{
		// We need no information about the window, we render wherever we are.
		onRender(win.viewport());
	}

	void SceneDebugView::setup() {
		_showGizmo = true;
		_wireframeMode = false;
		_useBasicCameraStub = false;
		_showCamStubs = true;
		_snapToImage = 0;
		_showLabels = false;
		std::string filename;
		
		filename = camera_path + "/topview.txt";

		// check if topview.txt exists
		std::ifstream topViewFile(filename);
		if (topViewFile.good())
		{
			SIBR_LOG << "Loaded saved topview (" << filename << ")." << std::endl;
			// Intialize a temp camera (used to load the saved top view pose) with
			// the current top view camera to get the resolution/fov right.
			InputCamera cam(_topViewCamera.getCamera());
			cam.readFromFile(topViewFile);
			// Apply it to the top view FPS camera.
			_topViewCamera.fromCamera(cam);
		}

		_topviewShader.init("TopviewShader",
			loadFile(Resources::Instance()->getResourceFilePathName("mesh_debugview.vp")),
			loadFile(Resources::Instance()->getResourceFilePathName("mesh_debugview.fp")));

		_topviewShader_proj.init(_topviewShader, "MVP");
		_topviewShader_inv_mv.init(_topviewShader, "invMV");
		_topviewShader_lightDir.init(_topviewShader, "lightDir");
		_topviewShader_hasColor.init(_topviewShader, "hasColor");
		_topviewShader_hasNormal.init(_topviewShader, "hasNormal");
		// Cam stub setup.
		_camStubShader.init("camstub",
			loadFile(Resources::Instance()->getResourceFilePathName("camstub.vp")),
			loadFile(Resources::Instance()->getResourceFilePathName("camstub.fp")));
		_camStubShaderMVP.init(_camStubShader, "MVP");
		_camStubShaderColor.init(_camStubShader, "color");
		_camStubMesh = std::move(RenderUtility::createCameraStub());
		_cameraScaling = 0.8f;

		// Axis gizmo setup.
		_axisGizmoShader.init("axisgizmo",
			loadFile(Resources::Instance()->getResourceFilePathName("axisgizmo.vp")),
			loadFile(Resources::Instance()->getResourceFilePathName("axisgizmo.fp")));
		_axisGizmoShaderMVP.init(_axisGizmoShader, "MVP");
		_axisGizmoMesh = std::move(RenderUtility::createAxisGizmo());

		// Setup labels.
		_labelShader.init("text-imgui",
			loadFile(Resources::Instance()->getResourceFilePathName("text-imgui.vp")),
			loadFile(Resources::Instance()->getResourceFilePathName("text-imgui.fp")));
		_labelShaderPosition.init(_labelShader, "position");
		_labelShaderScale.init(_labelShader, "scale");
		_labelShaderViewport.init(_labelShader, "viewport");

		setupCameraImageShader();

		
		if (_scene) {

			for (const auto & cam : _scene->cameras()->inputCameras()) {
				unsigned int sepIndex = 0;
				_labelMeshes[cam.id()] = {};
				_labelMeshes[cam.id()].mesh = generateMeshForText(std::to_string(cam.id()), sepIndex);
				_labelMeshes[cam.id()].splitIndex = sepIndex;
			}

		}

	}

	void SceneDebugView::setupCameraImageShader()
	{
		const std::string vertexShader =
			SIBR_SHADER(420,
				layout(location = 0) in vec3 in_vertex;
		layout(location = 2) in vec2 in_uv;
		out vec2 out_uv;

		uniform mat4 mvp;

		void main() {
			out_uv = in_uv;
			gl_Position = mvp * vec4(in_vertex, 1.0);
		}
		);

		const std::string fragmentShader =
			SIBR_SHADER(420,
				layout(location = 0) out vec4 out_color;
		layout(binding = 0) uniform sampler2D input_rgb;
		in vec2 out_uv;
		uniform float alpha;
		void main() {
			out_color = vec4(texture(input_rgb, out_uv).xyz, alpha);
		}
		);

		//std::cout << vertexShader << std::endl << fragmentShader << std::endl;

		_cameraImageShader.init("cameraImageShader", vertexShader, fragmentShader);
		_alphaImage.init(_cameraImageShader, "alpha");
		_cameraImage_mvp.init(_cameraImageShader, "mvp");
		_alphaImage = 0.5f;


		const std::string fragmentShaderArray =
			SIBR_SHADER(420,
				layout(location = 0) out vec4 out_color;
		layout(binding = 0) uniform sampler2DArray input_rgbs;
		in vec2 out_uv;
		uniform float alpha;
		uniform int slice;
		void main() {
			vec3 uv_cam = vec3(out_uv, slice);
			out_color = vec4(texture(input_rgbs, uv_cam).xyz, alpha);
		}
		);

		_cameraImageShaderArray.init("cameraImageShaderArray", vertexShader, fragmentShaderArray);
		_cameraImage_mvpArray.init(_cameraImageShaderArray, "mvp");
		_alphaImageArray.init(_cameraImageShaderArray, "alpha");
		_sliceArray.init(_cameraImageShaderArray, "slice");

	}

	void SceneDebugView::onUpdate(const Input & input, const float deltaTime, const Viewport & viewport)
	{
		_topViewCamera.update(input, deltaTime, viewport);
		//Camera stub size
		if (input.key().isActivated(Key::LeftControl) && input.mouseScroll() != 0.0) {
			setCameraScale(std::max(0.01f, getCameraScale() + (float)input.mouseScroll()*0.1f));
		}
		if (input.key().isActivated(Key::LeftControl) && input.key().isReleased(Key::P)) {
			_showGizmo = !_showGizmo;
		}

		if (input.key().isActivated(Key::LeftControl) && input.key().isReleased(Key::Z)) {
			_wireframeMode = !_wireframeMode;
		}

		if (input.key().isReleased(Key::T)) {
			save();
		}


	}

	void SceneDebugView::onGUI() {

		if (ImGui::Begin("Top view settings")) {
			if (ImGui::Button("Save camera")) {
				save();
			}
			ImGui::Checkbox("Show mesh", &_showMesh);
			ImGui::SameLine();
			ImGui::Checkbox("Wireframe", &_wireframeMode);
			ImGui::Checkbox("Show gizmo", &_showGizmo);
			ImGui::PushScaledItemWidth(120);

			ImGui::Separator();
			ImGui::Checkbox("Draw Cam Stubs ", &_showCamStubs);
			if (_showCamStubs) {
				ImGui::SameLine();
				ImGui::InputFloat("Camera scale", &_cameraScaling, 0.1f, 10.0f);
				ImGui::Checkbox("Basic camstub", &_useBasicCameraStub);
			}
			ImGui::Checkbox("Draw labels ", &_showLabels);
			if (_showLabels) {
				ImGui::SameLine();
				ImGui::InputFloat("Label scale", &_labelScale, 0.2f, 10.0f);
			}

			ImGui::Separator();
			ImGui::Checkbox("Draw Input Images ", &_showImages);
			if (_showImages) {
				ImGui::SameLine();
				if (_scene.get() != nullptr && _scene->renderTargets()->getInputRGBTextureArrayPtr().get() != nullptr) {
					ImGui::SliderFloat("Alpha", &_alphaImageArray.get(), 0, 1.0);
				}
				else {
					ImGui::SliderFloat("Alpha", &_alphaImage.get(), 0, 1.0);
				}
				if (_scene && ImGui::InputInt("Snap To", &_snapToImage, 1, 10)) {
					if (_snapToImage < 0) {
						_snapToImage = 0;
					}
					if (_snapToImage >= _cameras.size()) {
						_snapToImage = int(_cameras.size() - 1);
					}
					_topViewCamera.snapToCamera(_snapToImage);
					_showCamStubs = false;
				}

			}
			ImGui::PopItemWidth();
			ImGui::Separator();
		}
		ImGui::End();
		_topViewCamera.fpsCamera().onGUI("Top view settings");

	}

	void SceneDebugView::setCameraScale(float scale) {
		_cameraScaling = scale;
	}

	float SceneDebugView::getCameraScale() {
		return _cameraScaling;
	}

	void SceneDebugView::save() {
		const std::string filename = camera_path + "/topview.txt";
		std::ofstream outfile(filename, std::ios::out | std::ios::trunc);
		std::cerr << "Saving topview camera to topview.txt" << std::endl;
		// save camera view proj matrix
		_topViewCamera.getCamera().writeToFile(outfile);
	}

	void SceneDebugView::onRender(const Viewport & viewport)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Scene debug view");

		glPointSize(2.f);
		viewport.clear(Vector3f(0.8f, 0.8f, 0.8f));

		InputCamera cam = _topViewCamera.getCamera();
		cam.znear(0.0001f);

		const Matrix4f cameraScalingMatrix = (Vector4f(_cameraScaling, _cameraScaling, _cameraScaling, 1.0f)).asDiagonal();
		_topviewShader.begin();
		_topviewShader_proj.set(cam.viewproj());
		const Matrix4f invMV = (cam.invViewproj()).transpose();
		_topviewShader_inv_mv.set(invMV);
		const Vector3f lightDir = -cam.dir().normalized();
		_topviewShader_lightDir.set(lightDir);
		glDepthFunc(GL_LESS);
		glClearDepth(1.0);
		_topviewShader.end();

		const Mesh * globalMesh = nullptr;
		const InputCamera * userCamera = nullptr;

		if (_scene) {
			//Fallback on BasicIBRScene.
			globalMesh = &(_scene->proxies()->proxy());
			/// \todo TODO: avoid clearing as the scene cameras won't change, only some flags.
			_cameras.clear();
			for (const auto & inputCam : _scene->cameras()->inputCameras()) {
				_cameras.emplace_back(inputCam, inputCam.id(), _scene->cameras()->isCameraUsedForRendering(inputCam.id()));
			}
			userCamera = &(_userCurrentCam->getCamera());

		}

		if (globalMesh && _showMesh) {
			if (_wireframeMode) {
				_camStubShader.begin();
				_camStubShaderMVP.set(cam.viewproj());
				_camStubShaderColor.set(Vector3f(0.f, 0.f, 0.f));
				globalMesh->render(true, true, Mesh::LineRenderMode, false);
				_camStubShader.end();
			}
			else {
				_topviewShader.begin();
				_topviewShader_hasColor.set(globalMesh->hasColors());
				_topviewShader_hasNormal.set(globalMesh->hasNormals());
				globalMesh->render(true, true, Mesh::FillRenderMode, false);
				_topviewShader.end();
			}
		}

		if (_showCamStubs) {
			for (const auto & camInfos : _cameras)
			{
				if (!camInfos.cam.isActive()) { continue; }
				const Vector3f color = camInfos.highlight ? Vector3f(0.f, 1.f, 0.f) : Vector3f(0.f, 0.f, 1.f);
				if (_useBasicCameraStub) {
					_viewUtils.renderCamStub(Camera(cam), Camera(camInfos.cam), color, _cameraScaling);
				}
				else {
					_viewUtils.renderCamFrustum(cam, camInfos.cam, color, 1, Mesh::LineRenderMode, 0.0f, _cameraScaling);
				}
			}
			if (_useBasicCameraStub) {
				
				_viewUtils.renderCamStub(Camera( cam), Camera(*userCamera), Vector3f(1.f, 0.f, 0.f), _cameraScaling);
			}
			else {
				_viewUtils.renderCamFrustum(cam, *userCamera, Vector3f(1.f, 0.f, 0.f), 1, Mesh::LineRenderMode, 0.0f, _cameraScaling);
			}
		}

		if (_showImages && _scene) {
			if (_scene->renderTargets()->getInputRGBTextureArrayPtr().get() != nullptr) {
				renderCameraImagesArray(cam, _cameras, viewport, _scene->renderTargets()->getInputRGBTextureArrayPtr());
			}
			else {
				std::vector<GLuint> textureIds;
				int camNum = 0;
				for (const auto & rt : _scene->renderTargets()->inputImagesRT()) {
					if (_cameras[camNum].cam.isActive()) {
						textureIds.push_back(rt->texture());
					}
					else {
						textureIds.push_back(-1);
					}
					camNum++;
				}
				renderCameraImages(cam, _cameras, viewport, textureIds);
			}
		}

		if (_showLabels) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			_labelShader.begin();
			// Bind the ImGui font texture.
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (GLuint)ImGui::GetFont()->ContainerAtlas->TexID);
			_labelShaderViewport.set(viewport.finalWidth(), viewport.finalHeight());

			for (const auto & camInfos : _cameras)
			{
				const auto & inputCam = camInfos.cam;
				if (!inputCam.isActive()) { continue; }
				
				//const StreamCamera::UID uid = camInfos.id;
				/*if (_labelMeshes.count(uid) == 0) {
					continue;
				}*/
				/// Draw the label.
				/// \todo TODO: we could try to use depth testing to have the labels overlap properly.
				/// As the label is put at the position of the camera, the label will intersect with the frustum mesh, causing artifacts.
				/// One way of solving this would be to just shift the label away a bit and enable depth testing (+ GL_LEQUAl for the text).
				const Vector3f camProjPos = cam.project(inputCam.position());
				if (!cam.frustumTest(inputCam.position(), camProjPos.xy())) {
					continue;
				}
				_labelShaderPosition.set(camProjPos);
				const auto & label = _labelMeshes[camInfos.id];
				// Render the background label.
				_labelShaderScale.set(0.8f*_labelScale);
				label.mesh->renderSubMesh(0, label.splitIndex, false, false);
				// Render the text label.
				_labelShaderScale.set(1.0f*_labelScale);
				label.mesh->renderSubMesh(label.splitIndex, label.mesh->triangles().size() * 3, false, false);

			}
			_labelShader.end();
			glDisable(GL_BLEND);
		}

		// Finally draw the axis gizmo.
		if (_showGizmo) {
			_axisGizmoShader.begin();
			_axisGizmoShaderMVP.set(Matrix4f(cam.viewproj()));
			glLineWidth(2.f);
			_axisGizmoMesh.render(false, false, Mesh::LineRenderMode);
			_axisGizmoShader.end();
		}
		// And the trackball.
		_topViewCamera.onRender(viewport);
		glPopDebugGroup();
	}

	void SceneDebugView::renderCameraImages(const InputCamera & eye, const std::vector<CameraInfos> & cameras, const Viewport & viewport, const std::vector<GLuint> & textureIds)
	{
		
		if (textureIds.empty()) {
			return;
		}
		static const Mesh::Triangles quadTriangles = {
			{ 0,1,2 },{ 0,2,3 }
		};
		static const Mesh::UVs quadUVs = {
			{ 0,1 } ,{ 1,1 } ,{ 1,0 } ,{ 0,0 }
		};
		static const Mesh::Triangles contourTriangles = {
			{ 0,0,1 },{ 1,1,2 }, {2,2,3}, {3,3,0}
		};

		Mesh cameraImageQuad(true);
		Mesh cameraImageContour(true);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendEquation(GL_FUNC_ADD);

		viewport.bind();

		CHECK_GL_ERROR;

		cameraImageQuad.triangles(quadTriangles);
		cameraImageQuad.texCoords(quadUVs);
		cameraImageContour.triangles(contourTriangles);

		RenderingOptions options;
		options.depthTest = false;
		options.backFaceCulling = false;
		options.mode = Mesh::LineRenderMode;


		float dist = std::max(_cameraScaling, 1.05f*eye.znear());

		for (int cam_id = 0; cam_id < (int)cameras.size(); ++cam_id) {

			const auto & cam = cameras[cam_id].cam;
			if (!cam.isActive()) { continue; }

			const std::vector<Vector2f> corners2D = { { 0,0 },{ cam.w() - 1,0 },{ cam.w() - 1,cam.h() - 1 },{ 0,cam.h() - 1 } };
			std::vector<Vector3f> dirs;
			for (const auto & c : corners2D) {
				dirs.push_back(CameraRaycaster::computeRayDir(cam, c + 0.5f*Vector2f(1, 1)));
			}

			std::vector<Vector3f> vertices;
			for (const auto & d : dirs) {
				vertices.push_back(cam.position() + _cameraScaling * d);
			}

			cameraImageQuad.vertices(vertices);

			_cameraImageShader.begin();
			_cameraImage_mvp.set(eye.viewproj());
			_alphaImage.send();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureIds[cam_id]);

			cameraImageQuad.render(true, false);
			_cameraImageShader.end();
			if (_useBasicCameraStub) {
				cameraImageContour.vertices(vertices);
				viewUtils.renderMeshWithColorAndAlpha(eye, cameraImageContour, Vector3f(0, 0, 1), 1.0f, options);
			}
		}

		glDisable(GL_BLEND);
	}

	void SceneDebugView::renderCameraImagesArray(const InputCamera & eye, const std::vector<CameraInfos> & cameras, const Viewport & viewport, const Texture2DArrayRGB::Ptr textureArray)
	{
		static const Mesh::Triangles quadTriangles = {
			{ 0,1,2 },{ 0,2,3 }
		};
		static const Mesh::UVs quadUVs = {
			{ 0,1 } ,{ 1,1 } ,{ 1,0 } ,{ 0,0 }
		};
		static const Mesh::Triangles contourTriangles = {
			{ 0,0,1 },{ 1,1,2 },{ 2,2,3 },{ 3,3,0 }
		};

		CHECK_GL_ERROR;

		Mesh cameraImageQuad(true);
		Mesh cameraImageContour(true);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		viewport.bind();

		cameraImageQuad.triangles(quadTriangles);
		cameraImageQuad.texCoords(quadUVs);
		cameraImageContour.triangles(contourTriangles);

		RenderingOptions options;
		options.depthTest = false;
		options.backFaceCulling = false;
		options.mode = Mesh::LineRenderMode;

		float dist = std::max(_cameraScaling, 1.05f*eye.znear());

		for (int cam_id = 0; cam_id < (int)cameras.size(); ++cam_id) {
			const auto & cam = cameras[cam_id].cam;

			const std::vector<Vector2f> corners2D = { { 0,0 },{ cam.w() - 1,0 },{ cam.w() - 1,cam.h() - 1 },{ 0,cam.h() - 1 } };
			std::vector<Vector3f> dirs;
			for (const auto & c : corners2D) {
				dirs.push_back(CameraRaycaster::computeRayDir(cam, c + 0.5f*Vector2f(1, 1)));
			}

			std::vector<Vector3f> vertices;
			for (const auto & d : dirs) {
				vertices.push_back(cam.position() + _cameraScaling * d);
			}

			cameraImageQuad.vertices(vertices);

			_cameraImageShaderArray.begin();
			_cameraImage_mvpArray.set(eye.viewproj());
			_alphaImageArray.send();

			_sliceArray.set(cam_id);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray->handle());

			cameraImageQuad.render(true, false);
			_cameraImageShaderArray.end();
			if (_useBasicCameraStub) {
				cameraImageContour.vertices(vertices);
				viewUtils.renderMeshWithColorAndAlpha(eye, cameraImageContour, Vector3f(0, 0, 1), 1.0f, options);
			}
		}

		glDisable(GL_BLEND);
	}


	void SceneDebugView::setScene(const std::shared_ptr<BasicIBRScene> & scene, const bool preserveCamera) {
		_scene = scene;
		const InputCamera cameraBack = _topViewCamera.getCamera();
		setup();
		_topViewCamera.setup(_scene->cameras()->inputCameras(), _topViewCamera.viewport(), _topViewCamera.getRaycaster());
		_topViewCamera.setupInterpolationPath(_scene->cameras()->inputCameras());
		// Optionally restore the camera pose.
		if (preserveCamera) {
			_topViewCamera.fromCamera(cameraBack, false);
		}
	}

	Mesh::Ptr generateCamFrustum(const InputCamera & cam, float near, float far)
	{
		static const Mesh::Triangles tris = {
			{0,0,1},{1,1,2},{2,2,3},{3,3,0},
			{4,4,5},{5,5,6},{6,6,7},{7,7,4},
			{0,0,4},{1,1,5},{2,2,6},{3,3,7},
		};

		std::vector<Vector3f> dirs;
		for (const auto & c : cam.getImageCorners()) {
			dirs.push_back(CameraRaycaster::computeRayDir(cam, c.cast<float>() + 0.5f*Vector2f(1, 1)));
		}
		float znear = (near >= 0 ? near : cam.znear());
		float zfar = (far >= 0 ? far : cam.zfar());
		Mesh::Vertices vertices;
		for (int k = 0; k < 2; k++) {
			float dist = (k == 0 ? znear : zfar);
			for (const auto & d : dirs) {
				vertices.push_back(cam.position() + dist * d);
			}
		}

		auto out = std::make_shared<Mesh>();
		out->vertices(vertices);
		out->triangles(tris);
		return out;
	}

	Mesh::Ptr generateCamFrustumColored(const InputCamera & cam, const Vector3f & col, float znear, float zfar)
	{
		auto out = generateCamFrustum(cam, znear, zfar);
		Mesh::Colors cols(out->vertices().size(), col);
		out->colors(cols);
		return out;
	}

	Mesh::Ptr generateCamQuadWithUvs(const InputCamera & cam, float dist)
	{
		static const Mesh::Triangles quadTriangles = {
			{ 0,1,2 },{ 0,2,3 }
		};
		static const Mesh::UVs quadUVs = {
			{ 0,1 } ,{ 1,1 } ,{ 1,0 } ,{ 0,0 }
		};

		std::vector<Vector3f> dirs;
		for (const auto & c : cam.getImageCorners()) {
			dirs.push_back(CameraRaycaster::computeRayDir(cam, c.cast<float>() + 0.5f*Vector2f(1, 1)));
		}
		std::vector<Vector3f> vertices;
		for (const auto & d : dirs) {
			vertices.push_back(cam.position() + dist * d);
		}

		auto out = std::make_shared<Mesh>();
		out->vertices(vertices);
		out->triangles(quadTriangles);
		out->texCoords(quadUVs);
		return out;
	}

	void ShaderImageArraySlice::initShader(const std::string & name, const std::string & vert, const std::string & frag)
	{
		ShaderAlphaMVP::initShader(name, vert, frag);
		slice.init(shader, "slice");
	}

	void ShaderImageArraySlice::render(const Camera & eye, const MeshData & data, GLuint textureArrayId, int image_id)
	{
		if (!data.meshPtr) {
			return;
		}
		shader.begin();
		ShaderAlphaMVP::setUniforms(eye, data);
		slice.set(image_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
		data.renderGeometry();
		shader.end();
	}


	void ShaderImageSlice::render(const Camera & eye, const MeshData & data, GLuint textureId)
	{
		if (!data.meshPtr) {
			return;
		}
		shader.begin();
		ShaderAlphaMVP::setUniforms(eye, data);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		data.renderGeometry();
		shader.end();
	}

	CameraInfos::CameraInfos(const InputCamera& cam, uint id, bool highlight)
		: cam(cam), id(id), highlight(highlight) {
	}

	void LabelsManager::setupLabelsManagerShader()
	{
		_labelShader.init("text-imgui",
			loadFile(Resources::Instance()->getResourceFilePathName("text-imgui.vp")),
			loadFile(Resources::Instance()->getResourceFilePathName("text-imgui.fp")));
		_labelShaderPosition.init(_labelShader, "position");
		_labelShaderScale.init(_labelShader, "scale");
		_labelShaderViewport.init(_labelShader, "viewport");
	}

	void LabelsManager::setupLabelsManagerMeshes(const std::vector<InputCamera> & cams)
	{
		_labelMeshes.clear();
		for (const auto & cam : cams) {
			unsigned int sepIndex = 0;
			_labelMeshes[cam.id()] = {};
			_labelMeshes[cam.id()].mesh = generateMeshForText(std::to_string(cam.id()), sepIndex);
			_labelMeshes[cam.id()].splitIndex = sepIndex;
		}
	}

	void LabelsManager::renderLabels(const Camera & eye, const Viewport & vp, const std::vector<CameraInfos>& cams_info)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		_labelShader.begin();
		// Bind the ImGui font texture.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (GLuint)ImGui::GetFont()->ContainerAtlas->TexID);
		_labelShaderViewport.set(Vector2f(vp.finalWidth(), vp.finalHeight()));

		for (const auto & camInfos : cams_info) {
			const auto & inputCam = camInfos.cam;
			if (!inputCam.isActive()) { continue; }
			const uint uid = camInfos.id;
			if (_labelMeshes.count(uid) == 0) {
				continue;
			}
			// Draw the label.
			// TODO: we could try to use depth testing to have the labels overlap properly.
			// As the label is put at the position of the camera, the label will intersect with the frustum mesh, causing artifacts.
			// One way of solving this would be to just shift the label away a bit and enable depth testing (+ GL_LEQUAl for the text).
			const Vector3f camProjPos = eye.project(inputCam.position());
			if (!eye.frustumTest(inputCam.position(), camProjPos.xy())) {
				continue;
			}
			_labelShaderPosition.set(camProjPos);
			const auto & label = _labelMeshes[uid];
			// Render the background label.
			_labelShaderScale.set(0.8f*_labelScale);
			label.mesh->renderSubMesh(0, label.splitIndex, false, false);
			// Render the text label.
			_labelShaderScale.set(1.0f*_labelScale);
			label.mesh->renderSubMesh(label.splitIndex, label.mesh->triangles().size() * 3, false, false);

		}
		_labelShader.end();
		glDisable(GL_BLEND);
	}

	void ImageCamViewer::initImageCamShaders()
	{
		const std::string vertex_str = loadFile(Resources::Instance()->getResourceFilePathName("uv_mesh.vert"));

		_cameraImageShader.initShader("cameraImageShader",
			vertex_str,
			loadFile(Resources::Instance()->getResourceFilePathName("alpha_uv_tex.frag")));

		_cameraImageShaderArray.initShader("cameraImageShaderArray",
			vertex_str,
			loadFile(Resources::Instance()->getResourceFilePathName("alpha_uv_tex_array.frag")));
	}

	void ImageCamViewer::renderImage(const Camera & eye, const InputCamera & cam,
		const std::vector<RenderTargetRGBA32F::Ptr> rts, int cam_id)
	{
		MeshData quad("", generateCamQuadWithUvs(cam, _cameraScaling));
		quad.setBackFace(false).setAlpha(_alphaImage);
		if (cam_id < rts.size() && rts[cam_id]) {
			_cameraImageShader.render(eye, quad, rts[cam_id]->handle());
		}
	}

	void ImageCamViewer::renderImage(const Camera & eye, const InputCamera & cam, uint tex2Darray_handle, int cam_id)
	{
		MeshData quad("", generateCamQuadWithUvs(cam, _cameraScaling));
		quad.setBackFace(false).setAlpha(_alphaImage);
		_cameraImageShaderArray.render(eye, quad, tex2Darray_handle, cam_id);
	}

	TopView::TopView(const BasicIBRScene::Ptr & scene, const Viewport & viewport,
		const InteractiveCameraHandler::Ptr & camHandler, const BasicIBRAppArgs & myArgs)
	{
		initImageCamShaders();
		setupLabelsManagerShader();

		_scene = scene;
		_userCurrentCam = camHandler;
		//camera_handler.setup(_scene->proxies()->proxy().clone(), viewport);

		if (!_scene->cameras()->inputCameras().empty()) {
			camera_handler.updateView(_scene->cameras()->inputCameras()[0]);
			camera_handler.setupInterpolationPath(_scene->cameras()->inputCameras());
		}

		_showImages = true;
		camera_path = myArgs.dataset_path.get() + "/cameras";

		setup();

		//camera_handler.switchMode(InteractiveCameraHandler::InteractionMode::TRACKBALL);

	}

	void TopView::onUpdate(Input & input, const float deltaTime, const Viewport & viewport)
	{
		MultiMeshManager::onUpdate(input, viewport);

		//Camera stub size
		if (input.key().isActivated(Key::LeftControl) && input.mouseScroll() != 0.0) {
			_cameraScaling = std::max(0.001f, _cameraScaling + (float)input.mouseScroll()*0.1f);
		}
		if (input.key().isActivated(Key::LeftControl) && input.key().isReleased(Key::P)) {
			MeshData & guizmo = getMeshData("guizmo");
			guizmo.active = !guizmo.active;
		}

		if (input.key().isActivated(Key::LeftControl) && input.key().isReleased(Key::Z)) {
			MeshData & proxy = getMeshData("proxy");
			if (proxy.renderMode == Mesh::RenderMode::FillRenderMode) {
				proxy.renderMode = Mesh::RenderMode::LineRenderMode;
			} else {
				proxy.renderMode = Mesh::RenderMode::FillRenderMode;
			}
		}

		if (input.key().isReleased(Key::T)) {
			save();
		}
	}

	void TopView::onUpdate(Input & input, const sibr::Viewport & viewport)
	{
		onUpdate(input, 1.0f / 60.0f, viewport);
	}

	void TopView::onUpdate(Input & input)
	{
		// Update camera with a fixed timestep.
		onUpdate(input, 1.0f / 60.0f);
	}

	void TopView::onRender(Window & win)
	{
		// We need no information about the window, we render wherever we are.
		onRender(win.viewport());
	}

	void TopView::onRender(const Viewport & viewport)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Scene debug view");

		viewport.clear(backgroundColor);
		viewport.bind();

		addMeshAsLines("scene cam", generateCamFrustum(_userCurrentCam->getCamera(), 0.0f, _cameraScaling)).setColor({ 1,0,0 });

		if (_scene) {
			for (int i = 0; i < (int)_scene->cameras()->inputCameras().size(); ++i) {
				_cameras[i].highlight =  _scene->cameras()->isCameraUsedForRendering(_scene->cameras()->inputCameras()[i].id());
			}
		}	

		auto used_cams = std::make_shared<Mesh>(), non_used_cams = std::make_shared<Mesh>();
		for (const auto & camInfos : _cameras) {
			if (!camInfos.cam.isActive()) { continue; }
			(camInfos.highlight ? used_cams : non_used_cams)->merge(*generateCamFrustum(camInfos.cam, 0.0f, _cameraScaling));
		}

		addMeshAsLines("used cams", used_cams).setColor({ 0,1,0 });
		addMeshAsLines("non used cams", non_used_cams).setColor({ 0,0,1 });

		renderMeshes();

		if (_scene && _showImages) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			int cam_id = 0;
			for (const auto & camInfos : _cameras) {
				if (camInfos.cam.isActive()) {
					const auto & scene_rts = _scene->renderTargets();
					if (scene_rts->getInputRGBTextureArrayPtr()) {
						renderImage(camera_handler.getCamera(), camInfos.cam, scene_rts->getInputRGBTextureArrayPtr()->handle(), cam_id);
					} else {
						renderImage(camera_handler.getCamera(), camInfos.cam, scene_rts->inputImagesRT(), cam_id);
					}
				}
				++cam_id;
			}
			glDisable(GL_BLEND);
		}

		if (_showLabels) {
			renderLabels(camera_handler.getCamera(), viewport, _cameras);
		}

		camera_handler.onRender(viewport);
		glPopDebugGroup();
	}

	void TopView::onGUI()
	{
		if (ImGui::Begin("Top view settings")) {
			gui_options();
			list_mesh_onGUI();
			gui_cameras();
		}
		ImGui::End();
		
	}

	void TopView::save()
	{
		const std::string filename = camera_path + "/topview.txt";
		std::ofstream outfile(filename, std::ios::out | std::ios::trunc);
		std::cerr << "Saving topview camera to topview.txt" << std::endl;
		// save camera view proj matrix
		camera_handler.getCamera().writeToFile(outfile);
	}

	void TopView::setScene(const BasicIBRScene::Ptr & scene, bool preserveCamera)
	{
		_scene = scene;
		const InputCamera cameraBack = camera_handler.getCamera();
		setup();
		camera_handler.setup(_scene->cameras()->inputCameras(), camera_handler.viewport(), camera_handler.getRaycaster());
		camera_handler.setupInterpolationPath(_scene->cameras()->inputCameras());
		// Optionally restore the camera pose.
		if (preserveCamera) {
			camera_handler.fromCamera(cameraBack, false);
		}
	}

	void TopView::gui_options()
	{
		if (ImGui::CollapsingHeader("OptionsTopView##", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button("Save topview")) {
				save();
			}

			ImGui::PushScaledItemWidth(120);
			ImGui::InputFloat("Camera scale", &_cameraScaling, 0.1f, 10.0f);
			_cameraScaling = std::max(0.001f, _cameraScaling);

			ImGui::Checkbox("Draw labels ", &_showLabels);
			if (_showLabels) {
				ImGui::SameLine();
				ImGui::InputFloat("Label scale", &_labelScale, 0.2f, 10.0f);
			}

			ImGui::Separator();
			ImGui::Checkbox("Draw Input Images ", &_showImages);
			if (_showImages) {
				ImGui::SameLine();
				ImGui::SliderFloat("Alpha", &_alphaImage, 0, 1.0);
			}

			

			camera_handler.onGUI("Top view settings");
			ImGui::PopItemWidth();
			ImGui::Separator();
		}
	}

	void TopView::gui_cameras()
	{
		if (ImGui::CollapsingHeader("Cameras##TopView")) {
			ImGui::Columns(4); // 0 name | snapto | active| size 

			ImGui::Separator();
			ImGui::Text("Camera"); ImGui::NextColumn();
			ImGui::Text("SnapTo"); ImGui::NextColumn();
			ImGui::Text("Active"); ImGui::NextColumn();
			ImGui::Text("Size"); ImGui::NextColumn();
			ImGui::Separator();
			for (uint i = 0; i < _cameras.size(); ++i) {
				std::string name = "cam_" + intToString<4>(i);
				ImGui::Text(name.c_str());
				ImGui::NextColumn();

				if (ImGui::Button(("SnapTo##" + name).c_str())) {
					//camera_handler.snapToCamera(i);
					//camera_handler.fromCamera(_cameras[i].cam, true, false);
					camera_handler.fromTransform(_cameras[i].cam.transform(), true, false);
				}
				ImGui::NextColumn();

				ImGui::Checkbox(("##is_valid" + name).c_str(), &_cameras[i].highlight);
				ImGui::NextColumn();
				std::stringstream temp;
				temp << _cameras[i].cam.w() << " x " << _cameras[i].cam.h();
				ImGui::Text(temp.str().c_str());
				ImGui::NextColumn();
			}
			ImGui::Columns(1);
		}
	}

	void TopView::setup()
	{
		if (_scene) {
			setupLabelsManagerMeshes(_scene->cameras()->inputCameras());
			setupMeshes();

			_cameras.clear();
			for (const auto & inputCam : _scene->cameras()->inputCameras()) {
				_cameras.emplace_back(inputCam, inputCam.id(), _scene->cameras()->isCameraUsedForRendering(inputCam.id()));
			}
		}

		_snapToImage = 0;
		_showLabels = false;

		std::string filename = camera_path + "/topview.txt";
		// check if topview.txt exists
		std::ifstream topViewFile(filename);
		if (topViewFile.good())
		{
			SIBR_LOG << "Loaded saved topview (" << filename << ")." << std::endl;
			// Intialize a temp camera (used to load the saved top view pose) with
			// the current top view camera to get the resolution/fov right.
			InputCamera cam(camera_handler.getCamera());
			cam.readFromFile(topViewFile);
			// Apply it to the top view FPS camera.
			//camera_handler.fromCamera(cam, false);
			camera_handler.fromTransform(cam.transform(), false, true);
		}

	}

	void TopView::setupMeshes()
	{
		addMesh("proxy", _scene->proxies()->proxyPtr());

		//guizmo
		addMeshAsLines("guizmo", RenderUtility::createAxisGizmoPtr())
			.setDepthTest(false).setColorMode(MeshData::ColorMode::VERTEX);
	}

} // namespace


