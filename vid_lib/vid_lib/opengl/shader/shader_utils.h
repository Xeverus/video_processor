#pragma once

#include <string>
#include <vector>

#include <vid_lib/opengl/opengl.h>

namespace vid_lib::opengl::shader
{

class ShaderUtils
{
public:
    struct VertexAttribute
    {
        GLuint index;
        std::string name;
    };

public:
    static GLuint CompileShader(GLenum shader_type, const std::vector<char>& shader_code);
};

}