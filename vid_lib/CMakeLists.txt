cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME "vid_lib")
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_HEADERS "vid_lib/*.h")
file(GLOB_RECURSE PROJECT_SOURCE "vid_lib/*.cpp")

add_library(${PROJECT_NAME}
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCE}
        vid_lib/sprite/atlas.h)

add_dependencies(${PROJECT_NAME}
        "glad"
        "glfw"
        "opencv"
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC .
        PUBLIC ${CMAKE_BINARY_DIR}/install/glad/include
        PUBLIC ${CMAKE_BINARY_DIR}/install/glfw/include
        PUBLIC ${CMAKE_BINARY_DIR}/install/opencv/include
        )

target_link_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_BINARY_DIR}/install/glad/lib
        PUBLIC ${CMAKE_BINARY_DIR}/install/glfw/lib
        PUBLIC ${CMAKE_BINARY_DIR}/install/opencv/x64/mingw/lib
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE "glad_lib"
        PRIVATE "glfw3"
        PRIVATE "opencv_core470"
        PRIVATE "opencv_imgcodecs470"
        PRIVATE "opencv_videoio470"
        )