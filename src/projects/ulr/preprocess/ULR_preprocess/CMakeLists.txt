project(ULR_preprocess)

# libraries used:
# * sibr graphics (vector2i class)
# * sibr system (programArg class)
# * boost filesystem (path class)
# * openmp

# Define build output for project
add_executable(${PROJECT_NAME} ulr_preprocess.cpp)

target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
    sibr_graphics
)
add_dependencies(${PROJECT_NAME} cropFromCenter distordCrop clippingPlanes)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "__projects__/__ulr__/__preprocess__")

#define postfix for various debug/release
set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX "_d"
	RELWITHDEBINFO_POSTFIX "_rwdi"
	MINSIZEREL_POSTFIX "_msr"
)

## Hight level macro to install in an homogen way all our ibr targets
include(install_runtime)
ibr_install_target(${PROJECT_NAME}
    INSTALL_PDB                         ## mean install also MSVC IDE *.pdb file (DEST according to target type)
		#RSC_FILE_ADD "${PROJECT_NAME}_rsc"
		#DIRS 				${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    STANDALONE  ${INSTALL_STANDALONE}   ## mean call install_runtime with bundle dependencies resolution
    COMPONENT   ${PROJECT_NAME}_install ## will create custom target to install only this project
)
set_target_properties(${PROJECT_NAME}_install PROPERTIES FOLDER ${SIBR_INSTALL_FOLDER})
