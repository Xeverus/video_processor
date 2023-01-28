#pragma once

#include <string>
#include <vector>

#include <vid_lib/opengl/opengl.h>

namespace vid_lib::opengl::shader
{

class ShaderUtils
{
public:
    static GLuint CompileShader(GLenum shader_type, const std::vector<char>& shader_code);

    static GLuint MakeProgramFromShaders(const std::vector<GLuint>& shaders);

    static GLuint MakeProgramFromFiles(const std::string& vs_filepath, const std::string& fs_filepath);
};

}