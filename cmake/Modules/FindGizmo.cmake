## Important Note:
## This is not an official Find*cmake. It has been written for searching through
## a custom path (GIZMO_DIR) before checking elsewhere.
##
## FindGIZMO.cmake
## Find GIZMO's includes and library
##
## This module defines :
## 	[in] 	GIZMO_DIR, The base directory to search for GIZMO (as cmake var or env var)
## 	[out] 	GIZMO_INCLUDE_DIR where to find GIZMO.h
## 	[out] 	GIZMO_LIBRARIES, GIZMO_LIBRARY, libraries to link against to use GIZMO
## 	[out] 	GIZMO_FOUND, If false, do not try to use GIZMO.
##


if(NOT GIZMO_DIR)
    set(GIZMO_DIR "$ENV{GIZMO_DIR}" CACHE PATH "GIZMO root directory")
endif()
if(GIZMO_DIR)
	file(TO_CMAKE_PATH ${GIZMO_DIR} GIZMO_DIR)
endif()


## set the LIB POSTFIX to find in a right directory according to what kind of compiler we use (32/64bits)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(GIZMO_SEARCH_LIB "lib64")
	set(GIZMO_SEARCH_BIN "bin64")
	set(GIZMO_SEARCH_LIB_PATHSUFFIXE "x64")
else()
	set(GIZMO_SEARCH_LIB "lib32")
	set(GIZMO_SEARCH_BIN "bin32")
	set(GIZMO_SEARCH_LIB_PATHSUFFIXE "x86")
endif()

set(PROGRAMFILESx86 "PROGRAMFILES(x86)")

FIND_PATH(GIZMO_INCLUDE_DIR
	NAMES libGizmo/IGizmo.h
	PATHS
		${GIZMO_DIR}
		## linux
		/usr
		/usr/local
		/opt/local
		## windows
		"$ENV{PROGRAMFILES}/GIZMO"
		"$ENV{${PROGRAMFILESx86}}/GIZMO"
		"$ENV{ProgramW6432}/GIZMO"
	PATH_SUFFIXES include
)

FIND_LIBRARY(GIZMO_LIBRARY
	NAMES libGIZMO
	PATHS
		${GIZMO_DIR}/${GIZMO_SEARCH_LIB}
		${GIZMO_DIR}/lib
		## linux
		/usr/${GIZMO_SEARCH_LIB}
		/usr/local/${GIZMO_SEARCH_LIB}
		/opt/local/${GIZMO_SEARCH_LIB}
		/usr/lib
		/usr/local/lib
		/opt/local/lib
		## windows
		"$ENV{PROGRAMFILES}/GIZMO/${GIZMO_SEARCH_LIB}"
		"$ENV{${PROGRAMFILESx86}}/GIZMO/${GIZMO_SEARCH_LIB}"
		"$ENV{ProgramW6432}/GIZMO/${GIZMO_SEARCH_LIB}"
		"$ENV{PROGRAMFILES}/GIZMO/lib"
		"$ENV{${PROGRAMFILESx86}}/GIZMO/lib"
		"$ENV{ProgramW6432}/GIZMO/lib"
	PATH_SUFFIXES ${GIZMO_SEARCH_LIB_PATHSUFFIXE}
)
set(GIZMO_LIBRARIES ${GIZMO_LIBRARY})

MARK_AS_ADVANCED(GIZMO_INCLUDE_DIR GIZMO_LIBRARIES)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GIZMO
	REQUIRED_VARS GIZMO_INCLUDE_DIR GIZMO_LIBRARIES
	FAIL_MESSAGE "GIZMO wasn't found correctly. Set GIZMO_DIR to the root SDK installation directory."
)

if(NOT GIZMO_FOUND)
	set(GIZMO_DIR "" CACHE STRING "Path to GIZMO install directory")
endif()
