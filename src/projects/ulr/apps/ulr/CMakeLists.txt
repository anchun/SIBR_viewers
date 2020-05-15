
project(SIBR_ulr_app)

file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")
source_group("Source Files" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}

	${Boost_LIBRARIES}
	${ASSIMP_LIBRARIES}
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
  ${OpenCV_LIBRARIES}
	#${CGAL_LIBRARIES}			## from IBR_spixelWarp needed dependencies
	#${SuiteSparse_LIBRARIES}	## from IBR_spixelWarp needed dependencies
	sibr_view
	sibr_assets
	sibr_ulr
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "__projects__/__ulr__/__apps__")

#define postfix for various debug/release
set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX "_d"
	RELWITHDEBINFO_POSTFIX "_rwdi"
	MINSIZEREL_POSTFIX "_msr"
)

## High level macro to install in an homogen way all our ibr targets
include(install_runtime)
ibr_install_target(${PROJECT_NAME}
    INSTALL_PDB                         ## mean install also MSVC IDE *.pdb file (DEST according to target type)
		#RSC_FILE_ADD "${PROJECT_NAME}_rsc"
		#DIRS 				${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    STANDALONE  ${INSTALL_STANDALONE}   ## mean call install_runtime with bundle dependencies resolution
    COMPONENT   ${PROJECT_NAME}_install ## will create custom target to install only this project
)
set_target_properties(${PROJECT_NAME}_install PROPERTIES FOLDER ${SIBR_INSTALL_FOLDER})
