cmake_minimum_required(VERSION 3.12)

sofa_add_application(Modeler Modeler OFF)

sofa_add_application(getDeprecatedComponents getDeprecatedComponents OFF)

sofa_add_application(GenerateRigid GenerateRigid)

sofa_add_application(SofaPhysicsAPI SofaPhysicsAPI)
sofa_add_application(SofaGuiGlut SofaGuiGlut OFF)

sofa_add_application(runSofa runSofa ON)
sofa_add_application(sofaOPENCL sofaOPENCL OFF)

sofa_add_subdirectory_external(Regression Regression)
sofa_add_subdirectory_external(SofaGLFW SofaGLFW)
