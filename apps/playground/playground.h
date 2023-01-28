#pragma once

class Playground
{
public:
    void Run();

private:
    float saturation_ = -0.15f;
    float exposure_ = 0.05f;
    float contrast_ = 0.15f;
    float brightness_ = -0.1f;
    float tint_[3] = {1.0f, 1.0f, 1.0f};
};