#include <shader_lab.h>

#include <iostream>

#include <vid_lib/math/aspect_ratio.h>
#include <vid_lib/math/film.h>
#include <vid_lib/math/random.h>

#include <vid_lib/opengl/debug/debug_messenger.h>
#include <vid_lib/opengl/shader/program.h>

#include <vid_lib/video/video_reader.h>

void ShaderLab::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto app = reinterpret_cast<ShaderLab*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_LEFT_SHIFT)
    {
        app->up = action != GLFW_PRESS;
        return;
    }

    if (action != GLFW_PRESS)
    {
        return;
    }

    const auto step = 0.05f;
    switch (key)
    {
        case GLFW_KEY_B:
            app->brightness_ += app->up ? step : -step;
            break;
        case GLFW_KEY_S:
            app->saturation_ += app->up ? step : -step;
            break;
        case GLFW_KEY_E:
            app->exposure_ += app->up ? step : -step;
            break;
        case GLFW_KEY_C:
            app->contrast_ += app->up ? step : -step;
            break;
        case GLFW_KEY_1:
            app->tint_[0] += app->up ? 0.01 : -0.01;
            break;
        case GLFW_KEY_2:
            app->tint_[1] += app->up ? 0.01 : -0.01;
            break;
        case GLFW_KEY_3:
            app->tint_[2] += app->up ? 0.01 : -0.01;
            break;
        case GLFW_KEY_R:
            app->brightness_ = 0.0f;
            app->saturation_ = 0.0f;
            app->exposure_ = 0.0f;
            app->contrast_ = 0.0f;
            app->tint_[0] = 1.0f;
            app->tint_[1] = 1.0f;
            app->tint_[2] = 1.0f;
            break;
    }

    std::cout
        << "; s=" << app->saturation_
        << "; e=" << app->exposure_
        << "; c=" << app->contrast_
        << "; b=" << app->brightness_
        << "; t=" << app->tint_[0] << ',' << app->tint_[1] << ',' << app->tint_[2]
        << std::endl;
}

void ShaderLab::Run()
{
    vid_lib::opengl::OpenGl open_gl;
    const auto window = open_gl.MakeWindow({1080, 1920, "Shader Lab"});
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, KeyCallback);
    vid_lib::opengl::debug::DebugMessenger::Enable();

    const auto program = vid_lib::opengl::shader::Program::MakeFromFiles(
        "../../../Assets/Shaders/vhs_filter.vs", "../../../Assets/Shaders/vhs_filter.fs");
    program->Use();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    const auto video_path = "../../../Assets/Movies/original.MOV";
    vid_lib::video::VideoReader video(video_path);

    const auto new_width = 480;
    const auto new_height = 640;

    const auto vertical_scale = vid_lib::math::AspectRatio::CalculateVerticalScale(
        video.GetFrameWidth(), video.GetFrameHeight(), new_width, new_height);

    const auto new_window_height = static_cast<int>(static_cast<float>(video.GetFrameHeight()) / vertical_scale);
    glfwSetWindowSize(window, video.GetFrameWidth(), new_window_height);
    glViewport(0, 0, video.GetFrameWidth(), new_window_height);

    film_margin_edges_ = vid_lib::math::Film::CalculateMarginEdges(new_window_height, 24, 8);

    cv::Mat image;
    video.ReadFrame(image);
    cv::flip(image, image, 0);

    vid_lib::math::Random random;
    while (!glfwWindowShouldClose(window))
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, video.GetFrameWidth(), video.GetFrameHeight(), 0, GL_BGR,
                     GL_UNSIGNED_BYTE, image.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        program->SetUniform("uVerticalScale", vertical_scale);
        program->SetUniform("uImage", 0);
        program->SetUniform("uBrightness", brightness_);
        program->SetUniform("uContrast", contrast_);
        program->SetUniform("uExposure", exposure_);
        program->SetUniform("uSaturation", saturation_);
        program->SetUniform("uTint", tint_[0], tint_[1], tint_[2]);
        program->SetUniform("uFilmMarginColor", film_margin_color_[0], film_margin_color_[1], film_margin_color_[2]);
        program->SetUniform("uFilmMarginEdges", film_margin_edges_[0], film_margin_edges_[1], film_margin_edges_[2], film_margin_edges_[3]);
        program->SetUniform("uRandomSeed", random.GetNextFloat());

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glFlush();

        glfwPollEvents();
    }
}
