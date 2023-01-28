#pragma once

#include <array>
#include <memory>

#include <vid_lib/opengl/opengl.h>

#include <config.h>

class Processor
{
public:
    Processor(int argc, char* argv[]);

    void Run();

private:
    Config config_;
    std::unique_ptr<vid_lib::opengl::OpenGl> open_gl_;
    GLFWwindow* glfw_window_;

    std::array<float, 3> film_margin_color_ = {0.15f, 0.08f, 0.0f};
    std::array<float, 4> film_margin_edges_ = {0.0f};
};