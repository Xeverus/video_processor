#include <vid_lib/opengl/shader/shader_utils.h>

#include <sstream>
#include <stdexcept>

#include <vid_lib/filesystem/binary_file.h>

namespace vid_lib::opengl::shader
{

GLuint ShaderUtils::CompileShader(GLenum shader_type, const std::vector<char>& shader_code)
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

GLuint ShaderUtils::MakeProgramFromShaders(const std::vector<GLuint>& shaders)
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

GLuint ShaderUtils::MakeProgramFromFiles(const std::string& vs_filepath, const std::string& fs_filepath)
{
    const auto vs_code = filesystem::BinaryFile::ReadWhole(vs_filepath);
    const auto fs_code = filesystem::BinaryFile::ReadWhole(fs_filepath);

    const auto vs_id = CompileShader(GL_VERTEX_SHADER, vs_code);
    const auto fs_id = CompileShader(GL_FRAGMENT_SHADER, fs_code);
    const auto program_id = MakeProgramFromShaders({vs_id, fs_id});

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);

    return program_id;
}

}