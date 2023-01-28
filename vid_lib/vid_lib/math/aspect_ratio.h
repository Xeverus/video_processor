#pragma once

namespace vid_lib::math
{

class AspectRatio
{
public:
    static float CalculateVerticalScale(int width_one, int height_one, int width_two, int height_two);
};

}