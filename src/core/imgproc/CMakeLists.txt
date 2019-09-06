project(sibr_imgproc)

file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")
source_group("Source Files" FILES ${SOURCES})

file(GLOB SOURCES "mrf/*.h" "mrf/*.cpp" "mrf/*.inc")
source_group("Source Files\\mrf" FILES ${SOURCES})

file(GLOB SOURCES "*.cpp" "*.h" "*.hpp" "mrf/*.h" "mrf/*.cpp" "mrf/*.inc"
	) 

## Specify target rules
add_library(${PROJECT_NAME} SHARED ${SOURCES})

include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
${Boost_LIBRARIES}
${OpenCV_LIBRARIES}
glfw3
sibr_system
sibr_graphics
sibr_assets
sibr_raycaster
)

if (NOT WIN32)
	target_link_libraries(${PROJECT_NAME}
 #GLEW
 rt m dl X11 pthread Xrandr Xinerama Xxf86vm Xcursor
	# X11 Xi Xrandr Xxf86vm Xinerama Xcursor dl rt m pthread
	)
endif()

#define postfix for various debug/release
set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX "_d"
	RELWITHDEBINFO_POSTFIX "_rwdi"
	MINSIZEREL_POSTFIX "_msr"
)

add_definitions( -DSIBR_IMGPROC_EXPORTS -DBOOST_ALL_DYN_LINK  )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER ${SIBR_FOLDER})


## Hight level macro to install in an homogen way all our ibr targets
include(install_runtime)
ibr_install_target(${PROJECT_NAME}
    INSTALL_PDB                         ## mean install also MSVC IDE *.pdb file (DEST according to target type)
		#RSC_FILE_ADD "${PROJECT_NAME}_rsc"
		#DIRS 				${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    #STANDALONE  ${INSTALL_STANDALONE}   ## mean call install_runtime with bundle dependencies resolution
    #COMPONENT   ${PROJECT_NAME}_install ## will create custom target to install only this project
)
#set_target_properties(${PROJECT_NAME}_install PROPERTIES FOLDER ${SIBR_INSTALL_FOLDER})
