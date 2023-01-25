cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME "ProVid")
project(${PROJECT_NAME})

add_executable(ProVid main.cpp)

add_dependencies(${PROJECT_NAME}
        "glfw"
        "opencv")

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_BINARY_DIR}/install/glfw/include
        PRIVATE ${CMAKE_BINARY_DIR}/install/opencv/include
        )

target_link_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_BINARY_DIR}/install/glfw/lib
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE "glfw3"
        )