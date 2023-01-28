#include <processor.h>

#include <array>
#include <iostream>

#include <vid_lib/math/aspect_ratio.h>
#include <vid_lib/math/film.h>
#include <vid_lib/math/random.h>

#include <vid_lib/opengl/debug/debug_messenger.h>
#include <vid_lib/opengl/shader/shader_utils.h>

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
            12,
            3
        };
}

void LoadImageToOpenGlTexture(cv::Mat& image, const GLuint texture_id)
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GLuint CreateFramebuffer(const GLuint texture_id, const int width, const int height)
{
    GLuint framebuffer_id = 0;
    glGenFramebuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_id, 0);

    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("Failed to use off-screen frame buffer");
    }

    return framebuffer_id;
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

    std::array<GLuint, 2> texture_ids = {0};
    glGenTextures(texture_ids.size(), texture_ids.data());
    GLuint image_texture_id = texture_ids[0];
    GLuint framebuffer_texture_id = texture_ids[1];

    const auto framebuffer_id = CreateFramebuffer(framebuffer_texture_id, config_.output_movie_width,
                                                  config_.output_movie_height);

    glfwSetWindowSize(glfw_window_, config_.output_movie_width, config_.output_movie_height);
    glViewport(0, 0, config_.output_movie_width, config_.output_movie_height);

    film_margin_edges_ = vid_lib::math::Film::CalculateMarginEdges(config_.output_movie_height,
                                                                   config_.film_margin_size, config_.film_margin_step);

    glActiveTexture(GL_TEXTURE0 + 0);
    glViewport(0, 0, config_.output_movie_width, config_.output_movie_height);

    const auto program = vid_lib::opengl::shader::ShaderUtils::MakeProgramFromFiles(
        "../../../Assets/Shaders/processor_0.vs", "../../../Assets/Shaders/processor_0.fs");
    glUseProgram(program);

    glUniform1f(glGetUniformLocation(program, "u_verticalScale"), vertical_scale);
    glUniform1i(glGetUniformLocation(program, "u_image"), 0);
    glUniform3fv(glGetUniformLocation(program, "u_filmMarginColor"), 1, film_margin_color_.data());
    glUniform4fv(glGetUniformLocation(program, "u_filmMarginEdges"), 1, film_margin_edges_.data());

    auto input_image = input_movie.GetNextFrame();
    cv::Mat output_image(config_.output_movie_height, config_.output_movie_width, input_image.type());

    while (!glfwWindowShouldClose(glfw_window_) && !input_image.empty())
    {
        LoadImageToOpenGlTexture(input_image, image_texture_id);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glReadPixels(0, 0, config_.output_movie_width, config_.output_movie_height, GL_BGR, GL_UNSIGNED_BYTE,
                     output_image.data);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(glfw_window_);

        output_movie.WriteFrame(output_image);

        glfwPollEvents();

        input_image = input_movie.GetNextFrame();
    }
}

