#pragma once

#include <array>
#include <memory>
#include <vector>

#include <vid_lib/math/random.h>

#include <vid_lib/opengl/buffer/sprite_buffer_array.h>
#include <vid_lib/opengl/opengl.h>
#include <vid_lib/opengl/shader/program.h>
#include <vid_lib/opengl/texture/framebuffer.h>
#include <vid_lib/opengl/texture/texture.h>

#include <vid_lib/sprite/atlas.h>
#include <vid_lib/sprite/sprite_batch.h>

#include <config.h>

class Processor
{
public:
    Processor(int argc, char* argv[]);

    void Run();

private:
    Config config_;
    GLFWwindow* glfw_window_;

    vid_lib::math::Random random_source_;

    std::unique_ptr<vid_lib::opengl::OpenGl> open_gl_;
    std::unique_ptr<vid_lib::opengl::buffer::SpriteBufferArray> text_buffer_array_;
    std::unique_ptr<vid_lib::opengl::buffer::SpriteBufferArray> decals_buffer_array_;
    std::unique_ptr<vid_lib::opengl::shader::Program> program_1a_;
    std::unique_ptr<vid_lib::opengl::shader::Program> program_1b_;
    std::unique_ptr<vid_lib::opengl::shader::Program> program_1c_;
    std::unique_ptr<vid_lib::opengl::shader::Program> program_2a_;
    std::unique_ptr<vid_lib::opengl::texture::Framebuffer> first_framebuffer_;
    std::unique_ptr<vid_lib::opengl::texture::Framebuffer> second_framebuffer_;
    std::unique_ptr<vid_lib::opengl::texture::Framebuffer> default_framebuffer_;
    std::unique_ptr<vid_lib::opengl::texture::Texture> input_image_texture_;
    std::unique_ptr<vid_lib::opengl::texture::Texture> font_texture_;
    std::unique_ptr<vid_lib::opengl::texture::Texture> decals_texture_;

    std::unique_ptr<vid_lib::sprite::Atlas> font_atlas_;
    std::unique_ptr<vid_lib::sprite::Atlas> decals_atlas_;
    std::unique_ptr<vid_lib::sprite::SpriteBatch> text_;
    std::unique_ptr<vid_lib::sprite::SpriteBatch> decals_;

    float time_;

    std::array<float, 3> film_margin_color_ = {0.15f, 0.08f, 0.0f};
    std::array<float, 4> film_margin_edges_ = {0.0f};
    float vertical_scale_;
    float text_sprite_screen_width_;
    float text_sprite_screen_height_;
    float decal_sprite_screen_width_;
    float decal_sprite_screen_height_;

private:
    float ComputeVerticalScale(int input_movie_width,
                               int input_movie_height) const;

    // add margins, apply aspect
    void RenderFirstPass();

    // add text
    void RenderSecondPass();

    // add white decals
    void RenderThirdPass();

    // apply postprocessing
    void RenderFourthPass();

    // apply channel separation and rescaling
    void RenderFifthPass();
};