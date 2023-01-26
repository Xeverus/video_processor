cmake_minimum_required(VERSION 3.23)

project(glad)

include(GNUInstallDirs)

add_library(glad include/glad/gl.h src/gl.c)

set_target_properties(glad PROPERTIES
        OUTPUT_NAME glad_lib
        PUBLIC_HEADER include/glad/gl.h)

target_include_directories(glad PUBLIC include)

install(TARGETS glad PUBLIC_HEADER DESTINATION include/glad)
