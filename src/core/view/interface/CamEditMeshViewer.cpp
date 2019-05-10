#include "CamEditMeshViewer.h"
#include <core/graphics/Window.hpp>
#include <core/assets/InputCamera.hpp>
#include <core/graphics/Mesh.hpp>
#include <core/raycaster/Raycaster.hpp>
#include <core/view/InteractiveCameraHandler.hpp>
#include <core/raycaster/CameraRaycaster.hpp>
#include <cmath>


sibr::CamEditMeshViewer::CamEditMeshViewer()
  : MeshViewer()
{

}


sibr::CamEditMeshViewer::CamEditMeshViewer(const sibr::Vector2i & screenRes,
                                           const sibr::Mesh & mesh, 
                                           const bool launchRenderingLoop,
                                           const std::string name)
  : MeshViewer(screenRes, mesh, launchRenderingLoop ),
  _name (name)
{
}

sibr::CamEditMeshViewer::~CamEditMeshViewer()
{
}

void sibr::CamEditMeshViewer::setMainMesh(const sibr::Mesh & mesh, 
                                          sibr::Mesh::RenderMode mode, 
                                          bool updateCam, 
                                          bool _setupRaycaster)
{
	setMainMesh(*window, mesh, mode, updateCam, _setupRaycaster);
}

void sibr::CamEditMeshViewer::setMainMesh(sibr::Window & win, 
                                          const sibr::Mesh & mesh, 
                                          sibr::Mesh::RenderMode mode, 
                                          bool updateCam, 
                                          bool _setupRaycaster)
{
	sibr::Mesh::Ptr meshGL = std::make_shared<sibr::Mesh>(true);
	meshGL->vertices(mesh.vertices());
	meshGL->triangles(mesh.triangles());
	if (mesh.hasNormals()) {
		meshGL->normals(mesh.normals());
	}

	renderer->resetMeshes();
	renderer->addMesh(meshGL, mode);

	if (updateCam) {
		interactCam->setup(meshGL, win.viewport());
	}

	if (_setupRaycaster) {
		raycaster = std::make_shared<sibr::Raycaster>();
		raycaster->init();
		raycaster->addMesh(*meshGL);
	}
}

void sibr::CamEditMeshViewer::render(const sibr::Viewport & viewport,
                                     const sibr::Camera & eye )
{
  viewport.bind();
	viewport.clear(sibr::Vector3f(0.9f, 0.9f, 0.9f));
	renderer->render(eye);
	interactCam->onRender(viewport);
	fpsCounter.update(true);

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
		window->swapBuffer();
	}
}

void sibr::CamEditMeshViewer::renderLoop(sibr::Window & window)
{

	bool doLoop = true;

	while (doLoop && window.isOpened() ) {
		sibr::Input::poll();

		if (sibr::Input::global().key().isPressed(sibr::Key::Escape)) {
			doLoop = false;
		}

		interactCam->update(sibr::Input::global(),1/60.0f, window.viewport());
		
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
	if (otherWindow.get() && !window.get() ) {
		window = otherWindow;
	}

	while (window->isOpened()) {
		sibr::Input::poll();

		if (sibr::Input::global().key().isPressed(sibr::Key::Escape)) {
			window->close();
		}

		interactCam->update(sibr::Input::global(),1/60.0f, window->viewport());
		render();
	}

	reset();
}

void sibr::CamEditMeshViewer::renderLoop( const std::function
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

		interactCam->update(input,1/60.0f, window->viewport());

		f(this);
   //ImGui::ShowDemoWindow();
    onGUI();
    listenKey();
    renderCameras();

		if (!customRendering) {
			render();
		}
	}

	reset();

}

