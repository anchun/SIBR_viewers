# FindCereslib
# --------
#
# Find Ceres
#
# Find include and library
#
# ::
#
#   CERES_INCLUDE_DIR   	- where to find ceres/config.h
#   CERES_LIBRARY      		- ceres.lib
#   CERES_FOUND          	- True if cereslib found.
#
#
#=============================================================================
if(NOT CERES_DIR)
    set(CERES_DIR "$ENV{CERES_DIR}" CACHE PATH "CERES_DIR root directory")
endif()

if(CERES_DIR)
	file(TO_CMAKE_PATH ${CERES_DIR} CERES_DIR)
endif()

# Path to include dirs
FIND_PATH(CERES_INCLUDE_DIR
		NAMES "ceres/ceres.h" 
		PATHS
			"${CERES_DIR}/include/" # modify this to adapt according to OS/compiler			
#			/usr/include/
#			/usr/local/include/
)
	

#Add libraries
FIND_LIBRARY(CERES_LIBRARY
		NAMES ceres
		PATHS
		${CERES_DIR}/lib
#		/usr/lib64/
#		/usr/lib/
#		/usr/local/lib64/
#		/usr/local/lib/
	)
	
set(CERES_INCLUDE_DIR ${CERES_INCLUDE_DIR})
set(CERES_LIBRARY ${CERES_LIBRARY})


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Levmar
	REQUIRED_VARS CERES_INCLUDE_DIR CERES_LIBRARY 
	FAIL_MESSAGE "ceres wasn't found correctly. Set CERES_DIR to the root SDK installation directory."
	)
if(NOT CERES_FOUND)
	set(CERES_DIR "" CACHE STRING "Path to ceres install directory")
endif()
