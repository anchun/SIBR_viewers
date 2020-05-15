project(fullColmapPreprocess)

file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")
source_group("Source Files" FILES ${SOURCES})


# Define build output for project
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    sibr_graphics
    sibr_assets
    sibr_raycaster
    sibr_system
    sibr_view
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "__projects__/__ulr__/__preprocess__")

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
