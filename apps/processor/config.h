#pragma once

#include <array>
#include <string>

struct Config
{
    std::string input_movie_filepath;
    std::string output_movie_filepath;
    int output_movie_width;
    int output_movie_height;
    int output_movie_fps;
    int film_margin_size;
    int film_margin_step;
    std::array<float, 3> film_margin_color;
    std::string shaders_dir;
    bool enable_debug;
};