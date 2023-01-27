cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME "ProVid")
project(${PROJECT_NAME})

add_executable(ProVid main.cpp File/File.cpp File/File.h)

add_dependencies(${PROJECT_NAME}
        "glad"
        "glfw"
        "opencv")

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_BINARY_DIR}/install/glad/include
        PRIVATE ${CMAKE_BINARY_DIR}/install/glfw/include
        PRIVATE ${CMAKE_BINARY_DIR}/install/opencv/include
        )

target_link_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_BINARY_DIR}/install/glad/lib
        PRIVATE ${CMAKE_BINARY_DIR}/install/glfw/lib
        PRIVATE ${CMAKE_BINARY_DIR}/install/opencv/x64/mingw/lib
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE "glad_lib"
        PRIVATE "glfw3"
        PRIVATE "opencv_core470d"
        PRIVATE "opencv_videoio470d"
        )