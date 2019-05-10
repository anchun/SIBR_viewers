## Try to find the MapmapCPU library
## Once done this will define
##  	MapmapCPU_FOUND 		- system has MapmapCPU
##  	MapmapCPU_INCLUDE_DIR 	- The MapmapCPU include directory

if(NOT MapmapCPU_DIR)
    set(MapmapCPU_DIR "$ENV{MapmapCPU_DIR}" CACHE PATH "MapmapCPU root directory")
endif()

if(MapmapCPU_DIR)
	file(TO_CMAKE_PATH ${MapmapCPU_DIR} MapmapCPU_DIR)
endif()

set(PROGRAMFILESx86 "PROGRAMFILES(x86)")

# message (WARNING "before calling find path")

# FIND_PATH(MapmapCPU_INCLUDE_DIR
# 	NAMES header/color.h
# 	PATHS
# 		${MapmapCPU_DIR}
# 		## linux
# 		/usr
# 		/usr/local
# 		/opt/local
# 		## windows
# 		"$ENV{PROGRAMFILES}/MapmapCPU"
# 		"$ENV{${PROGRAMFILESx86}}/MapmapCPU"
# 		"$ENV{ProgramW6432}/MapmapCPU"
#         #PATH_SUFFIXES include
# )
# 
# message (WARNING "path to mapmap/full.h ${MapmapCPU_INCLUDE_DIR}")

set (MapmapCPU_INCLUDE_DIR ${MapmapCPU_DIR})
# message (WARNING "setting MapmapCPU_INCLUDE_DIR to ${MapmapCPU_INCLUDE_DIR}")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(MapmapCPU
    REQUIRED_VARS MapmapCPU_INCLUDE_DIR
	FAIL_MESSAGE "MapmapCPU wasn't found correctly. Set MapmapCPU_DIR to the root SDK installation directory."
)

if(NOT MapmapCPU)
	set(MapmapCPU_DIR "" CACHE STRING "Path to MapmapCPU install directory")
endif()
