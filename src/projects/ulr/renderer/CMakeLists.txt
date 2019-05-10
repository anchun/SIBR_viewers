project(sibr_ulr)

file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")
source_group("Source Files" FILES ${SOURCES})

file(GLOB SHADERS "shaders/*.frag" "shaders/*.vert" "shaders/*.geom")
source_group("Source Files\\shaders" FILES ${SHADERS})

file(GLOB SOURCES "*.cpp" "*.h" "*.hpp" "shaders/*.frag" "shaders/*.vert" "shaders/*.geom")


## Specify target rules
add_library(${PROJECT_NAME} SHARED ${SOURCES})

include_directories(${Boost_INCLUDE_DIRS} .)
target_link_libraries(${PROJECT_NAME}
${Boost_LIBRARIES}
${ASSIMP_LIBRARIES}
${GLEW_LIBRARIES}
${OPENGL_LIBRARIES}
${OpenCV_LIBRARIES}
#${CGAL_LIBRARIES}			## from IBR_spixelWarp needed dependencies
#${SuiteSparse_LIBRARIES}	## from IBR_spixelWarp needed dependencies
glfw3
sibr_system
sibr_view
sibr_assets
sibr_renderer
)

#define postfix for various debug/release
set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX "_d"
	RELWITHDEBINFO_POSTFIX "_rwdi"
	MINSIZEREL_POSTFIX "_msr"
)

add_definitions( -DSIBR_EXP_ULR_EXPORTS -DBOOST_ALL_DYN_LINK  )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "__projects__/__ulr__/__renderer__")



set(RESOURCES ${SHADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES RESOURCE "${RESOURCES}")
set(DOXY_DOC_ULR_IMG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/doc" CACHE INTERNAL "") ## globale variable used in doxyfile.in)

## Hight level macro to install in an homogen way all our ibr targets
include(install_runtime)
ibr_install_target(${PROJECT_NAME}
    INSTALL_PDB                         ## mean install also MSVC IDE *.pdb file (DEST according to target type)
		RSC_FILE_ADD "${PROJECT_NAME}_rsc"
		#DIRS 				${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    #STANDALONE  ${INSTALL_STANDALONE}   ## mean call install_runtime with bundle dependencies resolution
    COMPONENT   ${PROJECT_NAME}_install ## will create custom target to install only this project
)
set_target_properties(${PROJECT_NAME}_install PROPERTIES FOLDER ${SIBR_INSTALL_FOLDER})

add_custom_target(${PROJECT_NAME}_CopyShaders ALL COMMAND python ${CMAKE_SOURCE_DIR}/cmake/CopyShaders.py "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*" "${CMAKE_INSTALL_PREFIX}/bin/shaders_rsc" VERBATIM)
add_dependencies( ${PROJECT_NAME} ${PROJECT_NAME}_CopyShaders )
set_target_properties(${PROJECT_NAME}_CopyShaders PROPERTIES FOLDER ${SIBR_INSTALL_FOLDER})
