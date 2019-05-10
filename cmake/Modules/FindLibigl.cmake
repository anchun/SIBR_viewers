## Try to find the Libigl 
## Once done this will define
##
##  	Libigl_FOUND 			- system has Libigl
##  	Libigl_INCLUDE_DIR 		- The Libigl include directory


if(NOT Libigl_DIR)
    set(Libigl_DIR "$ENV{Libigl_DIR}" CACHE PATH "Libigl_DIR root directory")
endif()
if(Libigl_DIR)
	file(TO_CMAKE_PATH ${Libigl_DIR} Libigl_DIR)
endif()


# Path to include dirs
FIND_PATH(Libigl_INCLUDE_DIR
		NAMES "/igl/arap.h" 
		PATHS
			"${Libigl_DIR}/libigl-master/include" # modify this to adapt according to OS/compiler			
			"${Libigl_DIR}/include" # modify this to adapt according to OS/compiler			
	)


# message("INcludes : ${Libigl_INCLUDE_DIR})
set(Libigl_INCLUDE_DIR ${Libigl_INCLUDE_DIR})

# 
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libigl
	REQUIRED_VARS Libigl_INCLUDE_DIR 
	FAIL_MESSAGE "libigl wasn't found correctly. Set Libigl_DIR to the root SDK installation directory."
	)
if(NOT Libigl_FOUND)
	set(Libigl_DIR "" CACHE STRING "Path to libigl install directory")
endif()
  
