#include <playground.h>

#include <vid_lib/opengl/opengl.h>
#include <vid_lib/opengl/debug/debug_messenger.h>
#include <vid_lib/opengl/shader/shader_utils.h>

#include <vid_lib/video/video_reader.h>
#include <vid_lib/video/video_writer.h>

void Playground::Run()
{
    vid_lib::opengl::OpenGl open_gl;
    const auto window = open_gl.MakeWindow({1080, 1920, "Playground"});
    vid_lib::opengl::debug::DebugMessenger::Enable();

    const auto program = vid_lib::opengl::shader::ShaderUtils::MakeProgramFromFiles(
        "../../../Assets/Shaders/vhs_filter.vs", "../../../Assets/Shaders/vhs_filter.fs");
    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    ///// !!! not needed
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexBuffer(0, vbo, 0, 0);
    ///// !!! not needed

    ///// render to texture
    /*GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLuint render_texture;
    glGenTextures(1, &render_texture);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
       throw std::runtime_error("Failed to use off-screen frame buffer");
    }*/
    ///// !!! render to texture

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    const auto video_path =
        "../../../Assets/Movies/original.MOV";
    //"../../../Assets/Movies/modified.MP4";
    vid_lib::video::VideoReader video(video_path);

    const auto old_width = video.GetFrameWidth();
    const auto old_height = video.GetFrameHeight();
    const auto new_width = 480;
    const auto new_height = 640;
    const auto new_fps = video.GetFramesPerSecond();

    auto CalculateAspectRatio = [old_width, old_height, new_width, new_height]()
    {
        const auto ow = static_cast<float>(old_width);
        const auto oh = static_cast<float>(old_height);
        const auto nw = static_cast<float>(new_width);
        const auto nh = static_cast<float>(new_height);

        return (oh / nh) / (ow / nw);
    };

    const auto aspect_x = 1.0f;
    const auto aspect_y = CalculateAspectRatio();

    glfwSetWindowSize(window, new_width, new_height);
    glViewport(0, 0, new_width, new_height);

    vid_lib::video::VideoWriter new_movie("../../../Assets/Movies/changed.mp4", new_width, new_height, new_fps);

    auto frames_to_convert = 300;
    while (!glfwWindowShouldClose(window))
    {
        auto image = video.GetNextFrame();
        if (!image.empty() && frames_to_convert > 0)
        {
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, video.GetFrameWidth(), video.GetFrameHeight(), 0, GL_BGR,
                         GL_UNSIGNED_BYTE, image.data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glUniform2f(glGetUniformLocation(program, "uAspectRatio"), aspect_x, aspect_y);

            glUniform1i(glGetUniformLocation(program, "uImage"), 0);
            glUniform1f(glGetUniformLocation(program, "uSaturation"), saturation_);
            glUniform1f(glGetUniformLocation(program, "uExposure"), exposure_);
            glUniform1f(glGetUniformLocation(program, "uContrast"), contrast_);
            glUniform1f(glGetUniformLocation(program, "uBrightness"), brightness_);
            glUniform3f(glGetUniformLocation(program, "uTint"), tint_[0], tint_[1], tint_[2]);

            ///
            //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            //glViewport(0, 0, 1920, 1024);
            ///

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            cv::Mat output_image(new_height, new_width, image.type());
            glReadPixels(0, 0, new_width, new_height, GL_BGR, GL_UNSIGNED_BYTE, output_image.data);
            new_movie.WriteFrame(output_image);

            glfwSwapBuffers(window);
            --frames_to_convert;
        }
        else
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwPollEvents();
    }
}