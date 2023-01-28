#pragma once

#include <array>

class Playground
{
public:
    void Run();

private:
    float saturation_ = -0.15f;
    float exposure_ = 0.05f;
    float contrast_ = 0.15f;
    float brightness_ = -0.1f;
    std::array<float, 3> tint_ = {1.0f, 1.0f, 1.0f};
    std::array<float, 3> film_margin_color_ = {0.15f, 0.08f, 0.0f};
    std::array<float, 4> film_margin_edges_ = {0.0f};
};