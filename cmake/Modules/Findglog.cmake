# FindGlog lib
# --------
#
# Find Glog
#
# Find all include and library
#
# ::
#
#   GLOG_INCLUDE_DIR   	- where to find glog/config.h
#   GLOG_LIBRARY      	- glog.lib
#   GLOG_FOUND          - True if gloglib found.
#
#
#=============================================================================
if(NOT GLOG_DIR)
    set(GLOG_DIR "$ENV{GLOG_DIR}" CACHE PATH "GLOG_DIR root directory")
endif()

if(GLOG_DIR)
	file(TO_CMAKE_PATH ${GLOG_DIR} GLOG_DIR)
endif()

# Path to include dirs
FIND_PATH(GLOG_INCLUDE_DIR
		NAMES "glog/logging.h" 
		PATHS
			"${GLOG_DIR}/include/" # modify this to adapt according to OS/compiler			
			/usr/include/
			/usr/local/include/
)
	

#Add libraries
FIND_LIBRARY(GLOG_LIBRARY
		NAMES glog
		PATHS
		${GLOG_DIR}/Lib
		/usr/lib64/
		/usr/lib/
		/usr/local/lib64/
		/usr/local/lib/
	)
	
set(GLOG_INCLUDE_DIR ${GLOG_INCLUDE_DIR})
set(GLOG_LIBRARY ${GLOG_LIBRARY})


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Levmar
	REQUIRED_VARS GLOG_INCLUDE_DIR GLOG_LIBRARY 
	FAIL_MESSAGE "glog wasn't found correctly. Set GLOG_DIR to the root SDK installation directory."
	)
if(NOT GLOG_FOUND)
	set(GLOG_DIR "" CACHE STRING "Path to gflags install directory")
endif()
