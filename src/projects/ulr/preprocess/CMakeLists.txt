
project(SIBR_ulr_preprocess)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/settings.ini.in ${CMAKE_CURRENT_SOURCE_DIR}/settings.ini)

add_subdirectory(colmapToSIBR)
add_subdirectory(distordCrop)
add_subdirectory(cropFromCenter)
add_subdirectory(clippingPlanes)
add_subdirectory(ULR_Preprocess)
