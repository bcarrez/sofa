cmake_minimum_required(VERSION 3.12)
project(SofaGeneralSimpleFem LANGUAGES CXX)

set(SOFAGENERALSIMPLEFEM_SRC "src/${PROJECT_NAME}")

set(HEADER_FILES
    ${SOFAGENERALSIMPLEFEM_SRC}/config.h.in
    ${SOFAGENERALSIMPLEFEM_SRC}/initSofaGeneralSimpleFem.h
    )

set(SOURCE_FILES
    ${SOFAGENERALSIMPLEFEM_SRC}/initSofaGeneralSimpleFem.cpp
    )

list(APPEND HEADER_FILES
    ${SOFAGENERALSIMPLEFEM_SRC}/BeamFEMForceField.h
    ${SOFAGENERALSIMPLEFEM_SRC}/BeamFEMForceField.inl
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedralFEMForceField.h
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedralFEMForceField.inl
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedralFEMForceFieldAndMass.h
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedralFEMForceFieldAndMass.inl
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedronFEMForceFieldAndMass.h
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedronFEMForceFieldAndMass.inl
    ${SOFAGENERALSIMPLEFEM_SRC}/TetrahedralCorotationalFEMForceField.h
    ${SOFAGENERALSIMPLEFEM_SRC}/TetrahedralCorotationalFEMForceField.inl
    ${SOFAGENERALSIMPLEFEM_SRC}/TriangularFEMForceFieldOptim.h
    ${SOFAGENERALSIMPLEFEM_SRC}/TriangularFEMForceFieldOptim.inl
    )
list(APPEND SOURCE_FILES
    ${SOFAGENERALSIMPLEFEM_SRC}/BeamFEMForceField.cpp
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedralFEMForceField.cpp
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedralFEMForceFieldAndMass.cpp
    ${SOFAGENERALSIMPLEFEM_SRC}/HexahedronFEMForceFieldAndMass.cpp
    ${SOFAGENERALSIMPLEFEM_SRC}/TetrahedralCorotationalFEMForceField.cpp
    ${SOFAGENERALSIMPLEFEM_SRC}/TriangularFEMForceFieldOptim.cpp

    )

sofa_find_package(SofaBase REQUIRED) # SofaBaseTopology SofaBaseMechanics
sofa_find_package(SofaSimpleFem REQUIRED)
sofa_find_package(SofaBaseLinearSolver REQUIRED)


add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SofaBaseTopology SofaSimpleFem SofaBaseLinearSolver SofaBaseMechanics)

sofa_create_package_with_targets(
    PACKAGE_NAME ${PROJECT_NAME}
    PACKAGE_VERSION ${Sofa_VERSION}
    TARGETS ${PROJECT_NAME} AUTO_SET_TARGET_PROPERTIES
    INCLUDE_SOURCE_DIR "src"
    INCLUDE_INSTALL_DIR "${PROJECT_NAME}"
    RELOCATABLE "plugins"
)
