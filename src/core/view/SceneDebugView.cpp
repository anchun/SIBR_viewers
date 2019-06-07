# include "core/view/SceneDebugView.hpp"
# include "core/graphics/RenderUtility.hpp"
# include "core/graphics/Input.hpp"
# include "core/graphics/GUI.hpp"
#include "ViewUtils.hpp"
#include <core/raycaster/CameraRaycaster.hpp>


namespace sibr
{

	SceneDebugView::SceneDebugView(const std::shared_ptr<sibr::BasicIBRScene> & scene, const sibr::Viewport & viewport, const sibr::InteractiveCameraHandler::Ptr & camHandler, const sibr::BasicIBRAppArgs & myArgs)
	{
		_scene = scene;
		_userCurrentCam = camHandler;
		_topViewCamera.setup(_scene->proxies()->proxy().clone(), viewport);
		_topViewCamera.updateView(_scene->userCamera());
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
			sibr::InputCamera cam(_topViewCamera.getCamera());
			cam.readFromFile(topViewFile);
			// Apply it to the top view FPS camera.
			_topViewCamera.fromCamera(cam);
		}

		_topviewShader.init("TopviewShader",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("mesh_debugview.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("mesh_debugview.fp")));

		_topviewShader_proj.init(_topviewShader, "MVP");
		_topviewShader_inv_mv.init(_topviewShader, "invMV");
		_topviewShader_lightDir.init(_topviewShader, "lightDir");
		_topviewShader_hasColor.init(_topviewShader, "hasColor");
		_topviewShader_hasNormal.init(_topviewShader, "hasNormal");
		// Cam stub setup.
		_camStubShader.init("camstub",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("camstub.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("camstub.fp")));
		_camStubShaderMVP.init(_camStubShader, "MVP");
		_camStubShaderColor.init(_camStubShader, "color");
		_camStubMesh = std::move(RenderUtility::createCameraStub());
		_cameraScaling = 0.8f;

		// Axis gizmo setup.
		_axisGizmoShader.init("axisgizmo",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("axisgizmo.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("axisgizmo.fp")));
		_axisGizmoShaderMVP.init(_axisGizmoShader, "MVP");
		_axisGizmoMesh = std::move(RenderUtility::createAxisGizmo());

		// Setup labels.
		_labelShader.init("text-imgui",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("text-imgui.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("text-imgui.fp")));
		_labelShaderPosition.init(_labelShader, "position");
		_labelShaderScale.init(_labelShader, "scale");
		_labelShaderViewport.init(_labelShader, "viewport");

		setupCameraImageShader();

		
		if (_scene) {

			for (const auto & cam : _scene->cameras()->inputCameras()) {
				unsigned int sepIndex = 0;
				_labelMeshes[cam.id()] = {};
				_labelMeshes[cam.id()].mesh = sibr::generateMeshForText(std::to_string(cam.id()), sepIndex);
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

	void SceneDebugView::onUpdate(const sibr::Input & input, const float deltaTime, const sibr::Viewport & viewport)
	{
		_topViewCamera.update(input, deltaTime, viewport);
		//Camera stub size
		if (input.key().isActivated(sibr::Key::LeftControl) && input.mouseScroll() != 0.0) {
			setCameraScale(std::max(0.01f, getCameraScale() + (float)input.mouseScroll()*0.1f));
		}
		if (input.key().isActivated(sibr::Key::LeftControl) && input.key().isReleased(sibr::Key::P)) {
			_showGizmo = !_showGizmo;
		}

		if (input.key().isActivated(sibr::Key::LeftControl) && input.key().isReleased(sibr::Key::Z)) {
			_wireframeMode = !_wireframeMode;
		}

		if (input.key().isReleased(sibr::Key::T)) {
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

	void SceneDebugView::onRender(const sibr::Viewport & viewport)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Scene debug view");

		glPointSize(2.f);
		viewport.clear(sibr::Vector3f(0.8f, 0.8f, 0.8f));
		auto & cam = _topViewCamera.getCamera();

		const sibr::Matrix4f cameraScalingMatrix = (sibr::Vector4f(_cameraScaling, _cameraScaling, _cameraScaling, 1.0f)).asDiagonal();
		_topviewShader.begin();
		_topviewShader_proj.set(cam.viewproj());
		const Matrix4f invMV = (cam.invViewproj()).transpose();
		_topviewShader_inv_mv.set(invMV);
		const Vector3f lightDir = -cam.dir().normalized();
		_topviewShader_lightDir.set(lightDir);
		glDepthFunc(GL_LESS);
		glClearDepth(1.0);
		_topviewShader.end();

		const sibr::Mesh * globalMesh = nullptr;
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
				_camStubShaderColor.set(sibr::Vector3f(0.f, 0.f, 0.f));
				globalMesh->render(true, true, sibr::Mesh::LineRenderMode, false);
				_camStubShader.end();
			}
			else {
				_topviewShader.begin();
				_topviewShader_hasColor.set(globalMesh->hasColors());
				_topviewShader_hasNormal.set(globalMesh->hasNormals());
				globalMesh->render(true, true, sibr::Mesh::FillRenderMode, false);
				_topviewShader.end();
			}
		}

		if (_showCamStubs) {
			for (const auto & camInfos : _cameras)
			{
				if (!camInfos.cam.isActive()) { continue; }
				const sibr::Vector3f color = camInfos.highlight ? sibr::Vector3f(0.f, 1.f, 0.f) : sibr::Vector3f(0.f, 0.f, 1.f);
				if (_useBasicCameraStub) {
					_viewUtils.renderCamStub(sibr::Camera(cam), sibr::Camera(camInfos.cam), color, _cameraScaling);
				}
				else {
					_viewUtils.renderCamFrustum(cam, camInfos.cam, color, 1, sibr::Mesh::LineRenderMode, 0.0f, _cameraScaling);
				}
			}
			if (_useBasicCameraStub) {
				
				_viewUtils.renderCamStub(sibr::Camera( cam), sibr::Camera(*userCamera), Vector3f(1.f, 0.f, 0.f), _cameraScaling);
			}
			else {
				_viewUtils.renderCamFrustum(cam, *userCamera, Vector3f(1.f, 0.f, 0.f), 1, sibr::Mesh::LineRenderMode, 0.0f, _cameraScaling);
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
				const sibr::Vector3f camProjPos = cam.project(inputCam.position());
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

	void SceneDebugView::renderCameraImages(const sibr::InputCamera & eye, const std::vector<CameraInfos> & cameras, const sibr::Viewport & viewport, const std::vector<GLuint> & textureIds)
	{
		
		if (textureIds.empty()) {
			return;
		}
		static const sibr::Mesh::Triangles quadTriangles = {
			{ 0,1,2 },{ 0,2,3 }
		};
		static const sibr::Mesh::UVs quadUVs = {
			{ 0,1 } ,{ 1,1 } ,{ 1,0 } ,{ 0,0 }
		};
		static const sibr::Mesh::Triangles contourTriangles = {
			{ 0,0,1 },{ 1,1,2 }, {2,2,3}, {3,3,0}
		};

		sibr::Mesh cameraImageQuad(true);
		sibr::Mesh cameraImageContour(true);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendEquation(GL_FUNC_ADD);

		viewport.bind();

		CHECK_GL_ERROR;

		cameraImageQuad.triangles(quadTriangles);
		cameraImageQuad.texCoords(quadUVs);
		cameraImageContour.triangles(contourTriangles);

		sibr::RenderingOptions options;
		options.depthTest = false;
		options.backFaceCulling = false;
		options.mode = sibr::Mesh::LineRenderMode;


		float dist = std::max(_cameraScaling, 1.05f*eye.znear());

		for (int cam_id = 0; cam_id < (int)cameras.size(); ++cam_id) {

			const auto & cam = cameras[cam_id].cam;
			if (!cam.isActive()) { continue; }

			const std::vector<Vector2f> corners2D = { { 0,0 },{ cam.w() - 1,0 },{ cam.w() - 1,cam.h() - 1 },{ 0,cam.h() - 1 } };
			std::vector<Vector3f> dirs;
			for (const auto & c : corners2D) {
				dirs.push_back(sibr::CameraRaycaster::computeRayDir(cam, c + 0.5f*sibr::Vector2f(1, 1)));
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
				viewUtils.renderMeshWithColorAndAlpha(eye, cameraImageContour, sibr::Vector3f(0, 0, 1), 1.0f, options);
			}
		}

		glDisable(GL_BLEND);
	}

	void sibr::SceneDebugView::renderCameraImagesArray(const sibr::InputCamera & eye, const std::vector<CameraInfos> & cameras, const sibr::Viewport & viewport, const sibr::Texture2DArrayRGB::Ptr textureArray)
	{
		static const sibr::Mesh::Triangles quadTriangles = {
			{ 0,1,2 },{ 0,2,3 }
		};
		static const sibr::Mesh::UVs quadUVs = {
			{ 0,1 } ,{ 1,1 } ,{ 1,0 } ,{ 0,0 }
		};
		static const sibr::Mesh::Triangles contourTriangles = {
			{ 0,0,1 },{ 1,1,2 },{ 2,2,3 },{ 3,3,0 }
		};

		CHECK_GL_ERROR;

		sibr::Mesh cameraImageQuad(true);
		sibr::Mesh cameraImageContour(true);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		viewport.bind();

		cameraImageQuad.triangles(quadTriangles);
		cameraImageQuad.texCoords(quadUVs);
		cameraImageContour.triangles(contourTriangles);

		sibr::RenderingOptions options;
		options.depthTest = false;
		options.backFaceCulling = false;
		options.mode = sibr::Mesh::LineRenderMode;

		float dist = std::max(_cameraScaling, 1.05f*eye.znear());

		for (int cam_id = 0; cam_id < (int)cameras.size(); ++cam_id) {
			const auto & cam = cameras[cam_id].cam;

			const std::vector<Vector2f> corners2D = { { 0,0 },{ cam.w() - 1,0 },{ cam.w() - 1,cam.h() - 1 },{ 0,cam.h() - 1 } };
			std::vector<Vector3f> dirs;
			for (const auto & c : corners2D) {
				dirs.push_back(sibr::CameraRaycaster::computeRayDir(cam, c + 0.5f*sibr::Vector2f(1, 1)));
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
				viewUtils.renderMeshWithColorAndAlpha(eye, cameraImageContour, sibr::Vector3f(0, 0, 1), 1.0f, options);
			}
		}

		glDisable(GL_BLEND);
	}


	void SceneDebugView::setScene(const std::shared_ptr<sibr::BasicIBRScene> & scene, const bool preserveCamera) {
		_scene = scene;
		const sibr::InputCamera cameraBack = _topViewCamera.getCamera();
		setup();
		_topViewCamera.setup(_scene->cameras()->inputCameras(), _topViewCamera.viewport(), _topViewCamera.getRaycaster());
		_topViewCamera.setupInterpolationPath(_scene->cameras()->inputCameras());
		// Optionally restore the camera pose.
		if (preserveCamera) {
			_topViewCamera.fromCamera(cameraBack, false);
		}
	}

	sibr::Mesh::Ptr generateCamFrustum(const sibr::InputCamera & cam, float near, float far)
	{
		static const sibr::Mesh::Triangles tris = {
			{0,0,1},{1,1,2},{2,2,3},{3,3,0},
			{4,4,5},{5,5,6},{6,6,7},{7,7,4},
			{0,0,4},{1,1,5},{2,2,6},{3,3,7},
		};

		std::vector<Vector3f> dirs;
		for (const auto & c : cam.getImageCorners()) {
			dirs.push_back(sibr::CameraRaycaster::computeRayDir(cam, c.cast<float>() + 0.5f*sibr::Vector2f(1, 1)));
		}
		float znear = (near >= 0 ? near : cam.znear());
		float zfar = (far >= 0 ? far : cam.zfar());
		sibr::Mesh::Vertices vertices;
		for (int k = 0; k < 2; k++) {
			float dist = (k == 0 ? znear : zfar);
			for (const auto & d : dirs) {
				vertices.push_back(cam.position() + dist * d);
			}
		}

		auto out = std::make_shared<sibr::Mesh>();
		out->vertices(vertices);
		out->triangles(tris);
		return out;
	}

	sibr::Mesh::Ptr generateCamFrustumColored(const InputCamera & cam, const Vector3f & col, float znear, float zfar)
	{
		auto out = generateCamFrustum(cam, znear, zfar);
		Mesh::Colors cols(out->vertices().size(), col);
		out->colors(cols);
		return out;
	}

	sibr::Mesh::Ptr generateCamQuadWithUvs(const sibr::InputCamera & cam, float dist)
	{
		static const sibr::Mesh::Triangles quadTriangles = {
			{ 0,1,2 },{ 0,2,3 }
		};
		static const sibr::Mesh::UVs quadUVs = {
			{ 0,1 } ,{ 1,1 } ,{ 1,0 } ,{ 0,0 }
		};

		std::vector<Vector3f> dirs;
		for (const auto & c : cam.getImageCorners()) {
			dirs.push_back(sibr::CameraRaycaster::computeRayDir(cam, c.cast<float>() + 0.5f*sibr::Vector2f(1, 1)));
		}
		std::vector<Vector3f> vertices;
		for (const auto & d : dirs) {
			vertices.push_back(cam.position() + dist * d);
		}

		auto out = std::make_shared<sibr::Mesh>();
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

	void ShaderImageArraySlice::render(const sibr::Camera & eye, const MeshData & data, GLuint textureArrayId, int image_id)
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


	void ShaderImageSlice::render(const sibr::Camera & eye, const MeshData & data, GLuint textureId)
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

	CameraInfos::CameraInfos(const sibr::InputCamera& cam, uint id, bool highlight)
		: cam(cam), id(id), highlight(highlight) {
	}

	void LabelsManager::setupShader()
	{
		_labelShader.init("text-imgui",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("text-imgui.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("text-imgui.fp")));
		_labelShaderPosition.init(_labelShader, "position");
		_labelShaderScale.init(_labelShader, "scale");
		_labelShaderViewport.init(_labelShader, "viewport");
	}

	void sibr::LabelsManager::renderLabels(const Camera & eye, const Viewport & vp, const std::vector<CameraInfos>& cams_info)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		_labelShader.begin();
		// Bind the ImGui font texture.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (GLuint)ImGui::GetFont()->ContainerAtlas->TexID);
		_labelShaderViewport.set(sibr::Vector2f(vp.finalWidth(), vp.finalHeight()));

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
			const sibr::Vector3f camProjPos = eye.project(inputCam.position());
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
		const std::string vertex_str = sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("uv_mesh.vert"));

		_cameraImageShader.initShader("cameraImageShader",
			vertex_str,
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("alpha_uv_tex.frag")));

		_cameraImageShaderArray.initShader("cameraImageShaderArray",
			vertex_str,
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("alpha_uv_tex_array.frag")));
	}

	void ImageCamViewer::renderImage(const Camera & eye, const InputCamera & cam, const std::vector<RenderTargetRGBA32F::Ptr> rts, int cam_id)
	{
		sibr::MeshData quad("", generateCamQuadWithUvs(cam, _cameraScaling));
		quad.setBackFace(false).setAlpha(_alphaImage);
		_cameraImageShader.render(eye, quad, rts[cam_id]->handle());
	}

	void ImageCamViewer::renderImage(const Camera & eye, const InputCamera & cam, uint tex2Darray_handle, int cam_id)
	{
		sibr::MeshData quad("", generateCamQuadWithUvs(cam, _cameraScaling));
		quad.setBackFace(false).setAlpha(_alphaImage);
		_cameraImageShaderArray.render(eye, quad, tex2Darray_handle, cam_id);
	}

} // namespace


