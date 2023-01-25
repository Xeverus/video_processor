set(PROJECT_NAME "ProVid")
project(${PROJECT_NAME})

add_executable(ProVid main.cpp)

add_dependencies(${PROJECT_NAME} "glfw")

target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_SOURCE_DIR}/ThirdParty/glfw/glfw/include)

target_link_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_BINARY_DIR}/ThirdParty/glfw/bin/src/)

target_link_libraries(${PROJECT_NAME}
        PRIVATE "glfw3")