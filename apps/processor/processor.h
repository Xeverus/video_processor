#pragma once

#include <array>
#include <memory>
#include <vector>

#include <vid_lib/math/random.h>

#include <vid_lib/opengl/opengl.h>

#include <vid_lib/sprite/atlas.h>
#include <vid_lib/sprite/sprite.h>

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

    vid_lib::math::Random random_source_;

    std::array<float, 3> film_margin_color_ = {0.15f, 0.08f, 0.0f};
    std::array<float, 4> film_margin_edges_ = {0.0f};

private:
    std::vector<vid_lib::sprite::Sprite> MakeRandomlyPlacedSprites(
        const vid_lib::sprite::Atlas& atlas,
        int sprite_count,
        float sprite_width,
        float sprite_height);

    float ComputeVerticalScale(int input_movie_width, int input_movie_height) const;
};