void sibr::CamEditMeshViewer::onGUI() {

		if (ImGui::Begin("Camera Editor")) {
			if (ImGui::Button("Save cameras")) {
				//save();
        writeCameras();
        //std::cout << "Cameras writted" << std::endl;
			}
      ImGui::SameLine();
			if (ImGui::Button("Load cameras")) {
				//load();
			}
      if (ImGui::CollapsingHeader("Parameters")) {
      ImGui::InputFloat("Size of cameras", &_scaleCam, 0.4f, 128.f);
      ImGui::InputInt("Number of interpolations", &_numberInterpolations, 2, 50);
      ImGui::InputInt("Number of cameras around", &_numberCamerasAround, 2, 50);
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
          //std::cout << "cameraInterpolated " << std::endl;
        }
      ImGui::SameLine();
        if (ImGui::Button("Make cameras around")) {
          interpolate_cameras(_numberInterpolations, 0.5f);
          makeCamerasAround(_numberCamerasAround, _maximumDistance);
          //std::cout << "cameraInterpolated and created around base" << std::endl;
        }
        if (ImGui::Button("Validate")) {
          validateSpline();
          //std::cout << "splineValidated" << std::endl;
          showInformationsList(_controlCameras);
          //std::cout << "informationsShowed" << std::endl;
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
          InputCamera& camToSnapping = camerasCurrentSet[currentCamToSnapping];
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
            std::string snapButton = std::string("Snap##Direct") + nameCam;
            if (ImGui::Button(snapButton.c_str())) {
              InputCamera currentTrackBallCamera(interactCam->getCamera());
              currentTrackBallCamera.setLookAt(cam.position(),
                cam.position() + cam.dir(), cam.up());
			  interactCam->fromCamera(currentTrackBallCamera);
            }
            ImGui::SameLine();
            std::string deleteButton = std::string("Delete Cam##") + nameCam;
          
            if (ImGui::Button(deleteButton.c_str())) {
            auto itCam = camerasCurrentSet.begin();
            unsigned int index = 0; 
            while (index < j) { itCam++; index++; }
            camerasCurrentSet.erase(itCam);
            if (camerasCurrentSet.empty())
              _setsOfCameras.erase(itSet);
            break;
            }

            /*if (ImGui::Button(deleteButton.c_str())) {

            }*/
          }
          currentIndexCamera++;
        }
      }
      itSet++;
    }
  }
		ImGui::End();

}


