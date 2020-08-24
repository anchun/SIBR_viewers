#########################################################
# Include doxygen documentation target
#########################################################
option(BUILD_DOCUMENTATION "build doxygen documentation ('Build' CompileDocs target, and find the compiled docs in sibr/docs/Documentation.html)" OFF)
if(BUILD_DOCUMENTATION)
	set(DOXYGEN_CHECKED_VERSION "1.8.17")
	find_package(Doxygen)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen not found, unable to generate documentation.")
	elseif(DOXYGEN_VERSION VERSION_LESS DOXYGEN_CHECKED_VERSION)
		message(FATAL_ERROR "Doxygen version is less than ${DOXYGEN_CHECKED_VERSION} (Current version is ${DOXYGEN_VERSION}).")
	else()
		set(DOXY_DOC_DEST_DIR               ${CMAKE_CURRENT_SOURCE_DIR}) 	    							## used in the doxyfile.in
		set(DOXY_DOC_INPUT_ROOT_DIRS        "${CMAKE_HOME_DIRECTORY}/src/ ${CMAKE_HOME_DIRECTORY}/docs ${CMAKE_CURRENT_BINARY_DIR}/generated/")    				## used in the doxyfile.in
		set(DOXY_DOC_EXCLUDE_PATTERNS_DIRS  "${DOXY_DOC_EXCLUDE_PATTERNS_DIRS} ${CMAKE_HOME_DIRECTORY}/src/core/graphics/imgui ${CMAKE_HOME_DIRECTORY}/src/core/assets/xatlas ${CMAKE_HOME_DIRECTORY}/src/core/raycaster/nanoflann ${CMAKE_HOME_DIRECTORY}/src/core/system/rapidxml-1.13") ## used in the doxyfile.in
		set(DOXY_DOC_COMMON_IMG_PATH        "${CMAKE_CURRENT_SOURCE_DIR}/img ${CMAKE_HOME_DIRECTORY}/src/projects/")

		## Generating documentation pages with variables
		file(GLOB_RECURSE doc_files "../pages/*.in")
		foreach(filename ${doc_files})
			message(STATUS "Generating ${filename}...")
			get_filename_component(output_filename ${filename} NAME_WLE)
			message(STATUS "Output in ${CMAKE_CURRENT_BINARY_DIR}/generated/${output_filename}...")
			configure_file(${filename} ${CMAKE_CURRENT_BINARY_DIR}/generated/${output_filename} @ONLY)
		endforeach()

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
