## Written by clement.riant@inria.fr

if(GLM_DIR)
	file(TO_CMAKE_PATH ${GLM_DIR} GLM_DIR)
	if(GLM_INCLUDE_DIR)
		set(GLM_FOUND TRUE)
	else()
		FIND_PATH(GLM_INCLUDE_DIR 
		NAMES glm.hpp
		PATHS
			${GLM_DIR}
			${GLM_DIR}/glm
		)
		
		if(GLM_INCLUDE_DIR)
			set(GLM_FOUND TRUE)
		endif()
	  
		if(NOT GLM_FOUND AND GLM_FIND_REQUIRED)
		  message(FATAL_ERROR "could NOT find GLM")
		else()
			MARK_AS_ADVANCED(GLM_INCLUDE_DIR)
		endif()
	endif()
else()
		message("Specify GLM_DIR")
endif()