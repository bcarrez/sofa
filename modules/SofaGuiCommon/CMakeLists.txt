cmake_minimum_required(VERSION 3.12)
project(SofaGuiCommon)

sofa_find_package(SofaBase REQUIRED)
sofa_find_package(SofaUserInteraction REQUIRED)
sofa_find_package(SofaGraphComponent REQUIRED)
sofa_find_package(SofaSimulationCommon REQUIRED)
sofa_find_package(Sofa.GL QUIET) # ColourPickingVisitor
sofa_find_package(Boost REQUIRED COMPONENTS program_options)

if(Sofa.GL_FOUND)
    message("-- ${PROJECT_NAME}: Sofa.GL dependent features enabled.")
else()
    message("-- ${PROJECT_NAME}: Sofa.GL dependent features disabled.")
endif()

set(SRC_ROOT src/sofa/gui)

set(HEADER_FILES
    ${SRC_ROOT}/config.h.in
    ${SRC_ROOT}/BaseGUI.h
    ${SRC_ROOT}/BaseViewer.h
    ${SRC_ROOT}/BatchGUI.h
    ${SRC_ROOT}/ColourPickingVisitor.h
    ${SRC_ROOT}/FilesRecentlyOpenedManager.h
    ${SRC_ROOT}/GuiDataRepository.h
    ${SRC_ROOT}/GUIManager.h
    ${SRC_ROOT}/MouseOperations.h
    ${SRC_ROOT}/OperationFactory.h
    ${SRC_ROOT}/PickHandler.h
    ${SRC_ROOT}/SofaGUI.h
    ${SRC_ROOT}/ViewerFactory.h
    ${SRC_ROOT}/ArgumentParser.h
)

set(SOURCE_FILES
    ${SRC_ROOT}/BaseGUI.cpp
    ${SRC_ROOT}/BaseViewer.cpp
    ${SRC_ROOT}/BatchGUI.cpp
    ${SRC_ROOT}/ColourPickingVisitor.cpp
    ${SRC_ROOT}/FilesRecentlyOpenedManager.cpp
    ${SRC_ROOT}/GuiDataRepository.cpp
    ${SRC_ROOT}/GUIManager.cpp
    ${SRC_ROOT}/MouseOperations.cpp
    ${SRC_ROOT}/PickHandler.cpp
    ${SRC_ROOT}/ViewerFactory.cpp
    ${SRC_ROOT}/ArgumentParser.cpp
)

# Compatibility files
set (COMPATSRC_ROOT "compat/")
set (COMPAT_HEADER_FILES
    ${COMPATSRC_ROOT}/sofa/helper/ArgumentParser.h
    )

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} ${COMPAT_HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SofaBase SofaGraphComponent SofaUserInteraction SofaSimulationCommon)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::program_options)

if (SOFA_BUILD_RELEASE_PACKAGE OR CMAKE_SYSTEM_NAME STREQUAL Windows)
    sofa_install_libraries(PATHS ${Boost_LIBRARIES}) # Boost_LIBRARIES covers Boost internal dependencies
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${COMPATSRC_ROOT}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<INSTALL_INTERFACE:include/SofaGuiCommon/SofaGuiCommon/compat>")
source_group("compat" FILES ${COMPAT_HEADER_FILES} )

if(Sofa.GL_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC Sofa.GL)
endif()

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Silence attribute warnings (for example, ignored already defined external template)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-attributes)
endif()

sofa_create_package_with_targets(
    PACKAGE_NAME ${PROJECT_NAME}
    PACKAGE_VERSION ${Sofa_VERSION}
    TARGETS ${PROJECT_NAME} AUTO_SET_TARGET_PROPERTIES
    INCLUDE_SOURCE_DIR "src"
    INCLUDE_INSTALL_DIR ${PROJECT_NAME}
    )
