## Try to find the TBB library
## Once done this will define
##  	TBB_FOUND 		- system has TBB
##  	TBB_INCLUDE_DIR - The TBB include directory
##  	TBB_LIBRARY 	- The libraries needed to use TBB

if(NOT TBB_DIR)
    set(TBB_DIR "$ENV{TBB_DIR}" CACHE PATH "TBB root directory")
endif()

if(TBB_DIR)
	file(TO_CMAKE_PATH ${TBB_DIR} TBB_DIR)
endif()

set(PROGRAMFILESx86 "PROGRAMFILES(x86)")

# FIND_PATH(TBB_INCLUDE_DIR
# 	NAMES tbb.h
# 	PATHS
# 		${TBB_DIR}/tbb2017_U7/include/tbb
# 		## linux
# 		/usr
# 		/usr/local
# 		/opt/local
# 		## windows
# 		"$ENV{PROGRAMFILES}/TBB"
# 		"$ENV{${PROGRAMFILESx86}}/TBB"
# 		"$ENV{ProgramW6432}/TBB"
# 	PATH_SUFFIXES include
# )

set (TBB_INCLUDE_DIR ${TBB_DIR}/tbb2017_U7/include)

FIND_LIBRARY(TBB_LIBRARY
	NAMES tbb
	PATHS
		${TBB_DIR}/tbb2017_U7/bin64
	PATH_SUFFIXES "64"
)

set(TBB_LIBRARIES ${TBB_LIBRARY})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(TBB
    REQUIRED_VARS TBB_INCLUDE_DIR TBB_LIBRARIES
	FAIL_MESSAGE "TBB wasn't found correctly. Set TBB_DIR to the root SDK installation directory."
)

if(NOT TBB)
	set(TBB_DIR "" CACHE STRING "Path to TBB install directory")
endif()
