#include "SceneDebugViewV2.hpp"

#include <core/graphics/GUI.hpp>

namespace sibr {

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

	TopView::CameraInfos::CameraInfos(const sibr::InputCamera& cam, uint id, bool highlight)
		: cam(cam), id(id), highlight(highlight) {
	}


	TopView::TopView(BasicIBRScene::Ptr scene) : MultiMeshManager("TopView")
	{
		changeScene(scene);
	}

	void TopView::onRender(const sibr::Viewport & viewport)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Scene debug view");

		viewport.clear({ 0.7f,0.7f,0.7f });
		viewport.bind();

		addMeshAsLines("scene cam", generateCamFrustum(_scene->userCamera(), 0.0f, _cameraScaling)).setColor({ 1,0,0 });

		auto used_cams = std::make_shared<Mesh>(), non_used_cams = std::make_shared<Mesh>();
		for (const auto & camInfos : _cameras) {
			if (!camInfos.cam.isActive()) { continue; }
			(camInfos.highlight ? used_cams : non_used_cams)->merge(*generateCamFrustum(camInfos.cam, 0.0f, _cameraScaling));
		}

		addMeshAsLines("used cams", used_cams).setColor({ 0,1,0 });
		addMeshAsLines("non used cams", non_used_cams).setColor({ 0,0,1 });

		renderMeshes();

