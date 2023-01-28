#pragma once

#include <array>
#include <random>

namespace vid_lib::math
{

class Random
{
public:
    Random();
    explicit Random(int max_value);

    int GetNextInt();
    float GetNextFloat();

    int MaxInt() const;

private:
    int max_value_;
    std::random_device random_device_;
    std::mt19937 number_generator_;
    std::uniform_int_distribution<> distribution_;
};

}