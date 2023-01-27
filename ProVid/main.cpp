#include <array>
#include <iostream>
#include <sstream>

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <opencv2/videoio.hpp>

#include "Filesystem/BinaryFile.h"
#include "Video/VideoReader.h"
#include "Video/VideoWriter.h"

GLuint CompileShader(GLenum shader_type, const std::vector<char>& shader_code)
{
    const auto shader_id = glCreateShader(shader_type);

    const auto shader_code_bytes = shader_code.data();
    const auto shader_code_length = static_cast<GLint>(shader_code.size());
    glShaderSource(shader_id, 1, &shader_code_bytes, &shader_code_length);

    glCompileShader(shader_id);
    auto code = GLint{0};
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &code);
    if (!code)
    {
        char info[256];
        glGetShaderInfoLog(shader_id, 256, nullptr, info);

        std::stringstream str;
        str << "Failed to compile shader: '" << info << "'";
        throw std::runtime_error(str.str());
    }

    return shader_id;
}

GLuint MakeProgram(const std::vector<GLuint>& shaders)
{
    const auto program_id = glCreateProgram();
    for (const auto shader_id: shaders)
    {
        glAttachShader(program_id, shader_id);
    }

    glLinkProgram(program_id);
    auto code = GLint{0};
    glGetProgramiv(program_id, GL_LINK_STATUS, &code);
    if (!code)
    {
        char info[256];
        glGetProgramInfoLog(program_id, 256, nullptr, info);

        std::stringstream str;
        str << "Failed to make program: '" << info << "'";
        throw std::runtime_error(str.str());
    }

    return program_id;
}

void GLAPIENTRY
glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
              const void* userParam)
{
    std::cout << message << std::endl;
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    const auto window = glfwCreateWindow(1920, 1024, "ProVid", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    const auto vs_shader_code = provid::filesystem::BinaryFile::ReadWhole(
        "../../Assets/Shaders/full_screen_quad.vs");
    const auto vs_shader_id = CompileShader(GL_VERTEX_SHADER, vs_shader_code);

    const auto fs_shader_code = provid::filesystem::BinaryFile::ReadWhole(
        "../../Assets/Shaders/full_screen_quad_textured_with_filter.fs");
    const auto fs_shader_id = CompileShader(GL_FRAGMENT_SHADER, fs_shader_code);

    const auto screen_quad_program_id = MakeProgram({vs_shader_id, fs_shader_id});
    glDeleteShader(vs_shader_id);
    glDeleteShader(fs_shader_id);

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

    glUseProgram(screen_quad_program_id);

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
        //"../../Assets/Movies/original.MOV";
        "../../Assets/Movies/modified.MP4";
    provid::video::VideoReader video(video_path);

    glfwSetWindowSize(window, video.GetFrameWidth(), video.GetFrameHeight());
    glViewport(0, 0, video.GetFrameWidth(), video.GetFrameHeight());

    provid::video::VideoWriter new_movie("../../Assets/Movies/new_movie.mp4", video.GetFrameWidth(),
                                         video.GetFrameHeight(),
                                         video.GetFramesPerSecond());

    auto frames_to_convert = 300;
    while (!glfwWindowShouldClose(window))
    {
        auto image = video.GetNextFrame();
        if (!image.empty() && frames_to_convert > 0)
        {
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glUniform1i(glGetUniformLocation(screen_quad_program_id, "image"), 0);

            ///
            //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            //glViewport(0, 0, 1920, 1024);
            ///

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            cv::Mat output_image(image.rows, image.cols, image.type());
            glReadPixels(0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, output_image.data);
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

    glfwTerminate();

    return 0;
}
