project(sibr_view)

file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")
source_group("Source Files" FILES ${SOURCES})

file(GLOB SHADERS "shaders/*.frag" "shaders/*.vert" "shaders/*.geom" "shaders/*.fp" "shaders/*.vp" "shaders/*.gp")
source_group("Source Files\\shaders" FILES ${SHADERS})

file(GLOB INTERFACE_SOURCES "interface/*.cpp" "interface/*.h" )
source_group("Source Files\\interface" FILES ${INTERFACE_SOURCES})

file(GLOB SOURCES
	"*.cpp" "*.h" "*.hpp"
	"shaders/*.frag" "shaders/*.vert" "shaders/*.geom"  "shaders/*.fp" "shaders/*.vp" "shaders/*.gp"
	"interface/*.cpp" "interface/*.h"
	)

## Specify target rules
add_library(${PROJECT_NAME} SHARED ${SOURCES})

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
 ${Boost_LIBRARIES}
 ${OpenCV_LIBRARIES}
sibr_graphics
sibr_assets
sibr_raycaster
sibr_scene
sibr_video
)

#define postfix for various debug/release
set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX "_d"
	RELWITHDEBINFO_POSTFIX "_rwdi"
	MINSIZEREL_POSTFIX "_msr"
)

add_definitions( -DSIBR_VIEW_EXPORTS -DBOOST_ALL_DYN_LINK  )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SIBR_FOLDER})

set(RESOURCES ${SHADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES RESOURCE "${RESOURCES}")

## Hight level macro to install in an homogen way all our ibr targets
include(install_runtime)
ibr_install_target(${PROJECT_NAME}
    INSTALL_PDB                         ## mean install also MSVC IDE *.pdb file (DEST according to target type)
		RSC_FILE_ADD "${PROJECT_NAME}_rsc"
		#DIRS 				${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    #STANDALONE  ${INSTALL_STANDALONE}   ## mean call install_runtime with bundle dependencies resolution
    #COMPONENT   ${PROJECT_NAME}_install ## will create custom target to install only this project
)
#set_target_properties(${PROJECT_NAME}_install PROPERTIES FOLDER ${SIBR_INSTALL_FOLDER})

add_custom_target(${PROJECT_NAME}_CopyShaders COMMAND python ${CMAKE_SOURCE_DIR}/cmake/CopyShaders.py "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*" "${CMAKE_INSTALL_PREFIX}/bin/shaders_rsc")
add_dependencies( ${PROJECT_NAME} ${PROJECT_NAME}_CopyShaders )
set_target_properties(${PROJECT_NAME}_CopyShaders PROPERTIES FOLDER ${SIBR_INSTALL_FOLDER})
