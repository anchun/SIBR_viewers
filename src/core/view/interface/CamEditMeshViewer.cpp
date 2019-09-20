#include "CamEditMeshViewer.h"
#include <core/graphics/Window.hpp>
#include <core/assets/InputCamera.hpp>
#include <core/raycaster/Raycaster.hpp>
#include <core/view/InteractiveCameraHandler.hpp>
#include <core/raycaster/CameraRaycaster.hpp>
#include <core/assets/Resources.hpp>
#include <cmath>

sibr::CamEditMeshViewer::CamEditMeshViewer()
	: MeshViewer()
{

}


sibr::CamEditMeshViewer::CamEditMeshViewer(const sibr::Vector2i & screenRes,
	const sibr::Mesh & mesh,
	const bool launchRenderingLoop,
	const std::string name,
	const std::string outputPath)
	: MeshViewer(screenRes, mesh, launchRenderingLoop),
	_name(name)
{
	_typeOfMesh = CamEditMeshViewer::TypeOfMesh::mesh;
	Vector3f center;
	float radius;
	mesh.getBoundingSphere(center, radius);
	InputCamera currentInteractCam(interactCam->getCamera());
	currentInteractCam.zfar(10.f * radius);
	currentInteractCam.znear(radius / 1000.f);
	interactCam->fromCamera(currentInteractCam);
	_initialDeltaLight = radius / 10.f;
	_currentDeltaLight = _initialDeltaLight;
	_currentRadius = radius / 100.f; // Init the radius of the spheres lights
	_outputPath = outputPath;
}

