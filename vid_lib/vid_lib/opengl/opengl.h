#pragma once

#include <glad/gl.h>
#include <glfw/glfw3.h>

#include <string>

namespace vid_lib::opengl
{

class OpenGl
{
public:
    struct WindowParameters;

public:
    OpenGl();

    ~OpenGl();

    GLFWwindow* MakeWindow(const WindowParameters& window_parameters) const;
};

struct OpenGl::WindowParameters
{
    int width;
    int height;
    std::string name;
};

}