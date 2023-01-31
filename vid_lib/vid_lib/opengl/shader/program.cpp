#include <vid_lib/opengl/shader/program.h>

#include <stdexcept>
#include <sstream>

#include <vid_lib/filesystem/binary_file.h>

#include <vid_lib/opengl/shader/shader_utils.h>

namespace vid_lib::opengl::shader
{

std::unique_ptr<Program> Program::MakeFromShaders(const std::vector<GLuint>& shaders)
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

    return std::unique_ptr<Program>(new Program(program_id));
}

std::unique_ptr<Program> Program::MakeFromFiles(const std::string& vs_filepath,
                                                const std::string& fs_filepath)
{
    const auto vs_code = filesystem::BinaryFile::ReadWhole(vs_filepath);
    const auto fs_code = filesystem::BinaryFile::ReadWhole(fs_filepath);

    const auto vs_id = ShaderUtils::CompileShader(GL_VERTEX_SHADER, vs_code);
    const auto fs_id = ShaderUtils::CompileShader(GL_FRAGMENT_SHADER, fs_code);
    auto program = MakeFromShaders({vs_id, fs_id});

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);

    return std::move(program);
}

Program::Program(GLuint id)
    : id_(id)
{
}

Program::~Program()
{
    glDeleteProgram(id_);
}

void Program::Use() const
{
    glUseProgram(id_);
}

void Program::SetUniform(const std::string& uniform_name, GLint value) const
{
    glUniform1i(glGetUniformLocation(id_, uniform_name.c_str()), value);
}

void Program::SetUniform(const std::string& uniform_name, GLfloat value) const
{
    glUniform1f(glGetUniformLocation(id_, uniform_name.c_str()), value);
}

void Program::SetUniform(const std::string& uniform_name,
                         GLfloat value, GLfloat second_value) const
{
    glUniform2f(glGetUniformLocation(id_, uniform_name.c_str()),
                value, second_value);
}

void Program::SetUniform(const std::string& uniform_name,
                         GLfloat value, GLfloat second_value, GLfloat third_value) const
{
    glUniform3f(glGetUniformLocation(id_, uniform_name.c_str()),
                value, second_value, third_value);
}

void Program::SetUniform(const std::string& uniform_name,
                         GLfloat value, GLfloat second_value, GLfloat third_value, GLfloat fourth_value) const
{
    glUniform4f(glGetUniformLocation(id_, uniform_name.c_str()),
                value, second_value, third_value, fourth_value);
}

GLint Program::GetAttributeLocation(const std::string& attribute_name) const
{
    return glGetAttribLocation(id_, attribute_name.c_str());
}

}