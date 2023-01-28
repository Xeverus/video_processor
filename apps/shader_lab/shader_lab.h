#pragma once

#include <vid_lib/opengl/opengl.h>

class ShaderLab
{
public:
    void Run();

private:
    float saturation_ = -0.15f;
    float exposure_ = 0.05f;
    float contrast_ = 0.15f;
    float brightness_ = -0.1f;
    float tint_[3] = {1.0f, 1.0f, 1.0f};
    bool up = true;

private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};