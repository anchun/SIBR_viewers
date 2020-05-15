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
		set(DOXY_DOC_INPUT_ROOT_DIRS        "${CMAKE_HOME_DIRECTORY}/src/ ${CMAKE_HOME_DIRECTORY}/docs ${CMAKE_CURRENT_BINARY_DIR}/generated/")    				## used in the doxyfile.in
		set(DOXY_DOC_EXCLUDE_PATTERNS_DIRS  "${DOXY_DOC_EXCLUDE_PATTERNS_DIRS} ${CMAKE_HOME_DIRECTORY}/src/core/graphics/imgui ${CMAKE_HOME_DIRECTORY}/src/core/assets/xatlas ${CMAKE_HOME_DIRECTORY}/src/core/raycaster/nanoflann ${CMAKE_HOME_DIRECTORY}/src/core/system/rapidxml-1.13") ## used in the doxyfile.in
		set(DOXY_DOC_COMMON_IMG_PATH        "${CMAKE_CURRENT_SOURCE_DIR}/img ${CMAKE_HOME_DIRECTORY}/src/projects/")

		## Generate project list in the projects page
		configure_file(Projects.dox.in ${CMAKE_CURRENT_BINARY_DIR}/generated/Projects.dox @ONLY)

		configure_file(layout.xml.in		${CMAKE_BINARY_DIR}/layout.xml			@ONLY)
		configure_file(doxyfile.in			${CMAKE_BINARY_DIR}/doxyfile			@ONLY)
		configure_file(doxyfileUpdate.cmake.in	${CMAKE_BINARY_DIR}/doxyfileUpdate.cmake @ONLY)
		add_custom_target(CompileDocs ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/doxyfileUpdate.cmake
			COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/doxyfile"
			WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
			COMMENT "Building user's documentation into doxyDoc build dir..."
			)
	endif()
endif()
