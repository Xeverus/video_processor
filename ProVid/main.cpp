#include <iostream>

#include <glfw/glfw3.h>
#include <opencv2/videoio.hpp>

int main() {
    const auto result = glfwInit();
    std::cout << "GLFW init: " << (result == GLFW_TRUE) << std::endl;

    return 0;
}
