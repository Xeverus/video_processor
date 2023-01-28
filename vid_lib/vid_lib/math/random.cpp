#include <vid_lib/math/random.h>

namespace vid_lib::math
{

Random::Random()
    : Random(1024 * 1024)
{
}

Random::Random(int max_value)
    : max_value_(max_value)
    , number_generator_(random_device_())
    , distribution_(0, max_value)
{

}

int Random::GetNextInt()
{
    return distribution_(number_generator_);
}

float Random::GetNextFloat()
{
    return static_cast<float>(GetNextInt()) / static_cast<float>(MaxInt());
}

int Random::MaxInt() const
{
    return max_value_;
}

}