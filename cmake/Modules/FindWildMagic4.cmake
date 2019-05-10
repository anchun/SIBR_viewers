## Written by clement.riant@inria.fr

if(WildMagic4_DIR)
	file(TO_CMAKE_PATH ${WildMagic4_DIR} WildMagic4_DIR)
	if(WildMagic4_INCLUDE_DIR AND WildMagic4_LIBRARIES)
		set(WildMagic4_FOUND TRUE)
	else()
		FIND_PATH(WildMagic4_INCLUDE_DIR 
		NAMES Wm4Capsule3.h
		PATHS
			${WildMagic4_DIR}/include
		)
		
		find_library(WildMagic4_LIBRARY_RELEASE 
			NAMES 			Wm4Foundation_vc110_x64 
			PATHS 			${WildMagic4_DIR}/lib
		)
		find_library(WildMagic4_LIBRARY_DEBUG 
			NAMES 			Wm4Foundation_vc110_x64_d
			PATHS 			${WildMagic4_DIR}/lib
		)
		
		set(WildMagic4_LIBRARIES optimized ${WildMagic4_LIBRARY_RELEASE} debug ${WildMagic4_LIBRARY_DEBUG})
		
		if(WildMagic4_INCLUDE_DIR AND WildMagic4_LIBRARIES)
			set(WildMagic4_FOUND TRUE)
		endif()
	  
		if(WildMagic4_FOUND)
			if(NOT WildMagic4_FIND_QUIETLY)
				message(STATUS "Found WildMagic4: ${WildMagic4_LIBRARIES}")
			endif()
		else()
			if(WildMagic4_FIND_REQUIRED)
			  message(FATAL_ERROR "could NOT find WildMagic4")
			endif()
		endif()
		MARK_AS_ADVANCED(WildMagic4_INCLUDE_DIR WildMagic4_LIBRARIES)
		
	endif()
else()
		message("Specify WildMagic4_DIR")
endif()