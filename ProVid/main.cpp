#include <array>
#include <iostream>
#include <sstream>

#include <glad/gl.h>
#include <glfw/glfw3.h>
#include <opencv2/videoio.hpp>

#include "File/File.h"

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

void GLAPIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
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

    cv::VideoCapture original("../../Assets/Movies/original.MOV");
    cv::VideoCapture modified("../../Assets/Movies/modified.MP4");

    const auto vs_shader_code = provid::file::File::ReadWholeBinaryFile(
        "../../Assets/Shaders/full_screen_quad.vs");
    const auto vs_shader_id = CompileShader(GL_VERTEX_SHADER, vs_shader_code);

    const auto fs_shader_code = provid::file::File::ReadWholeBinaryFile(
        "../../Assets/Shaders/full_screen_quad_textured.fs");
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

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    auto b = 1;
    while (!glfwWindowShouldClose(window))
    {
        cv::Mat image;
        original >> image;
        if (!image.empty())
        {
            glClearColor(0.0, 0.5f, static_cast<float>(b) / 1000.0f, 1.0f);
            b = (b + 5) % 1000;
            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1024, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glUniform1i(glGetUniformLocation(screen_quad_program_id, "image"), 0);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glfwSwapBuffers(window);
        }

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
