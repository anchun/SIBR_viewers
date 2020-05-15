# project name
project(distordCrop)

file(GLOB SOURCES "*.cpp" "*.h" "*.hpp")
include(FindOpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_definitions(-Dcimg_use_zlib -Dcimg_use_jpeg -Dcimg_use_png -Dcimg_use_tiff)

## libmini-sl uses Cimg that uses ImageMagick by default
## Switch to the original libs
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Z7")

# Define build output for project
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
   ${Boost_LIBRARIES}
   sibr_system
   sibr_graphics
   sibr_imgproc
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "__projects__/__ulr__/__preprocess__")
if (WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
endif()


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
