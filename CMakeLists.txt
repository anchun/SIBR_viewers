CMAKE_MINIMUM_REQUIRED(VERSION 3.16.3)
set (CMAKE_SYSTEM_VERSION 10.0.15063.0 CACHE TYPE INTERNAL FORCE)
PROJECT(sibr_projects)

## Check the cmake version
set(CHECKED_VERSION "3.16.4")
if (CMAKE_VERSION VERSION_GREATER CHECKED_VERSION)
    message(WARNING "Untested version of cmake. If you checked everything is working properly, please update ${CHECKED_VERSION} in the main CmakeLists.txt with the version you tested.")
endif()

## Include cmake stuff (functions/macros) : Modules files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
if (WIN32) #Seb: use custom modules only for windows (some don't work on Fedora)
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/win32)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

## To maintain cmake versions compatibilities
include(cmake_policies)
setPolicies()

## Allow C++11 + other flags
include(CheckCXXCompilerFlag)
get_filename_component(currentBuildTool ${CMAKE_BUILD_TOOL} NAME_WE)	# tool that can launch the native build system. returned value may be the full path
if(${currentBuildTool} MATCHES "(msdev|devenv|nmake|MSBuild)")
    add_definitions(/W3 /DNOMINMAX /MP -D_USE_MATH_DEFINES)# /D_ITERATOR_DEBUG_LEVEL=1 because you need all external DLl to compile with this flag too
    set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo;Release;Debug" CACHE STRING "" FORCE)
    set(CMAKE_CXX_STANDARD 14)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
    #set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo;Release" CACHE STRING "" FORCE)
    # "Warning level 4 generates a lot of warning messages for STL headers and is not currently recommended for that reason."
    # from support.microsoft.com
elseif(${currentBuildTool} MATCHES "(make|gmake)")
    add_definitions("-Wall -Wno-unknown-pragmas -Wno-sign-compare -g -std=c++11 -D__forceinline=\"inline\ __attribute__((always_inline))\"")
	# CHECK_CXX_COMPILER_FLAG("-std=gnu++11" COMPILER_SUPPORTS_CXX11)
	# CHECK_CXX_COMPILER_FLAG("-std=gnu++0x" COMPILER_SUPPORTS_CXX0X)
	# if(COMPILER_SUPPORTS_CXX11)
	# 	add_definitions(-std=gnu++11)
	# elseif(COMPILER_SUPPORTS_CXX0X)
	# 	add_definitions(-std=gnu++0x)
	# else()
	# 	message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	# endif()
elseif(APPLE) ## \todo TODO: do a better test and send error on unsupported c++11 compiler
    add_definitions(-std=c++11 -stdlib=libc++)
endif()

## Default install path; user doesn't have to specify
## will cause the GUI will be not updated, and unavailable for changing by user
#set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/OUTPUTS/${CONFIG_TYPES}") # /!\ We don't know the CONFIG_TYPES used at this step...
SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
MESSAGE(STATUS "Install path set to ${CMAKE_INSTALL_PREFIX}.")
set(INSTALL_STANDALONE ON)

SET(CMAKE_OUTPUT_BIN ${CMAKE_CURRENT_SOURCE_DIR}/install/bin)

## Set default build output binaries (used also in sub CMakeLists.txt) :
set(BIN_BUILT_DIR "bin")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ARCHI_BUILT_DIR "x64")
	set(LIB_BUILT_DIR "lib64")
else()
	set(ARCHI_BUILT_DIR "x86")
	set(LIB_BUILT_DIR "lib")
