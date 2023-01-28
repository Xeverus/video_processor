#include <vid_lib/opengl/opengl.h>

#include <iostream>
#include <stdexcept>

namespace vid_lib::opengl
{
namespace
{

void SetWindowHints()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

GLFWwindow* CreateWindowAndContext(const OpenGl::WindowParameters& window_parameters)
{
    const auto window = glfwCreateWindow(window_parameters.width, window_parameters.height,
                                         window_parameters.name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    return window;
}

void LoadOpenGl()
{
    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        throw std::runtime_error("Failed to initialize OpenGL context");
    }

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
}

}

OpenGl::OpenGl()
{
    glfwInit();
}

OpenGl::~OpenGl()
{
    glfwTerminate();
}

GLFWwindow* OpenGl::MakeWindow(const WindowParameters& window_parameters) const
{
    SetWindowHints();
    const auto window = CreateWindowAndContext(window_parameters);
    LoadOpenGl();

    return window;
}

}