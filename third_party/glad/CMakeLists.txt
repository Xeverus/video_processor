cmake_minimum_required(VERSION 3.23)

include(ExternalProject)

ExternalProject_Add(
        "glad"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/third_party/glad/glad/"
        BINARY_DIR "${CMAKE_BINARY_DIR}/third_party/glad/bin/"
        CMAKE_ARGS
            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/install/glad
        TEST_COMMAND ""
)
