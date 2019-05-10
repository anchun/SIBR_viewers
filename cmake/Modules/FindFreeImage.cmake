## Written by clement.riant@inria.fr


if(NOT FreeImage_DIR)
    set(FreeImage_DIR "$ENV{FreeImage_DIR}" CACHE PATH "FreeImage root directory")
endif()
if(FreeImage_DIR)
	file(TO_CMAKE_PATH ${FreeImage_DIR} FreeImage_DIR)
endif()

## set default verbosity
if(NOT FreeImage_VERBOSE)
	set(FreeImage_VERBOSE OFF)
else()
	message(STATUS "Start to FindFreeImage.cmake :")
endif()


## set the LIB POSTFIX to find in a right directory according to what kind of compiler we use (32/64bits)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # Size in bytes!
	set(FreeImage_SEARCH_LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
else()  # Size in bytes!
	set(FreeImage_SEARCH_LIB_POSTFIX "" CACHE STRING "suffix for 32/64 dir placement")
endif()
if(FreeImage_SEARCH_LIB_POSTFIX)
	mark_as_advanced(FreeImage_SEARCH_LIB_POSTFIX)
	if(FreeImage_VERBOSE)
		message(STATUS "   find_library will search inside lib${FreeImage_SEARCH_LIB_POSTFIX} directory (can be changed with FreeImage_SEARCH_LIB_POSTFIX)")
	endif()
endif()

if(FreeImage_DIR)

	file(TO_CMAKE_PATH ${FreeImage_DIR} FreeImage_DIR)
	if(FreeImage_INCLUDE_DIR AND FreeImage_LIBRARIES)
		set(FreeImage_FOUND TRUE)
	else()
		FIND_PATH(FreeImage_INCLUDE_DIR 
		NAMES FreeImage.h
		PATHS ${FreeImage_DIR}/include
		)
		
		find_library(FreeImage_LIBRARY_RELEASE 
			NAMES 			FreeImage 
			PATHS 			${FreeImage_DIR}/lib${FreeImage_SEARCH_LIB_POSTFIX}
            PATH_SUFFIXES	Release
		)
		find_library(FreeImage_LIBRARY_DEBUG 
			NAMES 			FreeImaged 
			PATHS 			${FreeImage_DIR}/lib${FreeImage_SEARCH_LIB_POSTFIX}
            PATH_SUFFIXES	Debug
		)
		
		set(FreeImage_LIBRARIES optimized ${FreeImage_LIBRARY_RELEASE} debug ${FreeImage_LIBRARY_DEBUG})
		
		if(FreeImage_INCLUDE_DIR AND FreeImage_LIBRARIES)
			set(FreeImage_FOUND TRUE)
		endif()
	  
		if(FreeImage_FOUND)
			if(NOT FreeImage_FIND_QUIETLY)
				message(STATUS "Found FreeImage: ${FreeImage_LIBRARIES}")
			endif()
		else()
			if(FreeImage_FIND_REQUIRED)
			  message(FATAL_ERROR "could NOT find FreeImage")
			endif()
		endif()
		MARK_AS_ADVANCED(FreeImage_INCLUDE_DIR FreeImage_LIBRARIES)
		
	endif()
else()
		message("Specify FreeImage_DIR")
endif()