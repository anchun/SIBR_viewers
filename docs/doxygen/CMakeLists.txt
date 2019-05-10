#########################################################
# Include doxygen documentation target
#########################################################
option(BUILD_DOCUMENTATION "build doxygen documentation ('Build' CompileDocs target, and find the compiled docs in sibr/docs/Documentation.html)" OFF)
if(BUILD_DOCUMENTATION)
	find_package(Doxygen)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen not found, unable to generate documentation")
	else()
		set(DOXY_DOC_DEST_DIR               ${CMAKE_CURRENT_SOURCE_DIR}) 	    							## used in the doxyfile.in
		set(DOXY_DOC_INPUT_ROOT_DIRS        "${CMAKE_HOME_DIRECTORY}/src/ ${CMAKE_HOME_DIRECTORY}/docs")    				## used in the doxyfile.in
		set(DOXY_DOC_EXCLUDE_PATTERNS_DIRS  "${CMAKE_HOME_DIRECTORY}/src/core/graphics/imgui ${CMAKE_HOME_DIRECTORY}/src/core/system/rapidxml-1.13") ## used in the doxyfile.in
		set(DOXY_DOC_COMMON_IMG_PATH        "${CMAKE_CURRENT_SOURCE_DIR}/img")
							## used in the doxyfile.in
		## \todo TODO parse sub dirs to append all sub doc dirs from sub projects
		## typicaly for storing images or doxygen pages...
		## message(STATUS ${DOXY_APP_SPECIFIC_IMG_PATH})
		configure_file(doxyfile.in 		${CMAKE_BINARY_DIR}/doxyfile 		 @ONLY)
		configure_file(doxyfileUpdate.cmake.in	${CMAKE_BINARY_DIR}/doxyfileUpdate.cmake @ONLY)
		add_custom_target(CompileDocs ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/doxyfileUpdate.cmake
			COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/doxyfile"
			WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
			COMMENT "Building user's documentation into doxyDoc build dir..."
			)
	endif()
endif()
