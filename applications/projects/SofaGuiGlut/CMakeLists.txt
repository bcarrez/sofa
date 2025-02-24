cmake_minimum_required(VERSION 3.12)
project(SofaGuiGlut)

find_package(SofaSimulation REQUIRED)
find_package(GLUT REQUIRED)
find_package(Boost COMPONENTS thread chrono )
find_package(Sofa.GL REQUIRED)

set(HEADER_FILES
    SimpleGUI.h)

set(SOURCE_FILES
    SimpleGUI.cpp)


list(APPEND SOURCE_FILES Main.cpp)

if(NOT APPLE)
	set(GLUT_ACTUAL_LIBRARIES  ${GLUT_glut_LIBRARY} )
else()
    # Workaround: on OS X, "${GLUT_glut_LIBRARY}" is not a target (it is
    # actually "-framework GLUT"), so we use find_library to find
    # something better (like /System/Library/Frameworks/GLUT.framework.)
    # (Fun fact: FindGLUT.cmake litterally reads "These values for Apple
    # could probably do with improvement.)"
	find_library(ACTUAL_GLUT_LIBRARIES "GLUT")
	set(GLUT_ACTUAL_LIBRARIES  ${ACTUAL_GLUT_LIBRARIES} )
endif()

set(_configPluginFileName plugin_list.conf)
set(_defaultConfigPluginFileName "${_configPluginFileName}.default")
set(_defaultConfigPluginFilePath "${CMAKE_BINARY_DIR}/${_pluginLocation}/${_defaultConfigPluginFileName}")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES}  )

target_link_libraries(${PROJECT_NAME} SofaGuiCommon SofaSimulationGraph ${GLUT_ACTUAL_LIBRARIES} )
if(Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
if(Sofa.GL_FOUND)
    target_link_libraries(${PROJECT_NAME} Sofa.GL)
endif()

sofa_install_targets(SofaGui SofaGuiGlut "")
