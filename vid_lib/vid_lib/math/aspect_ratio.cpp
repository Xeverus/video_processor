#include <vid_lib/math/aspect_ratio.h>

namespace vid_lib::math
{

float AspectRatio::CalculateVerticalScale(int from_width, int from_height, int to_width, int to_height)
{
    const auto w1 = static_cast<float>(from_width);
    const auto h1 = static_cast<float>(from_height);

    const auto w2 = static_cast<float>(to_width);
    const auto h2 = static_cast<float>(to_height);

    return (h1 / h2) / (w1 / w2);
}

}