project("glfw")

include(ExternalProject)

ExternalProject_Add(
        "glfw"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/ThirdParty/glfw/glfw/"
        BINARY_DIR "${CMAKE_BINARY_DIR}/ThirdParty/glfw/bin/"
        CMAKE_ARGS
            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        CMAKE_CACHE_ARGS
            -DGLFW_BUILD_TESTS:BOOL=OFF
        TEST_COMMAND ""
        INSTALL_COMMAND ""
)
set_property(TARGET "glfw" PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})