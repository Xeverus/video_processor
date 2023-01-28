#include <shader_lab.h>

#include <iostream>

#include <vid_lib/opengl/debug/debug_messenger.h>
#include <vid_lib/opengl/shader/shader_utils.h>

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
            if (app->up)
            {
                app->tint_[0] += 0.01;
                app->tint_[2] += 0.01;
            }
            else
            {
                app->tint_[0] -= 0.01;
                app->tint_[2] -= 0.01;
            }
            break;
        case GLFW_KEY_2:
            if (app->up)
            {
                app->tint_[1] += 0.01;
            }
            else
            {
                app->tint_[1] -= 0.01;
            }
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

    const auto program = vid_lib::opengl::shader::ShaderUtils::MakeProgramFromFiles(
        "../../../Assets/Shaders/vhs_filter.vs", "../../../Assets/Shaders/vhs_filter.fs");
    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    const auto video_path = "../../../Assets/Movies/original.MOV";
    vid_lib::video::VideoReader video(video_path);

    glfwSetWindowSize(window, video.GetFrameWidth(), video.GetFrameHeight());
    glViewport(0, 0, video.GetFrameWidth(), video.GetFrameHeight());

    const auto image = video.GetNextFrame();

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

        glUniform2f(glGetUniformLocation(program, "uAspectRatio"), 1.0f, 1.0f);

        glUniform1i(glGetUniformLocation(program, "uImage"), 0);
        glUniform1f(glGetUniformLocation(program, "uBrightness"), brightness_);
        glUniform1f(glGetUniformLocation(program, "uContrast"), contrast_);
        glUniform1f(glGetUniformLocation(program, "uExposure"), exposure_);
        glUniform1f(glGetUniformLocation(program, "uSaturation"), saturation_);
        glUniform3f(glGetUniformLocation(program, "uTint"), tint_[0], tint_[1], tint_[2]);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
}