

################################################################################
# This CMakeLists.txt manages which projects should be built and add their     #
# dependencies.                                                                #
################################################################################

include(CMakeDependentOption)
option(BUILD_SIBR "Build core libs of SIBR (sibr_system, sibr_graphics, sibr_view and sibr_assets)" ON)
option(BUILD_VIDEO "Build vbr lib in core of SIBR" OFF)
set(USE_RENDERER_API_GLUT ON)

if (BUILD_IBR_TFGL_INTEROP)
  find_package(CUDA REQUIRED)
endif()

if (BUILD_IBR_TORCHGL_INTEROP)
  find_package(CUDA REQUIRED)
endif()

## DEPS ##
include(dependencies)		## Map/bind 3rdParty/external dependencies packages to cmake
include(FindOpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

#https://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()


# '_' underscores are used to sort them in the IDE
set(SIBR_FOLDER           "___core___")
set(SIBR_INSTALL_FOLDER   "_install_")


# TODO: SV
# some of these libraries in extlibs might need to be recompiled using MSVC14.1 (VS 2017)
# incluing here only glfw at this time
# libraries in this folder that have been already recompiled or gotten an compatible library:
# * glfw 3.2.1 (using MSVS 2015 official release)
if (MSVC11)
    include_directories(../extlibs/win64-msvc2012/glfw-3.1.2/include/)
    link_directories(../extlibs/win64-msvc2012/glfw-3.1.2/lib/)

elseif (MSVC14)
    include_directories(../extlibs/win64-msvc2017/glfw-3.2.1/include/)
    link_directories(../extlibs/win64-msvc2017/glfw-3.2.1/lib/)

else()
    # setting VS2012 as default
    message("WARNING using glfw 3.1.2 compiled with MSVC11 (Visual Studio 2012)")
    include_directories(../extlibs/win64-msvc2012/glfw-3.1.2/include/)
    link_directories(../extlibs/win64-msvc2012/glfw-3.1.2/lib/)
endif()

## For GENERATE_EXPORT_HEADER cmake macro of common/core lib used by sub-projects (used when target SHARED under windows)
include_directories(${CMAKE_BINARY_DIR}/include)

include_directories(.)

if (BUILD_SIBR)
  add_subdirectory(core/system)
  add_subdirectory(core/graphics)
  add_subdirectory(core/renderer)
  add_subdirectory(core/raycaster)
  add_subdirectory(core/view)
  add_subdirectory(core/assets)
  add_subdirectory(core/imgproc)
endif()

if (BUILD_SIBR AND BUILD_VIDEO)
  add_subdirectory(core/video)
endif()

set(SIBR_PROJECTS_FOLDER           "${CMAKE_CURRENT_SOURCE_DIR}/projects/")
SUBDIRLIST(SUBDIRS ${SIBR_PROJECTS_FOLDER})

FOREACH(subdir ${SUBDIRS})

  message("project ${subdir} found")
  set(PROJECT_NAME "BUILD_IBR_${subdir}")
  string(TOUPPER ${PROJECT_NAME} PROJECT_NAME)
  if(${${PROJECT_NAME}})
    message(STATUS ${PROJECT_NAME} " is ON")
    if(EXISTS "${SIBR_PROJECTS_FOLDER}/${subdir}/apps/")
      add_subdirectory("${SIBR_PROJECTS_FOLDER}/${subdir}/apps/")
    endif()
    if(EXISTS "${SIBR_PROJECTS_FOLDER}/${subdir}/renderer/")
      add_subdirectory("${SIBR_PROJECTS_FOLDER}/${subdir}/renderer/")
    endif()
    if(EXISTS "${SIBR_PROJECTS_FOLDER}/${subdir}/preprocess/")
      add_subdirectory("${SIBR_PROJECTS_FOLDER}/${subdir}/preprocess/")
    endif()
    if(EXISTS "${SIBR_PROJECTS_FOLDER}/${subdir}/documentation/" AND BUILD_DOCUMENTATION)
      set(DOXY_APP_SPECIFIC_IMG_PATH_LOCAL "${DOXY_APP_SPECIFIC_IMG_PATH_LOCAL} ${SIBR_PROJECTS_FOLDER}/${subdir}/documentation/")
    endif()
  else()
    if(EXISTS "${SIBR_PROJECTS_FOLDER}/${subdir}/renderer/" OR EXISTS "${SIBR_PROJECTS_FOLDER}/${subdir}/apps/")
      option(${PROJECT_NAME} ON)
      message(STATUS ${PROJECT_NAME} " is OFF")
    endif()
  endif()
ENDFOREACH()
  
set(DOXY_APP_SPECIFIC_IMG_PATH "${DOXY_APP_SPECIFIC_IMG_PATH_LOCAL}" PARENT_SCOPE)

if (BUILD_IBR_TFGL_INTEROP)
  add_subdirectory(projects/tfgl_interop/renderer/custom_ops)
endif()
