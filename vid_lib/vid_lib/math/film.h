#pragma once

#include <array>

namespace vid_lib::math
{

class Film
{
public:
    static std::array<float, 4> CalculateMarginEdges(int image_dimension, int margin_size, int margin_step_size);
};

}