endif()
if(DEFINED CMAKE_BUILD_TYPE) ## for mono config type (make/nmake/ninja based)
	# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/OUTPUTS/${ARCHI_BUILT_DIR}/${CMAKE_BUILD_TYPE}/${LIB_BUILT_DIR})
	# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/OUTPUTS/${ARCHI_BUILT_DIR}/${CMAKE_BUILD_TYPE}/${LIB_BUILT_DIR})
	# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/OUTPUTS/${ARCHI_BUILT_DIR}/${CMAKE_BUILD_TYPE}/${BIN_BUILT_DIR})
	# set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/OUTPUTS/${ARCHI_BUILT_DIR}/${CMAKE_BUILD_TYPE}/${BIN_BUILT_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_OUTPUT_BIN})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_OUTPUT_BIN})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_OUTPUT_BIN})
  set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_OUTPUT_BIN})

	if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
		set(CMAKE_DEBUG_POSTFIX "_d")
	elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
		set(CMAKE_DEBUG_POSTFIX "_rwdi")
	elseif(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
		set(CMAKE_DEBUG_POSTFIX "_msr")
	endif()
endif()
foreach(CONFIG_TYPES ${CMAKE_CONFIGURATION_TYPES}) ## for multi config types (MSVC based)
	string(TOUPPER ${CONFIG_TYPES} CONFIG_TYPES_UC)
	# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPES_UC} ${CMAKE_BINARY_DIR}/OUTPUTS/${CONFIG_TYPES}/${LIB_BUILT_DIR})
	# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPES_UC} ${CMAKE_BINARY_DIR}/OUTPUTS/${CONFIG_TYPES}/${LIB_BUILT_DIR})
	# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPES_UC} ${CMAKE_BINARY_DIR}/OUTPUTS/${CONFIG_TYPES}/${BIN_BUILT_DIR})
	# set(CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG_TYPES_UC} ${CMAKE_BINARY_DIR}/OUTPUTS/${CONFIG_TYPES}/${BIN_BUILT_DIR})

  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPES_UC} ${CMAKE_OUTPUT_BIN})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPES_UC} ${CMAKE_OUTPUT_BIN})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPES_UC} ${CMAKE_OUTPUT_BIN})
  set(CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG_TYPES_UC} ${CMAKE_OUTPUT_BIN})
	if(${CONFIG_TYPES} MATCHES "Debug")
		set(CMAKE_DEBUG_POSTFIX "_d")
	elseif(${CONFIG_TYPES} MATCHES "RelWithDebInfo")
		set(CMAKE_RELWITHDEBINFO_POSTFIX "_rwdi")
	elseif(${CONFIG_TYPES} MATCHES "MinSizeRel")
		set(CMAKE_MINSIZEREL_POSTFIX "_msr")
	endif()
endforeach()


# Settings for RPATH
if (NOT WIN32)
	# Default config of Fedora at INRIA has no LD_LIBRARY_PATH (for security reasons I guess)
	# So at least I had "./" in RPATH and found link paths
	#set(CMAKE_SKIP_RPATH TRUE)
	#SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

	SET(CMAKE_INSTALL_RPATH "$ORIGIN")
	#SET(CMAKE_INSTALL_RPATH "./")
	#SET(CMAKE_INSTALL_RPATH "./:/usr/lib64/:/usr/lib/:/usr/local/lib64/:/usr/local/lib/") # This one causes be a problem -> a "default" version of libGL (swrast) is located in /usr/lib64 and was selected instead of nvidia one (in /usr/lib64/nividia)

	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()


set(SIBR_PROGRAMARGS "" CACHE STRING "Default program arguments used in Visual Studio target properties")
if ("${SIBR_PROGRAMARGS}" STREQUAL "")
  if (DEFINED ENV{SIBR_PROGRAMARGS})
    set(SIBR_PROGRAMARGS "$ENV{SIBR_PROGRAMARGS}" CACHE STRING "Default program arguments used in Visual Studio target properties" FORCE)
    message( STATUS "Using program options found in environment variable 'SIBR_PROGRAMARGS' => '${SIBR_PROGRAMARGS}'")
  else()
    message(
      "Note you can provide default program options for Visual Studio target properties by either setting"
      " a value for the cmake cached variable 'SIBR_PROGRAMARGS' or by setting a new environment "
      "variable 'SIBR_PROGRAMARGS'")
  endif()
endif()

#add_custom_target(BuildAndInstall DEPENDS INSTALL)

# IF(MSVC)
#    ADD_DEFINITIONS(/arch:AVX)
# ENDIF(MSVC)

## Include all projects
set(SIBR_PROJECTS_SUBPAGE_REF "")
set(DOXY_APP_SPECIFIC_IMG_PATH "")
set(DOXY_DOC_EXCLUDE_PATTERNS_DIRS "")
ADD_SUBDIRECTORY(src)


## handle documentation
ADD_SUBDIRECTORY(docs/doxygen)
