cmake_minimum_required(VERSION 2.8)

#Project
project(GlobaMatting)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

#---- add path to boost windows
#SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "../../../build/win3rdParty/MSVC11/Boost/boost_1_55_0/")

#find opencv 
SET(OpenCV_DIR "../../build/win3rdParty/MSVC11/OpenCV/opencv/build/")
FIND_PACKAGE(OpenCV) 

#config & find boost
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

#Boost
SET(BOOST_ROOT "../../build/win3rdParty/MSVC11/Boost/boost_1_55_0/")
SET(BOOST_LIBRARYDIR "../../build/win3rdParty/MSVC11/Boost/boost_1_55_0/lib64/")
FIND_PACKAGE(Boost 1.55.0 COMPONENTS filesystem system REQUIRED)

#openmp
find_package(OpenMP)



#include directories
include_directories(
	${Boost_INCLUDE_DIR}
	${Boost_LIBRARY_DIR}
	${OpenCV_INCLUDE_DIR}
)

link_directories(
	${Boost_LIBRARY_DIR}
)

add_definitions( -DBOOST_ALL_DYN_LINK  )

#source files list
file(
	GLOB_RECURSE
	header_files
	src/*.h
)
file(
	GLOB_RECURSE
	source_files
	src/*.cpp
)

#executable
add_executable(
	GlobaMatting
	${header_files}
	${source_files}
)

	
#link libraries
TARGET_LINK_LIBRARIES(
	GlobaMatting
	${Boost_LIBRARIES}
	${OpenCV_LIBS}
)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_OPTIONS ${OpenMP_CXX_FLAGS} LINK_FLAGS ${OpenMP_CXX_FLAGS})

install( TARGETS GlobaMatting DESTINATION "../../install/bin")
