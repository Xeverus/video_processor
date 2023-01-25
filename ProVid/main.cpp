#include <iostream>

#include <glfw/glfw3.h>

int main() {
    const auto result = glfwInit();
    std::cout << "GLFW init: " << (result == GLFW_TRUE) << std::endl;
    return 0;
}
