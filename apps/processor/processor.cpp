#include <processor.h>

#include <fstream>

#include <vid_lib/math/aspect_ratio.h>
#include <vid_lib/math/film.h>
#include <vid_lib/math/random.h>

#include <vid_lib/opengl/buffer/buffer.h>
#include <vid_lib/opengl/debug/debug_messenger.h>
#include <vid_lib/opengl/shader/program.h>
#include <vid_lib/opengl/texture/framebuffer.h>
#include <vid_lib/opengl/texture/texture.h>

#include "vid_lib/sprite/atlas.h"
#include "vid_lib/sprite/geometry_generator.h"

#include <vid_lib/video/video_reader.h>
#include <vid_lib/video/video_writer.h>

namespace
{

Config LoadConfig(const int argc, char* argv[])
{
    return Config
        {
            "../../../Assets/Movies/original.MOV",
            "../../../Assets/Movies/output.mp4",
            480,
            640,
            30,
            32,
            4
        };
}

}

Processor::Processor(const int argc, char* argv[])
{
    config_ = LoadConfig(argc, argv);
    open_gl_ = std::make_unique<vid_lib::opengl::OpenGl>();
    glfw_window_ = open_gl_->MakeWindow({1024, 768, "Video Processor"});
    glfwSetWindowUserPointer(glfw_window_, this);

    vid_lib::opengl::debug::DebugMessenger::Enable();
}

