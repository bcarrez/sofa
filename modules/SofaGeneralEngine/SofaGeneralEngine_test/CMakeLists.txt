cmake_minimum_required(VERSION 3.12)

project(SofaGeneralEngine_test)

set(HEADER_FILES)

set(SOURCE_FILES ../../empty.cpp)

list(APPEND SOURCE_FILES
    AverageCoord_test.cpp
    ClusteringEngine_test.cpp
    ComplementaryROI_test.cpp
    DifferenceEngine_test.cpp
    DilateEngine_test.cpp
    ExtrudeEdgesAndGenerateQuads_test.cpp
    MeshROI_test.cpp
    SphereROI_test.cpp
    PlaneROI_test.cpp
    TransformEngine_test.cpp
    SmoothMeshEngine_test.cpp
    IndicesFromValues_test.cpp
    MergePoints_test.cpp
    IndexValueMapper_test.cpp
    JoinPoints_test.cpp
    RandomPointDistributionInSurface_test.cpp
    Engine_test.cpp
    TestEngine.cpp
    )

sofa_find_package(SofaGeneralEngine REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} Sofa.Testing SofaTest SofaGeneralEngine)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
