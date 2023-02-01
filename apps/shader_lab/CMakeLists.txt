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

add_dependencies(${PROJECT_NAME}
        vid_lib)

target_link_libraries(${PROJECT_NAME}
        PRIVATE vid_lib)

# install shared libs
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${OPENCV_SHARED_LIBS}/libopencv_core470.dll
        ${OPENCV_SHARED_LIBS}/libopencv_imgcodecs470.dll
        ${OPENCV_SHARED_LIBS}/libopencv_imgproc470.dll
        ${OPENCV_SHARED_LIBS}/libopencv_videoio470.dll
        ${OPENCV_SHARED_LIBS}/opencv_videoio_ffmpeg470_64.dll
        ${CMAKE_CURRENT_BINARY_DIR}/)