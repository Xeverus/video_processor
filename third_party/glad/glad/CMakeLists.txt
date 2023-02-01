cmake_minimum_required(VERSION 3.23)

project(glad)

include(GNUInstallDirs)

set(GLAD_HEADERS
        include/glad/gl.h
        include/KHR/khrplatform.h
        )

add_library(glad
        ${GLAD_HEADERS}
        src/gl.c)

set_target_properties(glad PROPERTIES OUTPUT_NAME glad_lib)

target_include_directories(glad PUBLIC include)

install(TARGETS glad)
install(DIRECTORY include DESTINATION .)
