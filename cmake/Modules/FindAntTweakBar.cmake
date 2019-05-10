## Try to find the AntTwearBar library
## Once done this will define
##
##  	AntTweakBar_FOUND 			- system has AntTweakBar
##  	AntTweakBar_INCLUDE_DIR 	- The AntTweakBar include directory
##  	AntTweakBar_LIBRARY 		- The libraries needed to use AntTweakBar
##		AntTweakBar_DYNAMIC_LIB		- DLLs for windows

if(NOT AntTweakBar_DIR)
    set(AntTweakBar_DIR "$ENV{AntTweakBar_DIR}" CACHE PATH "AntTweakBar_DIR root directory")
endif()
if(AntTweakBar_DIR)
	file(TO_CMAKE_PATH ${AntTweakBar_DIR} AntTweakBar_DIR)
endif()


# Path to include dirs
FIND_PATH(AntTweakBar_INCLUDE_DIR
		NAMES "AntTweakBar.h" 
		PATHS
			"${AntTweakBar_DIR}/include" # modify this to adapt according to OS/compiler			
	)
		
#Add libraries
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(AntTweakBar_LIBRARY
		NAMES AntTweakBar64
		PATHS
		${AntTweakBar_DIR}/lib
	)
else()
	FIND_LIBRARY(AntTweakBar_LIBRARY
	NAMES AntTweakBar
	PATHS
	${AntTweakBar_DIR}/lib
	)
endif()

if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		file(GLOB AntTweakBar_DYNAMIC_LIB "${AntTweakBar_DIR}/lib/AntTweakBar64.dll")
		message("${AntTweakBar_DYNAMIC_LIB}")
	else()
		file(GLOB AntTweakBar_DYNAMIC_LIB "${AntTweakBar_DIR}/lib/AntTweakBar.dll")
		message("${AntTweakBar_DYNAMIC_LIB}")
	endif()
	
	if(NOT AntTweakBar_DYNAMIC_LIB)
		message("AntTweakBar_DYNAMIC_LIB is missing...")
	endif()
	
	set(AntTweakBar_DYNAMIC_LIB ${AntTweakBar_DYNAMIC_LIB} CACHE PATH "Windows dll location" FORCE)
endif()

# message("INcludes : ${AntTweakBar_INCLUDE_DIR})
set(AntTweakBar_INCLUDE_DIR ${AntTweakBar_INCLUDE_DIR})
set(AntTweakBar_LIBRARY ${AntTweakBar_LIBRARY})
set(AntTweakBar_DYNAMIC_LIB ${AntTweakBar_DYNAMIC_LIB})

# MARK_AS_ADVANCED(AntTweakBar_DYNAMIC_LIB AntTwearBar_INCLUDE_DIRS AntTweakBar_LIBRARY)	
 
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AntTweakBar
	REQUIRED_VARS AntTweakBar_INCLUDE_DIR AntTweakBar_LIBRARY AntTweakBar_DYNAMIC_LIB
	FAIL_MESSAGE "AntTweak wasn't found correctly. Set AntTweakBar_DIR to the root SDK installation directory."
	)
if(NOT AntTweakBar_FOUND)
	set(AntTweakBar_DIR "" CACHE STRING "Path to AntTweakBar install directory")
endif()
  
