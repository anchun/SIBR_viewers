#pragma once
#include "core/view/interface/MeshViewer.h"
#include <core/assets/InputCamera.hpp>
#include <random>

namespace sibr {
  class SIBR_VIEW_EXPORT CamEditMeshViewer : public sibr::MeshViewer
  {

  public:

    CamEditMeshViewer();
    CamEditMeshViewer(
      const sibr::Vector2i & screenRes,
      const sibr::Mesh & mesh = sibr::Mesh(),
      const bool launchRenderingLoop = false,
      const std::string name = "Unknown"
    );
    ~CamEditMeshViewer();

    void setMainMesh(
      const sibr::Mesh & mesh,
      sibr::Mesh::RenderMode mode = sibr::Mesh::FillRenderMode,
      bool updateCam = true,
      bool setupRaycaster = true
    );

    void setMainMesh(
      sibr::Window & win,
      const sibr::Mesh & mesh,
      sibr::Mesh::RenderMode mode = sibr::Mesh::FillRenderMode,
      bool updateCam = true,
      bool setupRaycaster = true
    );

    void render(const sibr::Viewport & viewport, const sibr::Camera & eye);
    void render(const sibr::Viewport & viewport);
    void render(const sibr::Camera & eye);
    void render();

    void renderLoop(sibr::Window & window);

    void renderLoop(std::shared_ptr<sibr::Window> window);
    void renderLoop(const std::function<void(CamEditMeshViewer*)> & f =
      [](CamEditMeshViewer* m) {}, bool customRendering = false);


    void reset();

    //!
    //! The function corresponding to the imGUI interface
    //! @return void
    //!
    void onGUI();

    //!
    //! Allows to add an InputCamera in the current list
    //! @return void
    //! @param cam The camera to place
    //!
    void addCamera(InputCamera& cam);

    //!
    //! Allows to render the cameras on the windows through several lines
    //! @return void
    //!
    void renderCameras();


    //!
    //! Allows to get the number of the validated cameras in the scene 
    //! @return unsigned int the number of the validated cameras in the scene 
    //!
    unsigned int getNumberOfCameras();

    //!
    //! Allows to validate a spline from the cameras of the current list.
    //! The current list will be emptied and the elements inserted into
    //! the interpolated list
    //! @return void
    //!
    void validateSpline();

    //!
    //! Allows to get the informations of an inputCamera in Mitsuba format
    //! @return string containing the information
    //! @param cam The camera to inspect
    //!
    std::string getInformationsCam(InputCamera& cam);


    //!
    //! Allows to show the informations of a list of several cameras
    //! @return void
    //! @param cam l The list of cameras
    //!
    void showInformationsList(std::vector<InputCamera>& l);

    //!
    //! Allows to get the informations of a list of several cameras
    //! @return string containing the informations about cameras 
    //! @param cam l The list of cameras
    //! @param offset The index of the first camera in the list
    //!
    std::string getInformationsList(std::vector<InputCamera>& l, unsigned int offset);

    //!
    //! Allows to do an interpolation of catmull rom spline (code coming from
    //! FRIBR source)
    //! @return Eigen::Vector3f vector containing the interpolated position
    //! @param t the interpolated variable ( between 0 and 1 )
    //! @param a the previous point before the two points interpolated
    //! @param b the first point interpolated
    //! @param c the second point interpolated
    //! @param d the succeeding point after the two points interpolated
    //! @param tau variable for accuracy
    //!
    static Eigen::Vector3f catmull_rom_spline(float t, Eigen::Vector3f a,
      Eigen::Vector3f b, Eigen::Vector3f c,
      Eigen::Vector3f d, float tau);

    //!
    //! Allows to do an interpolation between points of the current list 
    //! @return void
    //! @param cameras_per_edge the number of cameras to create between each 
    //! couple of points
    //! @param tau variable for accuracy 
    //!
    void interpolate_cameras(unsigned int cameras_per_edge, float tau);

    void makeCamerasAround(unsigned int nbNewCameras, float distanceMax);

    //!
    //! Allows to save cameras in the file
    //! @return void
    //!
    void writeCameras();

    //!
    //! Allows to listen the keyboard and do actions
    //! @return void
    //!
    void listenKey();

    //!
    //! @enum mapper::cameraCategory
    //! @brief enum class representing the type of camera
    //!
    enum class cameraCategory
    {
      reconstruction,
      render,
      both,
      unknown
    };

    std::vector<InputCamera> _controlCameras;

    std::vector<InputCamera> _interpolatedCameras; //!< The list contains the currents interpolated cameras
                                                   //!<

    std::vector<InputCamera> _aroundCameras; //!< The list contains the cameras created around the
                                             //!< unvalidated cameras.
                                             //!<

    std::vector<InputCamera> _currentList; //!< The list contains the currents cameras
                                           //!<

    std::vector<std::pair<cameraCategory, std::vector<InputCamera>>> _setsOfCameras; //!< A list of pair.
                                        //!< The first element of the pair is the category of the set
                                        //!< The second element of the pair is the vector of cameras of the set
                                        //!<

    std::string _name; //!< the name of the scene
                       //!<

    cameraCategory _setCategory = cameraCategory::both; //!< The category used to for the set validation
                                                             //!<

    float _scaleCam = 0.8f; //!< the scale of the display of the cameras
                                     //!<

    int _numberInterpolations = 10; //!< the numbers of new cameras created when the currents cameras
                                    //!< are interpolated
                                    //!<

    int _numberCamerasAround = 10; //!< the number of new cameras created around each points
                                           //!< 

    float _maximumDistance = 10.f; //!< the maximum distance between the basic camera
                                   //!< and the camera created around it

    float _maximumAngle = 20.f; //!< the maximum angle variation between the basic camera
                                      //!< and the camera created around it

  };
}
