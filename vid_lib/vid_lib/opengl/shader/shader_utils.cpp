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

}