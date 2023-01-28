cmake_minimum_required(VERSION 3.23)

include(ExternalProject)

ExternalProject_Add(
        "glfw"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/glfw/glfw/"
        BINARY_DIR "${CMAKE_BINARY_DIR}/third_party/glfw/bin/"
        CMAKE_ARGS
            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/install/glfw
            -DGLFW_BUILD_TESTS=OFF
            -DGLFW_BUILD_DOCS=OFF
        TEST_COMMAND ""
)
set_property(TARGET "glfw" PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})