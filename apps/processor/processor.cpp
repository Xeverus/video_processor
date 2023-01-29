#include <processor.h>

#include <array>
#include <iostream>

#include <vid_lib/math/aspect_ratio.h>
#include <vid_lib/math/film.h>
#include <vid_lib/math/random.h>

#include <vid_lib/opengl/debug/debug_messenger.h>
#include <vid_lib/opengl/shader/shader_utils.h>
#include <vid_lib/opengl/texture/framebuffer.h>

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
            24,
            3
        };
}

void LoadImageToOpenGlTexture(cv::Mat& image, const GLuint texture_id)
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
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

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const auto vertical_scale = vid_lib::math::AspectRatio::CalculateVerticalScale(
        input_movie.GetFrameWidth(), input_movie.GetFrameHeight(), config_.output_movie_width,
        config_.output_movie_height);

    GLuint image_texture_id = 0;
    glGenTextures(1, &image_texture_id);

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

    const auto program_1a = vid_lib::opengl::shader::ShaderUtils::MakeProgramFromFiles(
        "../../../Assets/Shaders/processor_1a.vs", "../../../Assets/Shaders/processor_1a.fs");
    const auto program_2a = vid_lib::opengl::shader::ShaderUtils::MakeProgramFromFiles(
        "../../../Assets/Shaders/processor_2a.vs", "../../../Assets/Shaders/processor_2a.fs");

    auto input_image = input_movie.GetNextFrame();
    cv::Mat output_image(config_.output_movie_height, config_.output_movie_width, input_image.type());

    while (!glfwWindowShouldClose(glfw_window_) && !input_image.empty())
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        LoadImageToOpenGlTexture(input_image, image_texture_id);

        // phase 1a
        glUseProgram(program_1a);
        glUniform1f(glGetUniformLocation(program_1a, "u_verticalScale"), vertical_scale);
        glUniform1i(glGetUniformLocation(program_1a, "u_image"), 0);
        glUniform3fv(glGetUniformLocation(program_1a, "u_filmMarginColor"), 1, film_margin_color_.data());
        glUniform4fv(glGetUniformLocation(program_1a, "u_filmMarginEdges"), 1, film_margin_edges_.data());

        framebuffer_1a.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFlush();

        // phase 1b


        // phase 2a
        glUseProgram(program_2a);
        glUniform1i(glGetUniformLocation(program_2a, "u_image"), 0);
        framebuffer_1a.BindTexture();
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

