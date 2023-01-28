#include "ShaderLab.h"

#include <iostream>

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <opencv2/videoio.hpp>

#include "Filesystem/BinaryFile.h"
#include "Video/VideoReader.h"
#include "Video/VideoWriter.h"

namespace
{

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

float saturation = -0.15f;
float exposure = 0.05f;
float contrast = 0.15f;
float brightness = -0.1f;
float tint[3] = {1.0f, 1.0f, 1.0f};
bool up = false;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_LEFT_SHIFT)
    {
        up = action != GLFW_PRESS;
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
            brightness += up ? step : -step;
            break;
        case GLFW_KEY_S:
            saturation += up ? step : -step;
            break;
        case GLFW_KEY_E:
            exposure += up ? step : -step;
            break;
        case GLFW_KEY_C:
            contrast += up ? step : -step;
            break;
        case GLFW_KEY_1:
            if (up) {
                tint[0] += 0.01;
                tint[2] += 0.01;
            } else {
                tint[0] -= 0.01;
                tint[2] -= 0.01;
            }
            break;
        case GLFW_KEY_2:
            if (up) {
                tint[1] += 0.01;
            } else {
                tint[1] -= 0.01;
            }
            break;
        case GLFW_KEY_R:
            brightness = 0.0f;
            saturation = 0.0f;
            exposure = 0.0f;
            contrast = 0.0f;
            tint[0] = 1.0f;
            tint[1] = 1.0f;
            tint[2] = 1.0f;
            break;
    }

    std::cout
        << "; s=" << saturation
        << "; e=" << exposure
        << "; c=" << contrast
        << "; b=" << brightness
        << "; t=" << tint[0] << ',' << tint[1] << ',' << tint[2]
        << std::endl;
}

void main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    const auto window = glfwCreateWindow(1920, 1024, "ProVid", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, KeyCallback);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return;
    }
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    const auto vs_shader_code = provid::filesystem::BinaryFile::ReadWhole(
        "../../Assets/Shaders/vhs_filter.vs");
    const auto vs_shader_id = CompileShader(GL_VERTEX_SHADER, vs_shader_code);

    const auto fs_shader_code = provid::filesystem::BinaryFile::ReadWhole(
        "../../Assets/Shaders/vhs_filter.fs");
    const auto fs_shader_id = CompileShader(GL_FRAGMENT_SHADER, fs_shader_code);

    const auto screen_quad_program_id = MakeProgram({vs_shader_id, fs_shader_id});
    glDeleteShader(vs_shader_id);
    glDeleteShader(fs_shader_id);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glUseProgram(screen_quad_program_id);

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    const auto video_path = "../../Assets/Movies/original.MOV";
    provid::video::VideoReader video(video_path);

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

        glUniform2f(glGetUniformLocation(screen_quad_program_id, "uAspectRatio"), 1.0f, 1.0f);

        glUniform1i(glGetUniformLocation(screen_quad_program_id, "uImage"), 0);
        glUniform1f(glGetUniformLocation(screen_quad_program_id, "uBrightness"), brightness);
        glUniform1f(glGetUniformLocation(screen_quad_program_id, "uContrast"), contrast);
        glUniform1f(glGetUniformLocation(screen_quad_program_id, "uExposure"), exposure);
        glUniform1f(glGetUniformLocation(screen_quad_program_id, "uSaturation"), saturation);
        glUniform3f(glGetUniformLocation(screen_quad_program_id, "uTint"), tint[0], tint[1], tint[2]);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
}

}

void ShaderLab::run()
{
    main();
}