void sibr::CamEditMeshViewer::reset()
{
	if (inChargeOfWindow) {
		interactCam.reset();
		renderer.reset();
		raycaster.reset();
		window.reset();
	}
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

    // We are about to display each camera
    for (InputCamera&camera : noValidated )
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
    for (std::pair<cameraCategory,std::vector<InputCamera>> &set : _setsOfCameras)
    { 
      // We define the color, green for render, red for reconstruction,
      // and blue for both
      sibr::Vector3f color;
      if (set.first == cameraCategory::both)
        color = sibr::Vector3f(0, 0, 1);
      if (set.first == cameraCategory::reconstruction )
        color = sibr::Vector3f(1, 0 , 0);
      if (set.first == cameraCategory::render)
        color = sibr::Vector3f(0, 1 , 0);
    for (InputCamera&camera : set.second) 
      {
        if (!camera.isActive()) { continue; }

        this->renderer->addPoint(camera.position(), color );


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

unsigned int sibr::CamEditMeshViewer::getNumberOfCameras() {

  unsigned int count = 0;

  for (std::pair<cameraCategory,std::vector<InputCamera>> &set: _setsOfCameras){
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


void sibr::CamEditMeshViewer::showInformationsList(std::vector<InputCamera>& l){

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
    //std::cout << infos << std::endl;
  }
  //std::cout << infos << std::endl;
}


std::string sibr::CamEditMeshViewer::getInformationsList(
                                     std::vector<InputCamera>& l,
                                     unsigned int offset ) {
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
      getZeros(getNumberOfCameras(), offset+i) +
      std::to_string(offset+i) +
      sibr::CamEditMeshViewer::getInformationsCam(camera);
      //std::cout << infos << std::endl;
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
        transform.rotation(_currentList[i].rotation()
                                         .slerp(t, 
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
                                                float distanceMax )
{
  static std::default_random_engine generator;
  std::vector<InputCamera> listToUse ; 
  std::uniform_real_distribution<float> randomDirection(
    -_maximumAngle / 2.f, _maximumAngle / 2.f);
  std::uniform_real_distribution<float> randomHorizontaleValue(-1.f,1.f);
  std::uniform_real_distribution<float> randomVerticaleValue(-1.f,1.f);
  if (!_interpolatedCameras.empty()){
    listToUse = _interpolatedCameras;
  }
  else if (!_currentList.empty()) {
    listToUse= _currentList;
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
        static_cast<float>(nbNewCameras); //+ 
                         //2*acosf(horizontale)/nbNewCameras;


      float angleBeta = -static_cast<float>(M_PI)/2.f + acosf(verticale);

      Eigen::AngleAxis<float> rotateAlpha(angleAlpha, camera.up());
      Eigen::Matrix3f matrixAlpha (rotateAlpha.toRotationMatrix());

      Eigen::AngleAxis<float> rotateBeta(angleBeta, camera.right());
      Eigen::Matrix3f matrixBeta (rotateBeta.toRotationMatrix());
      
      sibr::Vector3f position(camera.dir());
      position = matrixAlpha * position;
      position = matrixBeta * position;
      position *= _maximumDistance;

      float angleGamma = randomDirection(generator);
      

      InputCamera cam(camera);

      Vector3f newDir (position.dot(cam.dir()) * cam.dir()
        + (position.dot(cam.right())* cam.right()));
      newDir.normalize();
      Transform3f cameraTransform(camera.transform());
      //cameraTransform.rotate(angleGamma*axes);
      //cameraTransform.rotation(newDir);
      //cameraTransform.rotate(angleGamma*axes);
      //std::cout << newDir[0] << "," << newDir[1] << "," << newDir[2] << std::endl;
      cameraTransform.translate(position);
      cam.transform(cameraTransform);
      cam.setLookAt(cam.position(), cam.position() + newDir, cam.up());

      Transform3f cameraTransform2(cam.transform());
      cameraTransform2.rotate(angleGamma*cam.right());
      cam.transform(cameraTransform2);

      //std::cout << angleGamma << std::endl;


      _interpolatedCameras.push_back(cam);
    }
  }

}

void sibr::CamEditMeshViewer::writeCameras() {

  // We will create two files.
  // On the one hand, a file containing the cameras of the render, and on the
  // On the other hand, a file containing the cameras of the reconstruction
  std::ofstream fileRender("camerasRender.lookat",
    std::ios::out | std::ios::trunc);
  std::ofstream fileReconstruction("camerasReconstruction.lookat",
    std::ios::out | std::ios::trunc);

  // We initialize two offsets. The offsets are using for the number for each
  // carema in the file.
  unsigned int offsetRender = 0;
  unsigned int offsetReconstruction = 0;


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
      if (set.first == cameraCategory::reconstruction || set.first == cameraCategory::both) {
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

void sibr::CamEditMeshViewer::listenKey() {

  if (input.global().key().isPressed(sibr::Key::B)) {
    InputCamera camToAdd = interactCam->getCamera();
    InputCamera& refToCam = camToAdd;
    addCamera(refToCam);
    //std::cout << "CameraAdded" << std::endl;
  }/*
  if (input.global().key().isPressed(sibr::Key::S)) {
    writeCameras();
    //std::cout << "Cameras writted" << std::endl;
  }
  if (input.global().key().isPressed(sibr::Key::I)) {
    interpolate_cameras(10, 0.5f);
    //std::cout << "cameraInterpolated " << std::endl;
    validateSpline();
    //std::cout << "splineValidated" << std::endl;
    //showInformationsList(_controlCameras);
    //std::cout << "informationsShowed" << std::endl;
  }
  if (input.global().key().isPressed(sibr::Key::T)) {
    showInformationsList(_currentList);
    //std::cout << "informations CurrentList Showed" << std::endl;
  }*/

}
