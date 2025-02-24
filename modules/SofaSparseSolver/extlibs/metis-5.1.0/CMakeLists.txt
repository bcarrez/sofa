cmake_minimum_required(VERSION 3.12)
project(metis)

unset(GKLIB_PATH CACHE)
get_filename_component(GKLIB_PATH "GKlib" ABSOLUTE)
include(GKlib/GKlibSystem.cmake)

file(GLOB metis_sources libmetis/*.c)

# The code must be relocatable if we want to link a shared library against it.
if("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xGNU" OR "x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xClang")
    add_compile_options(-fPIC)
endif()

add_library(${PROJECT_NAME} STATIC ${metis_sources} ${GKlib_sources})
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/GKlib>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libmetis>")
if(UNIX)
    target_link_libraries(${PROJECT_NAME} m)
endif()

sofa_create_package_with_targets(
    PACKAGE_NAME Metis
    PACKAGE_VERSION 5.1.0
    TARGETS metis AUTO_SET_TARGET_PROPERTIES
    INCLUDE_SOURCE_DIR "include"
    INCLUDE_INSTALL_DIR "extlibs/Metis"
    )