		if (_showImages && _scene) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			int cam_id = 0;
			for (const auto & camInfos : _cameras) {
				if (camInfos.cam.isActive()) {
					sibr::MeshData quad("",generateCamQuadWithUvs(camInfos.cam, _cameraScaling));
					quad.backFaceCulling = false;
					quad.alpha = _alphaImage;
					if (_scene->renderTargets()->getInputRGBTextureArrayPtr()) {
						_cameraImageShaderArray.render(camera_handler.getCamera(), quad, _scene->renderTargets()->getInputRGBTextureArrayPtr()->handle(), cam_id);
					} else if(_scene->renderTargets()->inputImagesRT().size() > 0 && _scene->renderTargets()->inputImagesRT()[cam_id]) {
						_cameraImageShader.render(camera_handler.getCamera(), quad, _scene->renderTargets()->inputImagesRT()[cam_id]->handle());
					}
				}		
				++cam_id;
			}
			glDisable(GL_BLEND);
		}

		if (_showLabels) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			_labelShader.begin();
			// Bind the ImGui font texture.
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (GLuint)ImGui::GetFont()->ContainerAtlas->TexID);
			_labelShaderViewport.set(sibr::Vector2f(viewport.finalWidth(), viewport.finalHeight()));

			for (const auto & camInfos : _cameras) {
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
				const sibr::Vector3f camProjPos = camera_handler.getCamera().project(inputCam.position());
				if (!camera_handler.getCamera().frustumTest(inputCam.position(), camProjPos.xy())) {
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

		camera_handler.onRender(viewport);
		glPopDebugGroup();
	}

	void TopView::onGUI()
	{
		if (ImGui::Begin((name +  "##begin").c_str())) {
			if (ImGui::CollapsingHeader("OptionsTopView##")) {
				ImGui::PushItemWidth(250);
				ImGui::InputFloat("Camera scale", &_cameraScaling, 0.1f, 10.0f);
				ImGui::PopItemWidth();

				ImGui::Checkbox("Draw labels ", &_showLabels);
				if (_showLabels) {
					ImGui::SameLine();
					ImGui::PushItemWidth(250);
					ImGui::InputFloat("Label scale", &_labelScale, 0.2f, 10.0f);
					ImGui::PopItemWidth();
				}

				ImGui::Separator();
				ImGui::Checkbox("Draw Input Images ", &_showImages);
				if (_showImages) {
					ImGui::PushItemWidth(250);
					ImGui::SliderFloat("Alpha", &_alphaImage, 0, 1.0);
					ImGui::PopItemWidth();

					if (_scene && ImGui::InputInt("Snap To", &_snapToImage, 1, 10)) {
						if (_snapToImage < 0) {
							_snapToImage = 0;
						}
						if (_snapToImage >= _cameras.size()) {
							_snapToImage = int(_cameras.size() - 1);
						}
						camera_handler.snapToCamera(_snapToImage);
						//_showCamStubs = false;
					}

				}
			}
	
			list_mesh_onGUI();

			if (ImGui::CollapsingHeader("Cameras##TopView")) {
				ImGui::Columns(4); // 0 name | snapto | active| size 
				
				ImGui::Separator();
				ImGui::Text("Camera"); ImGui::NextColumn();
				ImGui::Text("SnapTo"); ImGui::NextColumn();
				ImGui::Text("Active"); ImGui::NextColumn();
				ImGui::Text("Size"); ImGui::NextColumn();
				ImGui::Separator();
				for (uint i = 0; i < _cameras.size(); ++i) {
					std::string name = "cam_" + sibr::intToString<4>(i);
					ImGui::Text(name.c_str());
					ImGui::NextColumn();

					if (ImGui::Button(("SnapTo##" + name).c_str())) {
						camera_handler.fromCamera(_cameras[i].cam);
					}
					ImGui::NextColumn();
					
					ImGui::Checkbox(("##is_valid" + name).c_str(), &_cameras[i].highlight);
					ImGui::NextColumn();

					ImGui::Text((std::stringstream() << _cameras[i].cam.w() << " x " << _cameras[i].cam.h()).str().c_str());
					ImGui::NextColumn();
				}
				ImGui::Columns(1);
			}

			ImGui::End();
		}	
	}

	void TopView::changeScene(BasicIBRScene::Ptr scene)
	{
		_scene = scene;

		//sibr::InputCamera init_cam = _scene->inputCameras()[0];
		//init_cam.znear(0.01f);
		//camera_handler.fromCamera(init_cam);
		//cam_is_init = true;
		//camera_handler.setupInterpolationPath(_scene->inputCameras());


		std::string filename;
		if (_scene) {
			filename = _scene->data()->basePathName() + "/topview.txt";
		}
		// check if topview.txt exists
		std::ifstream topViewFile(filename);
		if (topViewFile.good())
		{
			SIBR_LOG << "Loaded saved topview (" << filename << ")." << std::endl;
			// Intialize a temp camera (used to load the saved top view pose) with
			// the current top view camera to get the resolution/fov right.
			sibr::InputCamera cam(camera_handler.getCamera());
			cam.readFromFile(topViewFile);
			// Apply it to the top view FPS camera.
			camera_handler.fromCamera(cam);
		}

		setupMeshes();

		initImageCamShaders();

		_cameras.clear();
		for (const auto & inputCam : _scene->cameras()->inputCameras()) {
			_cameras.emplace_back(inputCam, inputCam.id(), true);
		}
	}

	void TopView::updateActiveCams(const std::vector<int> & cams_ids) {
		for (auto& cam : _cameras) {
			cam.cam.setActive(false);
		}
		for (int id : cams_ids) {
			if (id >= 0 && id < _cameras.size()) {
				_cameras[id].cam.setActive(true);
			}
		}
	}

	void TopView::setupMeshes()
	{
		addMesh("proxy", _scene->proxies()->proxyPtr()->clone());

		//addPoints("box center", { _scene->proxy().getBoundingBox().center() } );

		 if (_scene) {

			_labelMeshes.clear();
			for (const auto & cam : _scene->cameras()->inputCameras()) {
				unsigned int sepIndex = 0;
				_labelMeshes[cam.id()] = {};
				_labelMeshes[cam.id()].mesh = sibr::generateMeshForText(std::to_string(cam.id()), sepIndex);
				_labelMeshes[cam.id()].splitIndex = sepIndex;
			}
		}

		//guizmo
		auto & guizmo = addMesh("guizmo", RenderUtility::createAxisGizmoPtr());
		guizmo.renderMode = sibr::Mesh::LineRenderMode;
		guizmo.depthTest = false;
		guizmo.backFaceCulling = false;
	}

	void TopView::initImageCamShaders()
	{
		const std::string folder = "./shaders_rsc/";
		const std::string vertex_str = sibr::loadFile(folder + "uv_mesh.vert");
		_cameraImageShader.initShader("cameraImageShader", vertex_str, sibr::loadFile(folder + "alpha_uv_tex.frag"));
		_cameraImageShaderArray.initShader("cameraImageShader", vertex_str, sibr::loadFile(folder + "alpha_uv_tex_array.frag"));

		// Setup labels.
		_labelShader.init("text-imgui",
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("text-imgui.vp")),
			sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName("text-imgui.fp")));
		_labelShaderPosition.init(_labelShader, "position");
		_labelShaderScale.init(_labelShader, "scale");
		_labelShaderViewport.init(_labelShader, "viewport");
	}

	

}