#include <processor.h>

#include <fstream>

#include <opencv2/imgcodecs.hpp>

#include <vid_lib/math/aspect_ratio.h>
#include <vid_lib/math/film.h>
#include <vid_lib/math/random.h>

#include <vid_lib/opengl/buffer/buffer.h>
#include <vid_lib/opengl/debug/debug_messenger.h>
#include <vid_lib/opengl/shader/program.h>
#include <vid_lib/opengl/texture/framebuffer.h>

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

void LoadImageToOpenGlTexture(cv::Mat& image, const GLuint texture_id)
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

    GLuint image_texture_id = 0;
    GLuint font_texture_id = 0;
    GLuint decals_texture_id = 0;
    glGenTextures(1, &image_texture_id);
    glGenTextures(1, &font_texture_id);
    glGenTextures(1, &decals_texture_id);

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
        "../../../assets/shaders/processor/processor_1a.vs", "../../../assets/shaders/processor/processor_1a.fs", {});
    auto program_1b = vid_lib::opengl::shader::Program::MakeFromFiles(
        "../../../assets/shaders/processor/processor_1b.vs", "../../../assets/shaders/processor/processor_1b.fs",
        {
            {0, "in_letterPosition"},
            {1, "in_letterTextureCoords"}
        });
    auto program_1c = vid_lib::opengl::shader::Program::MakeFromFiles(
        "../../../assets/shaders/processor/processor_1c.vs", "../../../assets/shaders/processor/processor_1c.fs", {});
    auto program_2a = vid_lib::opengl::shader::Program::MakeFromFiles(
        "../../../assets/shaders/processor/processor_2a.vs", "../../../assets/shaders/processor/processor_2a.fs", {});

    auto input_image = input_movie.GetNextFrame();
    cv::Mat output_image(config_.output_movie_height, config_.output_movie_width, input_image.type());

    const auto font_atlas = vid_lib::sprite::Atlas("../../../assets/sprites/font_48x80.txt");
    cv::Mat font_image = cv::imread("../../../assets/sprites/font_48x80.png");
    glActiveTexture(GL_TEXTURE0 + 1);
    LoadImageToOpenGlTexture(font_image, font_texture_id);

    const auto decals_atlas = vid_lib::sprite::Atlas("../../../assets/sprites/decals_64x128.txt");
    cv::Mat decals_image = cv::imread("../../../assets/sprites/decals_64x128.png");
    glActiveTexture(GL_TEXTURE0 + 2);
    LoadImageToOpenGlTexture(decals_image, decals_texture_id);

    const auto text_height = 0.14f;
    const auto text_width = text_height * 0.6f;
    auto text = vid_lib::sprite::GeometryGenerator::MakeVerticalText(
        "JAN.29 2023", -0.85f, -0.8f, text_width, text_height, font_atlas);
    const auto text_bottom = vid_lib::sprite::GeometryGenerator::MakeVerticalText(
        "PM 6:41", -0.7f, -0.8f, text_width, text_height, font_atlas);
    text.insert(text.end(), text_bottom.begin(), text_bottom.end());

    vid_lib::math::Random random;
    std::vector<vid_lib::sprite::GeometryGenerator::SpriteVertex> decals;
    decals.reserve(16);
    const auto decal_width = 0.16f;
    const auto decal_height = decal_width * 2.5f;
    {
        for (auto i = 0; i < 16; ++i)
        {
            const char decal_name = 'a' + static_cast<char>(random.GetNextInt() % 8);
            const auto sprite_description = decals_atlas.GetSpriteDescription(decal_name);
            const auto pos_x = (random.GetNextFloat() * 2.0f - 1.0f) * 0.8f;
            const auto pos_y = (random.GetNextFloat() * 2.0f - 1.0f) * 0.8f;
            const auto decal = vid_lib::sprite::GeometryGenerator::MakeSprite(sprite_description, pos_x, pos_y,
                                                                              decal_width,
                                                                              decal_height);
            decals.push_back(decal);
        }
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    vid_lib::opengl::buffer::Buffer text_buffer(text);
    vid_lib::opengl::buffer::Buffer decals_buffer(decals);

    const auto step = 0.73432117f;
    auto time = 0.0f;
    while (!glfwWindowShouldClose(glfw_window_) && !input_image.empty())
    {
        time += step;

        glActiveTexture(GL_TEXTURE0 + 0);
        LoadImageToOpenGlTexture(input_image, image_texture_id);

        // phase 1a - add margins, apply aspect
        program_1a->Use();
        program_1a->SetUniform("u_verticalScale", vertical_scale);
        program_1a->SetUniform("u_image", 0);
        program_1a->SetUniform("u_filmMarginColor", film_margin_color_[0], film_margin_color_[1], film_margin_color_[2]);
        program_1a->SetUniform("u_filmMarginEdges", film_margin_edges_[0], film_margin_edges_[1], film_margin_edges_[2], film_margin_edges_[3]);

        framebuffer_1a.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFlush();

        // phase 1b - add text and white decals
        program_1b->Use();
        program_1b->SetUniform("u_image", 0);
        program_1b->SetUniform("u_fontImage", 1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        {
            text_buffer.Bind();
            const auto position_location = program_1b->GetAttributeLocation("in_letterPosition");
            const auto coord_location = program_1b->GetAttributeLocation("in_letterTextureCoords");
            glEnableVertexAttribArray(position_location);
            glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 16, (const void*)0);
            glVertexAttribDivisor(position_location, 1);
            glEnableVertexAttribArray(coord_location);
            glVertexAttribPointer(coord_location, 2, GL_FLOAT, GL_FALSE, 16, (const void*)8);
            glVertexAttribDivisor(coord_location, 1);

            program_1b->SetUniform("u_spriteSize", text_width, text_height);
            program_1b->SetUniform("u_spriteTextureSize", font_atlas.GetSpriteTextureWidth(),
                                   font_atlas.GetSpriteTextureHeight());
            program_1b->SetUniform("u_spriteRotation", 3.14f / 2.0f);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, text.size());
            glFlush();
        }

        if (random.GetNextFloat() < 0.03f)
        {
            decals_buffer.Bind();
            const auto position_location = program_1b->GetAttributeLocation("in_letterPosition");
            const auto coord_location = program_1b->GetAttributeLocation("in_letterTextureCoords");
            glEnableVertexAttribArray(position_location);
            glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 16, (const void*)0);
            glVertexAttribDivisor(position_location, 1);
            glEnableVertexAttribArray(coord_location);
            glVertexAttribPointer(coord_location, 2, GL_FLOAT, GL_FALSE, 16, (const void*)8);
            glVertexAttribDivisor(coord_location, 1);

            program_1b->SetUniform("u_fontImage", 2);
            program_1b->SetUniform("u_spriteSize", decal_width, decal_height);
            program_1b->SetUniform("u_spriteTextureSize", decals_atlas.GetSpriteTextureWidth(),
                                   decals_atlas.GetSpriteTextureHeight());
            program_1b->SetUniform("u_spriteRotation", 0.0f);
            const auto instance_number = 1;
            const auto first_instance = random.GetNextInt() % (decals.size() - instance_number);
            glDrawArraysInstancedBaseInstance(GL_TRIANGLE_STRIP, 0, 4, instance_number, first_instance);
            glFlush();
        }
        glDisable(GL_BLEND);

        // phase 1c - apply postprocessing
        program_1c->Use();
        program_1c->SetUniform("u_image", 0);
        program_1c->SetUniform("u_time", time);
        framebuffer_1a.BindTexture();
        framebuffer_1b.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFlush();

        // phase 2a - apply channel separation and rescaling
        program_2a->Use();
        program_2a->SetUniform("u_image", 0);
        framebuffer_1b.BindTexture();
        framebuffer_2a.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFlush();

        glReadPixels(0, 0, config_.output_movie_width, config_.output_movie_height, GL_BGR, GL_UNSIGNED_BYTE,
                     output_image.data);
        output_movie.WriteFrame(output_image);

        glfwPollEvents();

        input_image = input_movie.GetNextFrame();
    }
}

