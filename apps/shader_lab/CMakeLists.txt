cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME "shader_lab")
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_HEADERS "*.h")
file(GLOB_RECURSE PROJECT_SOURCE "*.cpp")

add_executable(${PROJECT_NAME}
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCE}
        )

target_include_directories(${PROJECT_NAME}
        PRIVATE .)

target_link_libraries(${PROJECT_NAME}
        PRIVATE vid_lib)