sibr::CamEditMeshViewer::CamEditMeshViewer(const sibr::Vector2i & screenRes,
	const sibr::MaterialMesh & mesh,
	const bool launchRenderingLoop,
	const std::string name,
	const std::string outputPath)
	: MeshViewer(screenRes, mesh, launchRenderingLoop),
	_name(name), _materialMesh(mesh)
{
	CHECK_GL_ERROR;
	if (mesh.hasTagsFile()) {
		_shaderAlbedo.init("shaderAlbedo", _materialMesh.vertexShaderAlbedo,
			_materialMesh.fragmentShaderAlbedoTag);
	}
	else {
		_shaderAlbedo.init("shaderAlbedo", _materialMesh.vertexShaderAlbedo,
			_materialMesh.fragmentShaderAlbedo);
	}
	CHECK_GL_ERROR;
	_shaderThreeSixtyMaterials.init("shaderThreeSixtyMaterials",
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlat.vp")),
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlatColor.fp")),
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlat.gp")),
		true,
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlat.tcs")),
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlat.tes")));
	CHECK_GL_ERROR;
	_shaderThreeSixtyDepth.init("shaderThreeSixtyDepth",
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlat.vp")),
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlatDepth.fp")),
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlat.gp")),
		true,
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlat.tcs")),
		sibr::loadFile(sibr::Resources::Instance()->getResourceFilePathName(
			"longlat.tes")));

	CHECK_GL_ERROR;

	_MVP_layer.init(_shaderAlbedo, "MVP");
	CHECK_GL_ERROR;
	_activeAoColors_layer.init(_shaderAlbedo, "AoIsActive");
	CHECK_GL_ERROR;
	_illuminanceAoCoefficient_layer.init(_shaderAlbedo, "IlluminanceCoefficient");
	CHECK_GL_ERROR;
	_positionDepth_layer.init(_shaderThreeSixtyDepth, "pos");
	_positionMaterials_layer.init(_shaderThreeSixtyMaterials, "pos");
	CHECK_GL_ERROR;
	_typeOfMesh = CamEditMeshViewer::TypeOfMesh::materialMesh;
	_outputPath = outputPath;

	Vector3f center;
	float radius;
	mesh.getBoundingSphere(center, radius);

	_initialDeltaLight = radius / 10.f;
	_currentDeltaLight = _initialDeltaLight;
	_currentRadius = radius / 100.f; // Init the radius of the spheres lights

	InputCamera currentInteractCam(interactCam->getCamera());
	currentInteractCam.zfar(10 * radius);
	currentInteractCam.znear(radius / 1000.f);
	interactCam->fromCamera(currentInteractCam);


}

sibr::CamEditMeshViewer::~CamEditMeshViewer()
{
}

void sibr::CamEditMeshViewer::renderMaterialMesh(const sibr::Camera& eye,
	bool renderCameras) {

	renderer->resetMeshes();
	if (_materialMesh.typeOfRender() ==
		MaterialMesh::RenderCategory::diffuseMaterials)
	{
		_shaderAlbedo.begin();
		_MVP_layer.set(eye.viewproj());
		_activeAoColors_layer.set(_materialMesh.ambientOcclusion()
			.AoIsActive);
		_illuminanceAoCoefficient_layer.set(
			_materialMesh.ambientOcclusion().IlluminanceCoefficient);
		_materialMesh.initAlbedoTextures();
		_materialMesh.render();
		_shaderAlbedo.end();

		if (renderCameras)
			renderer->render(eye);
	}
	else if (_materialMesh.typeOfRender() ==
		MaterialMesh::RenderCategory::threesixtyDepth) {
		_shaderThreeSixtyDepth.begin();
		_positionDepth_layer.set(eye.position());
		_materialMesh.render(true, false, sibr::Mesh::FillRenderMode, false,
			false);
		_shaderThreeSixtyDepth.end();
	}
	else if (_materialMesh.typeOfRender() ==
		MaterialMesh::RenderCategory::threesixtyMaterials) {
		_shaderThreeSixtyMaterials.begin();
		_positionMaterials_layer.set(eye.position());
		_materialMesh.render(true, false, sibr::Mesh::FillRenderMode, false,
			false);
		_shaderThreeSixtyMaterials.end();
	}
}

void sibr::CamEditMeshViewer::render(const sibr::Viewport & viewport,
	const sibr::Camera & eye)
{
	if (_typeOfMesh == CamEditMeshViewer::TypeOfMesh::mesh ||
		_materialMesh.typeOfRender() == MaterialMesh::RenderCategory::classic) {
		MeshViewer::render(viewport, eye);
	}
	if (_typeOfMesh == CamEditMeshViewer::TypeOfMesh::materialMesh) {
		viewport.bind();
		viewport.clear(sibr::Vector3f(0.9f, 0.9f, 0.9f));

		renderMaterialMesh(eye);
		
		interactCam->onRender(viewport);

		fpsCounter.update(true);
	}
}


void sibr::CamEditMeshViewer::render(const sibr::Viewport & viewport)
{
	render(viewport, interactCam->getCamera());
}

void sibr::CamEditMeshViewer::render(const sibr::Camera & eye)
{
	if (window.get()) {
		render(window->viewport(), eye);
		window->swapBuffer();
	}
}

void sibr::CamEditMeshViewer::render()
{
	if (window.get()) {
		render(window->viewport(), interactCam->getCamera());
		renderGizmo();
		window->swapBuffer();
	}
}

void sibr::CamEditMeshViewer::renderLoop(sibr::Window & window)
{
	bool doLoop = true;

	while (doLoop && window.isOpened()) {
		sibr::Input::poll();

		if (sibr::Input::global().key().isPressed(sibr::Key::Escape)) {
			doLoop = false;
		}

		interactCam->update(sibr::Input::global(), 1 / 60.0f, window.viewport());

		window.viewport().bind();
		window.viewport().clear(sibr::Vector3f(0.9f, 0.9f, 0.9f));
		renderer->render(interactCam->getCamera());
		interactCam->onRender(window.viewport());

		window.swapBuffer();
	}
}

void sibr::CamEditMeshViewer::renderLoop(std::shared_ptr<sibr::Window>
	otherWindow)
{
	if (!otherWindow.get() && !window->isOpened()) {
		return;
	}
	if (otherWindow.get() && !window.get()) {
		window = otherWindow;
	}

	while (window->isOpened()) {
		sibr::Input::poll();

		if (sibr::Input::global().key().isPressed(sibr::Key::Escape)) {
			window->close();
		}

		interactCam->update(sibr::Input::global(), 1 / 60.0f, window->viewport());
		render();
	}
	reset();
}

void sibr::CamEditMeshViewer::renderLoop(const std::function
	<void(CamEditMeshViewer*)>& f,
	bool customRendering)
{
	bool doRender = true;
	while (doRender && window->isOpened()) {
		sibr::Input::poll();
		input = sibr::Input::global();
		if (input.key().isPressed(sibr::Key::Escape)) {
			doRender = false;
			if (inChargeOfWindow) {
				window->close();
			}
		}
		

		listenKey();
		if (!gizmoIsUsing)
		interactCam->update(input, 1 / 60.0f, window->viewport());

		f(this);
		onGUI();
		renderCameras();
		renderLights();
		if (!customRendering) {
			render();
		}
	}
	reset();

}

void sibr::CamEditMeshViewer::reset()
{
	_materialMesh = MaterialMesh(false);
	_shaderAlbedo.terminate();
	_shaderThreeSixtyMaterials.terminate();
	_shaderThreeSixtyDepth.terminate();
	MeshViewer::reset();
}

void sibr::CamEditMeshViewer::onGUI() {

	if (_typeOfApp == CamEditMeshViewer::TypeOfApp::CamEditor) {
		if (ImGui::Begin("Camera Editor")) {
			if (ImGui::Button("Save cameras")) {
				writeCameras();
			}
			ImGui::SameLine();
			if (ImGui::Button("Save bundle")) {
				std::vector<InputCamera> camerasToSaveRecons;
				std::vector<InputCamera> camerasToSaveRender;
				for (std::pair<cameraCategory, std::vector<InputCamera>>
					&set : _setsOfCameras)
				{
					if (set.first == cameraCategory::render ||
						set.first == cameraCategory::both) {
						camerasToSaveRender.insert(camerasToSaveRender.end(),
							set.second.begin(), set.second.end());
					}
					if (set.first == cameraCategory::reconstruction ||
						set.first == cameraCategory::both) {
						camerasToSaveRecons.insert(camerasToSaveRecons.end(),
							set.second.begin(), set.second.end());
					}
				}

				std::string outputBundlePath;
				if (sibr::showFilePicker(outputBundlePath, FilePickerMode::Save,
					"", "out")) {
					if (!outputBundlePath.empty()) {
						sibr::InputCamera::saveAsBundle(camerasToSaveRender,
							outputBundlePath + ".out", false, false);
						sibr::InputCamera::saveAsBundle(camerasToSaveRecons,
							outputBundlePath + "_recons.out", false, false);
					}
				}

			}
			ImGui::SameLine();
			if (ImGui::Button("Load cameras")) {
				//load();
				SIBR_WRG << "Uninmplemented for now." << std::endl;
			}
			if (ImGui::CollapsingHeader("Parameters")) {
				ImGui::InputFloat("Size of cameras", &_scaleCam, 0.4f, 128.f);
				ImGui::InputInt("Number of interpolations",
					&_numberInterpolations, 2, 50);
				ImGui::InputInt("Number of cameras around",
					&_numberCamerasAround, 2, 50);
				ImGui::InputFloat("Distance of cameras around",
					&_maximumDistance, 0.4f, 128.f);

				static int e = 2;
				ImGui::RadioButton("Reconstruction", &e, 0); ImGui::SameLine();
				ImGui::RadioButton("Render", &e, 1); ImGui::SameLine();
				ImGui::RadioButton("Both", &e, 2);

				if (e == 0) _setCategory = cameraCategory::reconstruction;
				else if (e == 1) _setCategory = cameraCategory::render;
				else if (e == 2) _setCategory = cameraCategory::both;

				ImGui::Separator();
			}
			if (ImGui::CollapsingHeader("Current set")) {
				if (ImGui::Button("Place camera")) {
					InputCamera camToAdd = interactCam->getCamera();
					InputCamera& refToCam = camToAdd;
					addCamera(refToCam);
				}
				ImGui::SameLine();
				if (ImGui::Button("Interpolate")) {
					interpolate_cameras(_numberInterpolations, 0.5f);
				}
				ImGui::SameLine();
				if (ImGui::Button("Make cameras around")) {
					interpolate_cameras(_numberInterpolations, 0.5f);
					makeCamerasAround(_numberCamerasAround, _maximumDistance);
				}
				if (ImGui::Button("Validate")) {
					validateSpline();
					showInformationsList(_controlCameras);
				}
				ImGui::SameLine();
				if (ImGui::Button("Clear")) {
					_currentList.clear();
					_interpolatedCameras.clear();
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove last")) {
					if (!_interpolatedCameras.empty()) _interpolatedCameras.pop_back();
					else if (!_currentList.empty()) _currentList.pop_back();
					_interpolatedCameras.clear();
				}
			}

			static unsigned int currentSetToSnapping = 0;
			static unsigned int currentCamToSnapping = 0;

			auto itSet = _setsOfCameras.begin();
			unsigned int currentIndexCamera = 0;
			for (unsigned int i = 0; i < _setsOfCameras.size(); i++)
			{
				std::vector<InputCamera>& camerasCurrentSet = itSet->second;

				std::string nameSet = std::string(std::string("Set") + std::to_string(i));
				if (ImGui::CollapsingHeader(nameSet.c_str())) {
					std::string deleteButton = std::string("Delete##") + nameSet;
					if (ImGui::Button(deleteButton.c_str())) {
						_setsOfCameras.erase(itSet);
						break;
					}

					ImGui::SameLine();
					std::string snapButton = std::string("Snap##") + nameSet;
					if (ImGui::Button(snapButton.c_str())) {
						if (i == currentSetToSnapping) {
							currentCamToSnapping = (currentCamToSnapping + 1)
								% camerasCurrentSet.size();
						}
						else {
							currentSetToSnapping = i;
							currentCamToSnapping = 0;
						}
						InputCamera& camToSnapping = camerasCurrentSet
							[currentCamToSnapping];
						InputCamera currentTrackBallCamera(interactCam->getCamera());
						currentTrackBallCamera.setLookAt(camToSnapping.position(),
							camToSnapping.position() + camToSnapping.dir(),
							camToSnapping.up());
						interactCam->fromCamera(currentTrackBallCamera);
					}

					//for (InputCamera&camera : noValidated )
					for (unsigned int j = 0; j < camerasCurrentSet.size(); j++)
					{
						InputCamera& cam = camerasCurrentSet[j];
						std::string nameCam = std::string(std::string("Cam") +
							std::to_string(currentIndexCamera));
						if (ImGui::CollapsingHeader(nameCam.c_str())) {
							std::string snapButton = std::string("Snap##Direct")
								+ nameCam;
							if (ImGui::Button(snapButton.c_str())) {
								InputCamera currentTrackBallCamera(
									interactCam->getCamera());
								currentTrackBallCamera.setLookAt(cam.position(),
									cam.position() + cam.dir(), cam.up());
								interactCam->fromCamera(currentTrackBallCamera);
							}
							ImGui::SameLine();
							std::string deleteButton = std::string("Delete Cam##")
								+ nameCam;

							if (ImGui::Button(deleteButton.c_str())) {
								auto itCam = camerasCurrentSet.begin();
								unsigned int index = 0;
								while (index < j) { itCam++; index++; }
								camerasCurrentSet.erase(itCam);
								if (camerasCurrentSet.empty())
									_setsOfCameras.erase(itSet);
								break;
							}

						}
						currentIndexCamera++;
					}
				}
				itSet++;
			}
		}
	}

	if (_typeOfMesh != CamEditMeshViewer::TypeOfMesh::mesh 
		&& _typeOfApp == CamEditMeshViewer::TypeOfApp::CamRenderer)
		if (ImGui::Begin("Rendering Parameters")) {
			static MaterialMesh::AmbientOcclusion aoCurrentParam =
				_materialMesh.ambientOcclusion();
			if (ImGui::CollapsingHeader("Ambient Occlusion")) {
				if (ImGui::Checkbox("Activate", &aoCurrentParam.AoIsActive)) {
					_materialMesh.ambientOcclusion(aoCurrentParam);
				}
				if (ImGui::SliderFloat("Illuminance coefficient",
					&aoCurrentParam.IlluminanceCoefficient, 0.f, 5.f))
				{
					_materialMesh.ambientOcclusion(aoCurrentParam);
				}
				ImGui::SliderFloat("Attenuation distance (Needs recomputing)",
					&aoCurrentParam.AttenuationDistance, 0.f, 10.f);
				ImGui::SliderFloat("Subdivide Threshold(Needs recomputing)",
					&aoCurrentParam.SubdivideThreshold, 10.f, 0.1f);
				if (ImGui::Button("Recompute AO")) {
					_materialMesh.ambientOcclusion(aoCurrentParam);
				}


			}
			ImGui::Separator();
			if (ImGui::CollapsingHeader("360 Options")) {

				static int option360 = 0;
				int currentOption = option360;

				ImGui::RadioButton("OFF", &option360, 0); ImGui::SameLine();
				ImGui::RadioButton("Depth", &option360, 1); ImGui::SameLine();
				ImGui::RadioButton("Materials", &option360, 2);

				if (option360 != currentOption) {
					if (option360 == 0) {
						aoCurrentParam.AoIsActive = false;
					}
					else {
						std::cout << "Material as color" << std::endl;
						float YEnvMap = 0.f;
						_materialMesh.fillColorsWithIndexMaterials();
					}
				}

				if (option360 == 0) _materialMesh.typeOfRender(
					MaterialMesh::RenderCategory::diffuseMaterials);
				else if (option360 == 1) _materialMesh.typeOfRender(
					MaterialMesh::RenderCategory::threesixtyDepth);
				else if (option360 == 2) _materialMesh.typeOfRender(
					MaterialMesh::RenderCategory::threesixtyMaterials);


			}
			ImGui::Separator();
			if (ImGui::CollapsingHeader("Exportation options")) {
				static int widthExport = window->w();
				static int heightExport = window->h();
				static bool flipH = false;
				static bool flipV = false;

				ImGui::InputInt("Width export", &widthExport);
				ImGui::InputInt("Height export", &heightExport);
				ImGui::Checkbox("Horizontal Flip ", &flipH);
				ImGui::Checkbox("Vertical Flip", &flipV);

				if (ImGui::Button("Export renders")) {

					auto itSet = _setsOfCameras.begin();
					unsigned int currentIndexCamera = 0;
					sibr::RenderTargetRGB32F rt(widthExport, heightExport);
					for (unsigned int i = 0; i < _setsOfCameras.size(); i++)
					{
						std::vector<InputCamera>& camerasCurrentSet = itSet->second;

						for (unsigned int j = 0; j < camerasCurrentSet.size(); j++)
						{
							InputCamera& cam = camerasCurrentSet[j];

							if (_materialMesh.typeOfRender() ==
								MaterialMesh::RenderCategory::diffuseMaterials)
								rt.clear(sibr::Vector3f(0.1f, 0.9f, 0.9f));
							else if (_materialMesh.typeOfRender() ==
								MaterialMesh::RenderCategory::threesixtyDepth ||
								_materialMesh.typeOfRender() ==
								MaterialMesh::RenderCategory::threesixtyMaterials
								)
								rt.clear(sibr::Vector3f(0.0f, 0.0f, 0.0f));

							rt.bind();
							glViewport(0, 0, rt.w(), rt.h());


							InputCamera camResized(cam);
							camResized.size(widthExport, heightExport);
							camResized.aspect(static_cast<float> (widthExport) /
								static_cast<float> (heightExport));

							renderMaterialMesh(camResized, false);

							sibr::ImageRGB im(widthExport, heightExport);
							rt.readBack(im, 0);

							if (flipH) im.flipH();
							if (flipV) im.flipV();

							std::string location = _outputPath + "/im" +
								std::to_string(currentIndexCamera) + ".png";
							std::cout << "Saved as " << location << std::endl;
							im.save(location);

							currentIndexCamera++;
						}
						itSet++;
					}
				}

			}
		}

	if ( _typeOfApp == CamEditMeshViewer::TypeOfApp::CamEditor)
	if (ImGui::Begin("Lights Editor")) {
		if (ImGui::Button("Save lights")) {
			writeLights();
		}
		if (ImGui::Button("Save lights separatively")) {
			writeLightsSeparatively();
		}
		/*if (ImGui::Button("Load cameras")) {
			//load();
			SIBR_WRG << "Uninmplemented for now." << std::endl;
		}*/
		if (ImGui::CollapsingHeader("Parameters")) {
			static int e = 0;
			ImGui::RadioButton("Sphere", &e, 0);
			//ImGui::RadioButton("Spot", &e, 1);
			ImGui::InputFloat("Radius", &_currentRadius, 0.4f, 128.f);

			ImGui::InputFloat("Radiance", &_currentRadiance, 0.4f, 128.f);

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Current set")) {
			if (ImGui::Button("Place light")) {
				InputCamera camera = interactCam->getCamera();
				_currentLightSpheres.push_back(
					LightSphere(camera.position() + camera.dir() * _currentDeltaLight
						, _currentRadius, _currentRadiance)
				);

				float* viewMat = interactCam->getCamera().view().data();
				float* projMat = interactCam->getCamera().proj().data();
				//gizmoMove->SetLocation(IGizmo::LOCATE_WORLD);
				gizmoMove->SetScreenDimension(window->w(),
					window->h());
				gizmoMove->SetCameraMatrix(viewMat, projMat);
				Transform3f posLight;
				posLight.position(_currentLightSpheres.back()._position);
				Matrix4f mLight(posLight.matrix());
				float* dataMLight = mLight.data();
				float* dataMLightT= mLight.transpose().data();

				for (int i = 0; i < 16; i++) {
					sharedDataGizmo[i] = dataMLightT[i];
				}

				gizmoMove->SetEditMatrix(sharedDataGizmo);
			}
			if (ImGui::Button("Validate")) {
				_lightSpheresValidated.insert(
					_lightSpheresValidated.end(),
					_currentLightSpheres.begin(),
					_currentLightSpheres.end());
				_currentLightSpheres.clear();
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear")) {
				_currentLightSpheres.clear();
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove last")) {
				if (!_currentLightSpheres.empty()) _currentLightSpheres.pop_back();
			}
			ImGui::SliderFloat("Delta camera",
				&_currentDeltaLight, _initialDeltaLight/100.f, _initialDeltaLight*20.f);
		}

		//static unsigned int currentSetToSnapping = 0;
		//static unsigned int currentCamToSnapping = 0;

		auto itSet = _lightSpheresValidated.begin();
		unsigned int currentIndexCamera = 0;
		for (unsigned int i = 0; i < _lightSpheresValidated.size(); i++)
		{

			std::string nameSet = std::string(std::string("Light") + std::to_string(i));
			if (ImGui::CollapsingHeader(nameSet.c_str())) {
				std::string deleteButton = std::string("Delete Light##") + nameSet;
				if (ImGui::Button(deleteButton.c_str())) {
					_lightSpheresValidated.erase(itSet);
					break;
				}
			}
			itSet++;
		}
	}
	ImGui::End();

}


void sibr::CamEditMeshViewer::addCamera(InputCamera& cam) {
	_currentList.push_back(cam);
}

void sibr::CamEditMeshViewer::renderCameras() {

	// We reset all the lines and points of the render to creat the news
	this->renderer->resetLinesAndPoints();

	// The set of all cameras which are no validated is the _currentList 
	// ( the cameras added manually by the user ) and the _interpolatedCameras
	// ( the cameras added automatically thanks to interpolation )
	std::vector<InputCamera> noValidated = _currentList;
	noValidated.insert(noValidated.end(), _interpolatedCameras.begin(),
		_interpolatedCameras.end());

	for (InputCamera&camera : noValidated)
	{
		if (!camera.isActive()) { continue; }
		this->renderer->addPoint(camera.position(), sibr::Vector3f(1, 1, 1));


		const std::vector<Vector2f> corners2D = { { 0,0 },{ camera.w() - 1,0 },
												  { camera.w() - 1,
													camera.h() - 1 },
												  { 0,camera.h() - 1 } };
		std::vector<Vector3f> dirs;
		for (const auto & c : corners2D) {
			dirs.push_back(sibr::CameraRaycaster::computeRayDir(camera, c + 0.5f*
				sibr::Vector2f(1, 1))
			);
		}

		std::vector<Vector3f> vertices;
		for (const auto & d : dirs) {
			vertices.push_back(camera.position() + _scaleCam * d);

			this->renderer->addPoint(camera.position() + _scaleCam * d,
				sibr::Vector3f(0.5, 0.5, 0.5));
			this->renderer->addLines({ camera.position(),
									   camera.position() + _scaleCam * d },
				sibr::Vector3f(0.5f, 0.5, 0.5));
		}

		// We bind the points of the square
		this->renderer->addLines({ vertices[0] , vertices[1] },
			sibr::Vector3f(1, 0, 1));
		this->renderer->addLines({ vertices[1] , vertices[2] },
			sibr::Vector3f(1, 0, 1));
		this->renderer->addLines({ vertices[2] , vertices[3] },
			sibr::Vector3f(1, 0, 1));
		this->renderer->addLines({ vertices[3] , vertices[0] },
			sibr::Vector3f(1, 0, 1));

	}

	// Now, we display the caremas which are already validated
	for (std::pair<cameraCategory, std::vector<InputCamera>> &set : _setsOfCameras)
	{
		// We define the color, green for render, red for reconstruction,
		// and blue for both
		sibr::Vector3f color;
		if (set.first == cameraCategory::both)
			color = sibr::Vector3f(0, 0, 1);
		if (set.first == cameraCategory::reconstruction)
			color = sibr::Vector3f(1, 0, 0);
		if (set.first == cameraCategory::render)
			color = sibr::Vector3f(0, 1, 0);
		for (InputCamera&camera : set.second)
		{
			if (!camera.isActive()) { continue; }

			this->renderer->addPoint(camera.position(), color);


			const std::vector<Vector2f> corners2D = { { 0,0 },{ camera.w() - 1,0 },
													  { camera.w() - 1,
														camera.h() - 1 },
													  { 0,camera.h() - 1 } };
			std::vector<Vector3f> dirs;
			for (const auto & c : corners2D) {
				dirs.push_back(sibr::CameraRaycaster::computeRayDir(camera, c + 0.5f*
					sibr::Vector2f(1, 1))
				);
			}

			std::vector<Vector3f> vertices;
			for (const auto & d : dirs) {
				vertices.push_back(camera.position() + _scaleCam * d);


				this->renderer->addPoint(camera.position() + _scaleCam * d,
					0.5f* color);

				this->renderer->addLines({ camera.position(),
										   camera.position() + _scaleCam * d },
					sibr::Vector3f(1, 0, 1));
			}

			this->renderer->addLines({ vertices[0] , vertices[1] },
				sibr::Vector3f(1, 0, 1));
			this->renderer->addLines({ vertices[1] , vertices[2] },
				sibr::Vector3f(1, 0, 1));
			this->renderer->addLines({ vertices[2] , vertices[3] },
				sibr::Vector3f(1, 0, 1));
			this->renderer->addLines({ vertices[3] , vertices[0] },
				sibr::Vector3f(1, 0, 1));


		}
	}
}

void sibr::CamEditMeshViewer::renderLights()
{

	static unsigned long int animation = 0;
	auto getSphere = [](const Vector3f& center, float radius,bool anim = false)
		-> std::vector<std::pair<sibr::Vector3f, sibr::Vector3f> > {

		std::vector<std::pair<sibr::Vector3f, sibr::Vector3f> > lines;
		for (int lat = -90; lat < 90; lat+=5) {
			for (int lgt = 0; lgt < 360; lgt+=30) {


				sibr::Vector3f pointA = cos(0.5* M_PI * lat / 90.0f)*
					(cos(2 * M_PI * lgt / 360.0f)*
						 sibr::Vector3f(1.0f, 0.f, 0.f) + sin(2 * M_PI * lgt / 360.0f)*
						 sibr::Vector3f(0.f, 1.f, 0.f))
					+ sin(0.5* M_PI * lat / 90.0f)*sibr::Vector3f(0.0f, 0.f, 1.f);

				sibr::Vector3f pointB = cos(0.5* M_PI * lat / 90.0f)*
					(cos(2 * M_PI * (lgt+30) / 360.0f)*
						 sibr::Vector3f(1.0f, 0.f, 0.f) + sin(2 * M_PI * (lgt+30) / 360.0f)*
						 sibr::Vector3f(0.f, 1.f, 0.f))
					+ sin(0.5* M_PI * lat / 90.0f)*sibr::Vector3f(0.0f, 0.f, 1.f);


				sibr::Vector4f pointA4(pointA.x(),pointA.y(),pointA.z(),1.f);
				sibr::Vector4f pointB4(pointB.x(),pointB.y(),pointB.z(),1.f);
				if (anim) {
					sibr::Transform3f t;
					t.rotation(static_cast<float>(animation), 0.f,0.f);

					sibr::Matrix4f m(t.matrix());

					pointA4 = m * pointA4;
					pointB4 = m * pointB4;
				}

				lines.push_back(std::pair<sibr::Vector3f, sibr::Vector3f>(
					radius*sibr::Vector3f(pointA4.xyz()) + center,
					radius*sibr::Vector3f(pointB4.xyz()) + center));
				

			}
		}

		return lines;
	};

	for (LightSphere s : _currentLightSpheres)
	{
		std::vector<std::pair<sibr::Vector3f, sibr::Vector3f> > linesSphere
			= getSphere(s._position,s._radius);
		for (std::pair<sibr::Vector3f, sibr::Vector3f> line : linesSphere) {
			this->renderer->addLines({ line.first, line.second },
				sibr::Vector3f(0.5f, 0.5f, 0.5f));
		}
	}

	for (LightSphere s : _lightSpheresValidated)
	{
		std::vector<std::pair<sibr::Vector3f, sibr::Vector3f> > linesSphere
			= getSphere(s._position,s._radius);
		for (std::pair<sibr::Vector3f, sibr::Vector3f> line : linesSphere) {
			this->renderer->addLines({ line.first, line.second },
				sibr::Vector3f(1.0f, 1.0f, 0.0f));
		}
	}
	
	if (_typeOfApp == CamEditMeshViewer::TypeOfApp::CamEditor) {
		InputCamera camera = interactCam->getCamera();
		LightSphere s(camera.position() + camera.dir() * _currentDeltaLight,
			_currentRadius, _currentRadiance);

		std::vector<std::pair<sibr::Vector3f, sibr::Vector3f> > linesSphere
			= getSphere(s._position, s._radius, true);
		for (std::pair<sibr::Vector3f, sibr::Vector3f> line : linesSphere) {
			this->renderer->addLines({ line.first, line.second },
				sibr::Vector3f(0.5f,0.5f, 0.5f));
		}
		animation++;
	}
	
}

void sibr::CamEditMeshViewer::renderGizmo() {
	if (_currentLightSpheres.size() > 0) {

		float* viewMat = interactCam->getCamera().view().data();
		float* projMat = interactCam->getCamera().proj().data();

		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix

		// Calculate The Aspect Ratio Of The Window
		gluPerspective(interactCam->getCamera().fovy(), 
					   interactCam->getCamera().aspect(),
					   interactCam->getCamera().znear(),
					   interactCam->getCamera().zfar()
		);

		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glLoadIdentity();									// Reset The Modelview Matrix
		gluLookAt(
			interactCam->getCamera().position().x(),
			interactCam->getCamera().position().y(),
			interactCam->getCamera().position().z(),
			interactCam->getCamera().dir().x(),
			interactCam->getCamera().dir().y(),
			interactCam->getCamera().dir().z(),
			interactCam->getCamera().up().x(),
			interactCam->getCamera().up().y(),
			interactCam->getCamera().up().z()
		);



		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(projMat);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(viewMat);



//		renderer->shaderLines.begin();
		gizmoMove->SetScreenDimension( window->w(),
				window->h() );
		gizmoMove->SetCameraMatrix(viewMat, projMat);
		gizmoMove->Draw();
//		renderer->shaderLines.end();
	}
}


unsigned int sibr::CamEditMeshViewer::getNumberOfCameras() {

	unsigned int count = 0;

	for (std::pair<cameraCategory, std::vector<InputCamera>> &set : _setsOfCameras) {
		count += static_cast<unsigned int> (set.second.size());
	}

	return count;
}

void sibr::CamEditMeshViewer::validateSpline() {

	// To validate a set, it's necessary that one the lists must be not empty
	if (!_interpolatedCameras.empty() || !_currentList.empty()) {
		float dist = std::max(0.8f, 1.05f*interactCam->getCamera().znear());
		std::vector<InputCamera>* camerasToValidate;
		// if several cameras are created thanks to interpolation tool,
		// we validate them
		if (!_interpolatedCameras.empty()) {
			camerasToValidate = &_interpolatedCameras;
		}
		else {
			// Else if none camera is interpolated, we validate the current list
			camerasToValidate = &_currentList;
		}

		// We insert the new set in the list of the editor
		_setsOfCameras.push_back(std::pair<cameraCategory, std::vector<InputCamera>>
			(_setCategory, *camerasToValidate));

		// Finally, we delete the two list containing the cameras which not validated
		_currentList.clear();
		_interpolatedCameras.clear();
	}
}


void sibr::CamEditMeshViewer::showInformationsList(std::vector<InputCamera>& l) {

	std::string infos = "";

	auto getZeros = [=](unsigned int size, unsigned int index) {
		unsigned sizeOfSize = 1;
		unsigned sizeOfIndex = 1;
		std::string zeros = "";
		while (size >= 10)
		{
			sizeOfSize++;
			size /= 10;
		}
		sizeOfSize = std::max(3u, sizeOfSize);
		while (index >= 10)
		{
			sizeOfIndex++;
			index /= 10;
		}
		for (unsigned int i = 0; i < sizeOfSize - sizeOfIndex; i++)
		{
			zeros += std::string("0");
		}
		return zeros;
	};


	for (unsigned int i = 0; i < l.size(); i++)
	{
		InputCamera& camera = l.at(i);
		infos += std::string("Cam") +
			getZeros(getNumberOfCameras(), i) +
			std::to_string(i) +
			sibr::CamEditMeshViewer::getInformationsCam(camera);
	}
}


std::string sibr::CamEditMeshViewer::getInformationsList(
	std::vector<InputCamera>& l,
	unsigned int offset) {
	std::string infos = "";

	// lambda function allows to get the correct number of 0 
	// ( for exemple if there are 100 cameras, we need 2 zeros for 
	// the first camera. ( 001 )
	auto getZeros = [=](unsigned int size, unsigned int index) {
		unsigned sizeOfSize = 1;
		unsigned sizeOfIndex = 1;
		std::string zeros = "";
		while (size >= 10)
		{
			sizeOfSize++;
			size /= 10;
		}
		while (index >= 10)
		{
			sizeOfIndex++;
			index /= 10;
		}
		for (unsigned int i = 0; i < sizeOfSize - sizeOfIndex; i++)
		{
			zeros += std::string("0");
		}
		return zeros;
	};


	for (unsigned int i = 0; i < l.size(); i++)
	{
		InputCamera& camera = l.at(i);
		infos += std::string("Cam") +
			getZeros(getNumberOfCameras(), offset + i) +
			std::to_string(offset + i) +
			sibr::CamEditMeshViewer::getInformationsCam(camera);
	}
	return infos;
}

std::string sibr::CamEditMeshViewer::getInformationsCam(InputCamera& cam) {
	std::string infos = std::string(" -D origin=") +
		std::to_string(cam.position()[0]) +
		std::string(",") +
		std::to_string(cam.position()[1]) +
		std::string(",") +
		std::to_string(cam.position()[2]) +
		std::string(" -D target=") +
		std::to_string(cam.position()[0] +
			cam.dir()[0]) +
		std::string(",") +
		std::to_string(cam.position()[1] +
			cam.dir()[1]) +
		std::string(",") +
		std::to_string(cam.position()[2] +
			cam.dir()[2]) +
		std::string(" -D up=") +
		std::to_string(cam.up()[0]) +
		std::string(",") +
		std::to_string(cam.up()[1]) +
		std::string(",") +
		std::to_string(cam.up()[2]) +
		std::string(" -D fov=") +
		std::to_string(cam.fovy()) +
		std::string(" -D clip=") +
		std::to_string(cam.znear()) +
		std::string(",") +
		std::to_string(cam.zfar()) +
		std::string("\n");
	return infos;
}


Eigen::Vector3f sibr::CamEditMeshViewer::catmull_rom_spline(float t,
	Eigen::Vector3f a,
	Eigen::Vector3f b,
	Eigen::Vector3f c,
	Eigen::Vector3f d,
	float tau)
{
	Eigen::Matrix4f M;
	M << 0.0f, 1.0f, 0.0f, 0.0f,
		-tau, 0.0f, tau, 0.0f,
		2.0f*tau, tau - 3.0f, 3.0f - 2.0f*tau, -tau,
		-tau, 2.0f - tau, tau - 2.0f, tau;


	Eigen::Matrix<float, 4, 3> X;
	X << a.transpose(),
		b.transpose(),
		c.transpose(),
		d.transpose();

	Eigen::Vector3f determined_position = Eigen::Matrix<float, 1, 4>
		(1.0f, t, t * t, t * t * t) * M * X;
	determined_position.transpose();
	return determined_position;
}

void sibr::CamEditMeshViewer::interpolate_cameras(unsigned int cameras_per_edge,
	float tau)
{
	if (!_currentList.empty()) {
		_interpolatedCameras.clear();
		const int LENGTH_SAMPLES = cameras_per_edge;

		std::vector<Eigen::Vector3f> control_points;
		control_points.reserve(_currentList.size() + 2);
		control_points.push_back(_currentList.front().position());
		for (InputCamera&camera : _currentList)
		{
			control_points.push_back(camera.position());
		}
		control_points.push_back(_currentList.back().position());

		for (unsigned int i = 0; i < control_points.size() - 3; ++i)
		{
			Eigen::Vector3f a_i(control_points[i + 0]);
			Eigen::Vector3f b_i(control_points[i + 1]);
			Eigen::Vector3f c_i(control_points[i + 2]);
			Eigen::Vector3f d_i(control_points[i + 3]);


			for (int j = 0; j < LENGTH_SAMPLES; ++j)
			{
				const float t = (j /*+ 1*/) / (0.0f + LENGTH_SAMPLES);
				const Eigen::Vector3f pos = catmull_rom_spline(t, a_i, b_i,
					c_i, d_i, 0.5);

				InputCamera cam(_currentList[i]);

				Transform3f transform;
				transform.position(pos);
				transform.rotation(_currentList[i].rotation().slerp(t,
					_currentList[i + 1].rotation()));
				//End of transform interpolation part


				cam.transform(transform);
				cam.fovy(t * _currentList[i].fovy() +
					(1.0f - t)* _currentList[i + 1].fovy());
				cam.aspect(t* _currentList[i].aspect() +
					(1.0f - t)*_currentList[i + 1].aspect());
				cam.zfar(t* _currentList[i].zfar() +
					(1.0f - t)*_currentList[i + 1].zfar());
				cam.znear(t* _currentList[i].znear() +
					(1.0f - t)*_currentList[i + 1].znear());

				_interpolatedCameras.push_back(cam);
			}
		}
	}

}

void sibr::CamEditMeshViewer::makeCamerasAround(unsigned int nbNewCameras,
	float distanceMax)
{
	static std::default_random_engine generator;
	std::vector<InputCamera> listToUse;
	std::uniform_real_distribution<float> randomDirection(
		-_maximumAngle / 2.f, _maximumAngle / 2.f);
	std::uniform_real_distribution<float> randomHorizontaleValue(-1.f, 1.f);
	std::uniform_real_distribution<float> randomVerticaleValue(-1.f, 1.f);
	if (!_interpolatedCameras.empty()) {
		listToUse = _interpolatedCameras;
	}
	else if (!_currentList.empty()) {
		listToUse = _currentList;
	}

	_interpolatedCameras.clear();
	for (InputCamera&camera : listToUse)
	{
		_interpolatedCameras.push_back(camera);
		for (unsigned int i = 0; i < nbNewCameras; ++i)
		{
			//Random position
			float horizontale = randomHorizontaleValue(generator);
			float verticale = randomVerticaleValue(generator);

			float angleAlpha = 2.f * static_cast<float>(M_PI)*static_cast<float>(i) /
				static_cast<float>(nbNewCameras);


			float angleBeta = -static_cast<float>(M_PI) / 2.f + acosf(verticale);

			Eigen::AngleAxis<float> rotateAlpha(angleAlpha, camera.up());
			Eigen::Matrix3f matrixAlpha(rotateAlpha.toRotationMatrix());

			Eigen::AngleAxis<float> rotateBeta(angleBeta, camera.right());
			Eigen::Matrix3f matrixBeta(rotateBeta.toRotationMatrix());

			sibr::Vector3f position(camera.dir());
			position = matrixAlpha * position;
			position = matrixBeta * position;
			position *= _maximumDistance;

			float angleGamma = randomDirection(generator);


			InputCamera cam(camera);

			Vector3f newDir(position.dot(cam.dir()) * cam.dir()
				+ (position.dot(cam.right())* cam.right()));
			newDir.normalize();
			Transform3f cameraTransform(camera.transform());
			cameraTransform.translate(position);
			cam.transform(cameraTransform);
			cam.setLookAt(cam.position(), cam.position() + newDir, cam.up());

			Transform3f cameraTransform2(cam.transform());
			cameraTransform2.rotate(angleGamma*cam.right());
			cam.transform(cameraTransform2);

			_interpolatedCameras.push_back(cam);
		}
	}

}

void sibr::CamEditMeshViewer::writeCameras() {

	// We will create two files.
	// On the one hand, a file containing the cameras of the render, and on the
	// On the other hand, a file containing the cameras of the reconstructions

	std::ofstream fileRender(_outputPath + "/camerasRender.lookat",
		std::ios::out | std::ios::trunc);
	/*std::ofstream fileRender("E:\\smorgent\\Scene\\roomJulien\\camerasRender.lookat",
	  std::ios::out | std::ios::trunc);*/
	std::ofstream fileReconstruction(_outputPath + "/camerasReconstruction.lookat",
		std::ios::out | std::ios::trunc);

	// We initialize two offsets. The offsets are using for the number for each
	// carema in the file.
	unsigned int offsetRender = 0;
	unsigned int offsetReconstruction = 0;

	// Safety: if validate was not press, the cameras won't be saved.
	// So if there are no cameras to save, try to validate.
	if (_setsOfCameras.empty()) {
		SIBR_WRG << "There were no validated sets, trying to validate any available camera list..."
			<< std::endl;
		validateSpline();
	}

	if (fileRender && fileReconstruction) {

		// For each set of cameras ( containing the cameras and the type of cameras )
		// We writte in the associated file
		for (std::pair<cameraCategory, std::vector<InputCamera>> &set : _setsOfCameras)
		{

			// Render side
			if (set.first == cameraCategory::render || set.first == cameraCategory::both) {
				std::string data = sibr::CamEditMeshViewer::getInformationsList(
					set.second, offsetRender);
				fileRender << data;
				offsetRender += static_cast<unsigned int> (set.second.size());
			}

			// Reconstruction side
			if (set.first == cameraCategory::reconstruction ||
				set.first == cameraCategory::both) {
				std::string data = sibr::CamEditMeshViewer::getInformationsList(
					set.second, offsetReconstruction);
				fileReconstruction << data;
				offsetReconstruction += static_cast<unsigned int> (set.second.size());
			}

		}
		fileRender.close();
		fileReconstruction.close();
	}
	else
		std::cerr << "Impossible to open this file !" << std::endl;

}

void sibr::CamEditMeshViewer::writeLightsSeparatively() {

	unsigned int counterLights = 0;

	// Safety: if validate was not press, the cameras won't be saved.
	// So if there are no cameras to save, try to validate.
	if (_lightSpheresValidated.empty()) {
		SIBR_WRG << "There were no validated sets, trying to validate any available camera list..."
			<< std::endl;
		validateSpline();
	}

	for (auto light : _lightSpheresValidated)
	{

	std::ofstream fileLights(_outputPath + "/outLight"+ std::to_string(counterLights) + ".xml",
		std::ios::out | std::ios::trunc);
	if (fileLights) {
		
		std::string data;
		data.append("<scene version=\"0.6.0\">\n");
		data.append("\t<shape type=\"sphere\">\n");
		data.append("\t\t<point name=\"center\" x=\"");
		data.append(std::to_string(light._position.x()));
		data.append("\" y=\"");
		data.append(std::to_string(light._position.y()));
		data.append("\" z=\"");
		data.append(std::to_string(light._position.z()));
		data.append("\"/>\n");
		data.append("\t\t<float name=\"radius\" value=\"");
		data.append(std::to_string(light._radius));
		data.append("\"/>\n\n");
		data.append("\t\t<emitter type=\"area\">\n");
		data.append("\t\t\t<spectrum name=\"radiance\" value=\"");
		data.append(std::to_string(light._radiance));
		data.append("\"/>\n");
		data.append("\t\t</emitter>\n");
		data.append("\t</shape>\n");
		data.append("</scene>");
		fileLights << data;
		fileLights.close();
		counterLights++;
	}
	else
		std::cerr << "Impossible to open this file !" << std::endl;
	}

}

void sibr::CamEditMeshViewer::writeLights() {

	std::ofstream fileLights(_outputPath + "/outLights.xml",
		std::ios::out | std::ios::trunc);


	unsigned int offsetLight = 0;

	// Safety: if validate was not press, the cameras won't be saved.
	// So if there are no cameras to save, try to validate.
	if (_lightSpheresValidated.empty()) {
		SIBR_WRG << "There were no validated sets, trying to validate any available camera list..."
			<< std::endl;
		validateSpline();
	}

	if (fileLights) {

		std::string data;
		data.append("<scene version=\"0.6.0\">\n");
		for (auto light : _lightSpheresValidated)
		{

			data.append("\t<shape type=\"sphere\">\n");
			data.append("\t\t<point name=\"center\" x=\"");
			data.append(std::to_string(light._position.x()));
			data.append("\" y=\"");
			data.append(std::to_string(light._position.y()));
			data.append("\" z=\"");
			data.append(std::to_string(light._position.z()));
			data.append("\"/>\n");
			data.append("\t\t<float name=\"radius\" value=\"");
			data.append(std::to_string(light._radius));
			data.append("\"/>\n\n");
			data.append("\t\t<emitter type=\"area\">\n");
			data.append("\t\t\t<spectrum name=\"radiance\" value=\"");
			data.append(std::to_string(light._radiance));
			data.append("\"/>\n");
			data.append("\t\t</emitter>\n");
			data.append("\t</shape>\n");
		}
		data.append("</scene>");
		fileLights << data;
		fileLights.close();
	}
	else
		std::cerr << "Impossible to open this file !" << std::endl;

}

void sibr::CamEditMeshViewer::listenKey() {
	if (input.global().key().isPressed(sibr::Key::B)) {
		InputCamera camToAdd = interactCam->getCamera();
		InputCamera& refToCam = camToAdd;
		addCamera(refToCam);
	}
	if (input.global().mouseButton().isPressed(sibr::Mouse::Button1)) {
		Vector2i posMouse = input.global().mousePosition();
		if (gizmoMove->OnMouseDown(posMouse.x(), posMouse.y()))
			gizmoIsUsing = true;
	}
	if (input.global().mouseButton().isReleased(sibr::Mouse::Button1)) {
		Vector2i posMouseA = input.global().mousePosition();
		gizmoMove->OnMouseUp(posMouseA.x(), posMouseA.y());
		gizmoIsUsing = false;
	}

	if (_currentLightSpheres.size() > 0) {
		Vector2i pos = input.global().mousePosition();
		gizmoMove->OnMouseMove(pos.x(), pos.y());
		Transform3f newTranslation;
		newTranslation.position(sharedDataGizmo[12],
			sharedDataGizmo[13],
			sharedDataGizmo[14]);
		Matrix4f mTranslation = newTranslation.matrix();


		_currentLightSpheres.back()._position = Vector3f(sharedDataGizmo[12],
			sharedDataGizmo[13],
			sharedDataGizmo[14]);
	}
}

