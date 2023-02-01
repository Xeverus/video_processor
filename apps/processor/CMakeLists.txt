cmake_minimum_required(VERSION 3.23)

set(PROJECT_NAME "processor")
project(${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_HEADERS "*.h")
file(GLOB_RECURSE PROJECT_SOURCE "*.cpp")

add_executable(${PROJECT_NAME}
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCE}
        )

target_include_directories(${PROJECT_NAME}
        PRIVATE .)

add_dependencies(${PROJECT_NAME}
        vid_lib)

target_link_libraries(${PROJECT_NAME}
        PRIVATE vid_lib)

# install config
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/config.txt
        ${CMAKE_CURRENT_BINARY_DIR}/config.txt)

# install shared libs
file(GLOB_RECURSE OPENCV_SHARED_OBJECTS "${CMAKE_BINARY_DIR}/install/opencv/x64/mingw/bin/*.dll")
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${OPENCV_SHARED_OBJECTS}
        ${CMAKE_CURRENT_BINARY_DIR}/)