void Processor::Run()
{
    vid_lib::video::VideoReader input_movie(config_.input_movie_filepath);
    vid_lib::video::VideoWriter output_movie(config_.output_movie_filepath, config_.output_movie_width,
                                             config_.output_movie_height, config_.output_movie_fps);

    const auto vertical_scale = vid_lib::math::AspectRatio::CalculateVerticalScale(
        input_movie.GetFrameWidth(), input_movie.GetFrameHeight(), config_.output_movie_width,
        config_.output_movie_height);

    // make buffers
    glfwSetWindowSize(glfw_window_, config_.output_movie_width, config_.output_movie_height);

    const auto framebuffer_1a = vid_lib::opengl::texture::Framebuffer::MakeNew(
        input_movie.GetFrameWidth(), input_movie.GetFrameHeight());
    const auto framebuffer_1b = vid_lib::opengl::texture::Framebuffer::MakeNew(
        input_movie.GetFrameWidth(), input_movie.GetFrameHeight());
    const auto framebuffer_2a = vid_lib::opengl::texture::Framebuffer::WrapDefault(
        config_.output_movie_width, config_.output_movie_height);
    //

    film_margin_edges_ = vid_lib::math::Film::CalculateMarginEdges(input_movie.GetFrameHeight(),
                                                                   config_.film_margin_size, config_.film_margin_step);

    auto program_1a = vid_lib::opengl::shader::Program::MakeFromFiles(
        "../../../assets/shaders/processor/full_screen_vertical_scale.vs",
        "../../../assets/shaders/processor/full_screen_film_margins.fs");
    auto program_1b = vid_lib::opengl::shader::Program::MakeFromFiles(
        "../../../assets/shaders/processor/sprites.vs",
        "../../../assets/shaders/processor/sprites.fs");
    auto program_1c = vid_lib::opengl::shader::Program::MakeFromFiles(
        "../../../assets/shaders/processor/full_screen.vs",
        "../../../assets/shaders/processor/full_screen_postprocess.fs");
    auto program_2a = vid_lib::opengl::shader::Program::MakeFromFiles(
        "../../../assets/shaders/processor/full_screen.vs",
        "../../../assets/shaders/processor/full_screen_channels_separation.fs");

    const auto input_image_texture = vid_lib::opengl::texture::Texture::MakeEmpty();

    glActiveTexture(GL_TEXTURE0 + 1);
    const auto font_texture = vid_lib::opengl::texture::Texture::MakeFromFile("../../../assets/sprites/font_48x80.png");
    const auto font_atlas = vid_lib::sprite::Atlas("../../../assets/sprites/font_48x80.txt");

    glActiveTexture(GL_TEXTURE0 + 2);
    const auto decals_texture = vid_lib::opengl::texture::Texture::MakeFromFile(
        "../../../assets/sprites/decals_64x128.png");
    const auto decals_atlas = vid_lib::sprite::Atlas("../../../assets/sprites/decals_64x128.txt");

    const auto text_height = 0.14f;
    const auto text_width = text_height * 0.6f;
    const auto text = vid_lib::sprite::GeometryGenerator::MakeVerticalText(
        "JAN.29 2023\nPM 6:41", -0.85f, -0.8f, text_width, text_height, font_atlas);

    const auto decal_width = 0.16f;
    const auto decal_height = decal_width * 2.5f;
    const auto decals = MakeRandomlyPlacedSprites(decals_atlas, 16, decal_width, decal_height);

    GLuint text_vao;
    GLuint decals_vao;
    glGenVertexArrays(1, &text_vao);
    glGenVertexArrays(1, &decals_vao);

    glBindVertexArray(text_vao);
    vid_lib::opengl::buffer::Buffer text_buffer(text.GetLetters());
    {
        text_buffer.Bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (const void*)0);
        glVertexAttribDivisor(0, 1);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (const void*)8);
        glVertexAttribDivisor(1, 1);
    }

    glBindVertexArray(decals_vao);
    vid_lib::opengl::buffer::Buffer decals_buffer(decals);
    {
        decals_buffer.Bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (const void*)0);
        glVertexAttribDivisor(0, 1);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (const void*)8);
        glVertexAttribDivisor(1, 1);
    }

    const auto step = 0.73432117f;
    auto time = 0.0f;

    auto input_image = input_movie.GetNextFrame();
    cv::Mat output_image(config_.output_movie_height, config_.output_movie_width, input_image.type());
    while (!glfwWindowShouldClose(glfw_window_) && !input_image.empty())
    {
        time += step;

        glActiveTexture(GL_TEXTURE0 + 0);
        input_image_texture->Update(input_image);

        // phase 1a - add margins, apply aspect
        {
            program_1a->Use();
            program_1a->SetUniform("u_verticalScale", vertical_scale);
            program_1a->SetUniform("u_image", 0);
            program_1a->SetUniform("u_filmMarginColor", film_margin_color_[0], film_margin_color_[1],
                                   film_margin_color_[2]);
            program_1a->SetUniform("u_filmMarginEdges", film_margin_edges_[0], film_margin_edges_[1],
                                   film_margin_edges_[2], film_margin_edges_[3]);

            framebuffer_1a.Bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glFlush();
        }

        // phase 1b - add text and white decals
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            program_1b->Use();
            program_1b->SetUniform("u_image", 0);
            program_1b->SetUniform("u_fontImage", 1);

            {
                glBindVertexArray(text_vao);
                program_1b->SetUniform("u_spriteScreenSize", text_width, text_height);
                program_1b->SetUniform("u_spriteTextureSize", font_atlas.GetSpriteTextureWidth(),
                                       font_atlas.GetSpriteTextureHeight());
                program_1b->SetUniform("u_spriteRotation", 3.14f / 2.0f);
                glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, text.GetLength());
                glFlush();
            }

            if (random_source_.GetNextFloat() < 0.03f)
            {
                glBindVertexArray(decals_vao);
                program_1b->SetUniform("u_fontImage", 2);
                program_1b->SetUniform("u_spriteScreenSize", decal_width, decal_height);
                program_1b->SetUniform("u_spriteTextureSize", decals_atlas.GetSpriteTextureWidth(),
                                       decals_atlas.GetSpriteTextureHeight());
                program_1b->SetUniform("u_spriteRotation", 0.0f);
                const auto instance_number = 1;
                const auto first_instance = random_source_.GetNextInt() % (decals.size() - instance_number);
                glDrawArraysInstancedBaseInstance(GL_TRIANGLE_STRIP, 0, 4, instance_number, first_instance);
                glFlush();
            }

            glDisable(GL_BLEND);
        }

        // phase 1c - apply postprocessing
        {
            program_1c->Use();
            program_1c->SetUniform("u_image", 0);
            program_1c->SetUniform("u_time", time);
            framebuffer_1a.BindTexture();
            framebuffer_1b.Bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glFlush();
        }

        // phase 2a - apply channel separation and rescaling
        {
            program_2a->Use();
            program_2a->SetUniform("u_image", 0);
            framebuffer_1b.BindTexture();
            framebuffer_2a.Bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glFlush();
        }

        glReadPixels(0, 0, config_.output_movie_width, config_.output_movie_height, GL_BGR, GL_UNSIGNED_BYTE,
                     output_image.data);
        output_movie.WriteFrame(output_image);

        glfwPollEvents();

        input_image = input_movie.GetNextFrame();
    }
}

std::vector<vid_lib::sprite::Sprite> Processor::MakeRandomlyPlacedSprites(
    const vid_lib::sprite::Atlas& atlas,
    const int sprite_count,
    const float sprite_screen_width,
    const float sprite_screen_height)
{
    std::vector<vid_lib::sprite::Sprite> decals;
    decals.reserve(sprite_count);

    const auto sprite_names = atlas.GetValidSpriteNames();
    for (auto i = 0; i < sprite_count; ++i)
    {
        const char decal_name = sprite_names[random_source_.GetNextInt() % sprite_names.size()];
        const auto sprite_description = atlas.GetSpriteDescription(decal_name);
        const auto pos_x = (random_source_.GetNextFloat() * 2.0f - 1.0f) * (1.0f - sprite_screen_width * 0.5f);
        const auto pos_y = (random_source_.GetNextFloat() * 2.0f - 1.0f) * (1.0f - sprite_screen_height * 0.5f);
        const auto decal = vid_lib::sprite::GeometryGenerator::MakeSprite(sprite_description, pos_x, pos_y,
                                                                          sprite_screen_width,
                                                                          sprite_screen_height);
        decals.push_back(decal);
    }

    return decals;
}

