#include <processor.h>

#include <future>
#include <iostream>
#include <fstream>

#include <vid_lib/math/aspect_ratio.h>
#include <vid_lib/math/film.h>

#include <vid_lib/opengl/debug/debug_messenger.h>

#include "vid_lib/sprite/geometry_generator.h"

#include <vid_lib/video/async_video_reader.h>
#include <vid_lib/video/async_video_writer.h>

namespace
{

Config LoadConfig(const int argc, char* argv[])
{
    std::ifstream file("config.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("failed to open config.txt file; please put it in exec folder");
    }

    Config config;
    std::string parameter_name;

    file >> parameter_name >> config.input_movie_filepath;
    file >> parameter_name >> config.output_movie_filepath;
    file >> parameter_name >> config.shaders_dir;
    file >> parameter_name >> config.sprites_dir;
    file >> parameter_name >> config.output_movie_width;
    file >> parameter_name >> config.output_movie_height;
    file >> parameter_name >> config.output_movie_fps;
    file >> parameter_name >> config.film_margin_size;
    file >> parameter_name >> config.film_margin_step;
    file >> parameter_name >> config.film_margin_color[0] >> config.film_margin_color[1] >> config.film_margin_color[2];
    file >> parameter_name >> config.artifacts_chance;
    file >> parameter_name >> config.enable_debug;

    return config;
}

}

Processor::Processor(const int argc, char* argv[])
    : time_step_(0.73432117f)
    , time_(0.0f)
    , vertical_scale_(0.0f)
{
    config_ = LoadConfig(argc, argv);
    open_gl_ = std::make_unique<vid_lib::opengl::OpenGl>();
    glfw_window_ = open_gl_->MakeWindow({config_.output_movie_width, config_.output_movie_height, "Video Processor"});
    glfwSetWindowUserPointer(glfw_window_, this);

    if (config_.enable_debug)
    {
        vid_lib::opengl::debug::DebugMessenger::Enable();
    }
}

void Processor::Run()
{
    auto input_movie = std::make_unique<vid_lib::video::VideoReader>(config_.input_movie_filepath);
    auto output_movie = std::make_unique<vid_lib::video::VideoWriter>(config_.output_movie_filepath,
                                                                      config_.output_movie_width,
                                                                      config_.output_movie_height,
                                                                      config_.output_movie_fps);

    ComputeParameters(input_movie->GetFrameWidth(), input_movie->GetFrameHeight());
    MakeFramebuffers(input_movie->GetFrameWidth(), input_movie->GetFrameHeight());
    MakeTextures();
    BuildPrograms();
    MakeTextBufferArray();
    MakeDecalsBufferArray();

    cv::Mat input_image;
    input_movie->ReadFrame(input_image);
    cv::Mat output_image(config_.output_movie_height, config_.output_movie_width, input_image.type());

    vid_lib::video::AsyncVideoReader async_video_read(std::move(input_movie));
    vid_lib::video::AsyncVideoWriter async_video_writer(std::move(output_movie));
    const auto start = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(glfw_window_) && !input_image.empty())
    {
        async_video_read.Wait();

        glActiveTexture(GL_TEXTURE0 + 0);
        input_image_texture_->Update(input_image);

        async_video_read.ReadFrame(input_image);

        UpdateTime();
        RenderFirstPass();
        RenderSecondPass();
        RenderThirdPass();
        RenderFourthPass();
        RenderFifthPass();

        glReadPixels(0, 0, config_.output_movie_width, config_.output_movie_height, GL_BGR, GL_UNSIGNED_BYTE,
                     output_image.data);

        async_video_writer.WriteFrame(output_image);

        glfwPollEvents();
    }
    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << (end - start).count() << std::endl;
}

void Processor::ComputeParameters(const int input_movie_width, const int input_movie_height)
{
    vertical_scale_ = vid_lib::math::AspectRatio::CalculateVerticalScale(
        input_movie_width, input_movie_height,
        config_.output_movie_width, config_.output_movie_height);

    film_margin_edges_ = vid_lib::math::Film::CalculateMarginEdges(
        input_movie_height,
        config_.film_margin_size,
        config_.film_margin_step);
}

void Processor::MakeTextures()
{
    glActiveTexture(GL_TEXTURE0 + 0);
    input_image_texture_ = vid_lib::opengl::texture::Texture::MakeEmpty();

    glActiveTexture(GL_TEXTURE0 + 1);
    font_texture_ = vid_lib::opengl::texture::Texture::MakeFromFile(config_.sprites_dir + "/font_48x80.png");
    font_atlas_ = std::make_unique<vid_lib::sprite::Atlas>(config_.sprites_dir + "/font_48x80.txt");

    glActiveTexture(GL_TEXTURE0 + 2);
    decals_texture_ = vid_lib::opengl::texture::Texture::MakeFromFile(config_.sprites_dir + "/decals_64x128.png");
    decals_atlas_ = std::make_unique<vid_lib::sprite::Atlas>(config_.sprites_dir + "/decals_64x128.txt");
}

void Processor::MakeFramebuffers(const int width, const int height)
{
    first_framebuffer_ = vid_lib::opengl::texture::Framebuffer::MakeNew(width, height);
    second_framebuffer_ = vid_lib::opengl::texture::Framebuffer::MakeNew(width, height);
    third_framebuffer_ = vid_lib::opengl::texture::Framebuffer::WrapDefault(
        config_.output_movie_width, config_.output_movie_height);
}

void Processor::BuildPrograms()
{
    program_1a_ = vid_lib::opengl::shader::Program::MakeFromFiles(
        config_.shaders_dir + "/full_screen_vertical_scale.vs",
        config_.shaders_dir + "/full_screen_film_margins.fs");
    program_1b_ = vid_lib::opengl::shader::Program::MakeFromFiles(
        config_.shaders_dir + "/sprites.vs",
        config_.shaders_dir + "/sprites.fs");
    program_1c_ = vid_lib::opengl::shader::Program::MakeFromFiles(
        config_.shaders_dir + "/full_screen.vs",
        config_.shaders_dir + "/full_screen_postprocess.fs");
    program_2a_ = vid_lib::opengl::shader::Program::MakeFromFiles(
        config_.shaders_dir + "/full_screen.vs",
        config_.shaders_dir + "/full_screen_channels_separation.fs");
}

void Processor::MakeTextBufferArray()
{
    const auto text_sprite_screen_height = 0.14f;
    const auto text_sprite_screen_width = text_sprite_screen_height * 0.6f;

    text_ = vid_lib::sprite::GeometryGenerator::MakeVerticalText(
        "JAN.29 2023\nPM 6:41", -0.85f, -0.8f, text_sprite_screen_width, text_sprite_screen_height, *font_atlas_);

    text_buffer_array_ = std::make_unique<vid_lib::opengl::buffer::SpriteBufferArray>(text_->GetSprites());
}

void Processor::MakeDecalsBufferArray()
{
    const auto decal_sprite_screen_width = 0.16f;
    const auto decal_sprite_screen_height = decal_sprite_screen_width * 2.5f;

    decals_ = vid_lib::sprite::GeometryGenerator::MakeRandomlyPlacedSprites(
        *decals_atlas_, 16, decal_sprite_screen_width, decal_sprite_screen_height, random_source_);

    decals_buffer_array_ = std::make_unique<vid_lib::opengl::buffer::SpriteBufferArray>(decals_->GetSprites());
}

void Processor::RenderFirstPass()
{
    program_1a_->Use();
    program_1a_->SetUniform("u_verticalScale", vertical_scale_);
    program_1a_->SetUniform("u_image", 0);
    program_1a_->SetUniform("u_filmMarginColor",
                            config_.film_margin_color[0],
                            config_.film_margin_color[1],
                            config_.film_margin_color[2]);
    program_1a_->SetUniform("u_filmMarginEdges",
                            film_margin_edges_[0], film_margin_edges_[1],
                            film_margin_edges_[2], film_margin_edges_[3]);

    first_framebuffer_->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();
}

void Processor::RenderSecondPass()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program_1b_->Use();
    {
        program_1b_->SetUniform("u_image", 0);
        program_1b_->SetUniform("u_fontImage", 1);
        program_1b_->SetUniform("u_spriteScreenSize", text_->GetSpriteScreenWidth(), text_->GetSpriteScreenHeight());
        program_1b_->SetUniform("u_spriteTextureSize",
                                font_atlas_->GetSpriteTextureWidth(), font_atlas_->GetSpriteTextureHeight());
        program_1b_->SetUniform("u_spriteRotation", 3.14f / 2.0f);
        text_buffer_array_->Render();
        glFlush();
    }
}

void Processor::RenderThirdPass()
{
    if (random_source_.GetNextFloat() < config_.artifacts_chance)
    {
        program_1b_->SetUniform("u_fontImage", 2);
        program_1b_->SetUniform("u_spriteScreenSize",
                                decals_->GetSpriteScreenWidth(), decals_->GetSpriteScreenHeight());
        program_1b_->SetUniform("u_spriteTextureSize",
                                decals_atlas_->GetSpriteTextureWidth(), decals_atlas_->GetSpriteTextureHeight());
        program_1b_->SetUniform("u_spriteRotation", 0.0f);
        const auto instance_number = 1;
        const auto first_instance = random_source_.GetNextInt() % (decals_->GetSpriteCount() - instance_number);
        decals_buffer_array_->Render(first_instance, instance_number);
        glFlush();
    }

    glDisable(GL_BLEND);
}

void Processor::RenderFourthPass()
{
    program_1c_->Use();
    program_1c_->SetUniform("u_image", 0);
    program_1c_->SetUniform("u_time", time_);
    first_framebuffer_->BindTexture();
    second_framebuffer_->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();
}

void Processor::RenderFifthPass()
{
    program_2a_->Use();
    program_2a_->SetUniform("u_image", 0);
    second_framebuffer_->BindTexture();
    third_framebuffer_->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glFlush();
}

void Processor::UpdateTime()
{
    time_ += time_step_;
}

