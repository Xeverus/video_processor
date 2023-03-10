#pragma once

#include <memory>
#include <string>
#include <vector>

#include <vid_lib/opengl/opengl.h>

namespace vid_lib::opengl::shader
{

class Program
{
public:
    static std::unique_ptr<Program> MakeFromShaders(const std::vector<GLuint>& shaders);

    static std::unique_ptr<Program> MakeFromFiles(const std::string& vs_filepath,
                                                  const std::string& fs_filepath);

public:
    ~Program();

    Program(const Program& other) = delete;
    Program(Program&& other) = default;
    Program operator=(const Program& other) = delete;
    Program& operator=(Program&& other) = default;

    void Use() const;

    void SetUniform(const std::string& uniform_name, GLint value) const;

    void SetUniform(const std::string& uniform_name, GLfloat value) const;
    void SetUniform(const std::string& uniform_name, GLfloat value, GLfloat second_value) const;
    void SetUniform(const std::string& uniform_name, GLfloat value, GLfloat second_value, GLfloat third_value) const;
    void SetUniform(const std::string& uniform_name,
                    GLfloat value, GLfloat second_value, GLfloat third_value, GLfloat fourth_value) const;

    GLint GetAttributeLocation(const std::string& attribute_name) const;

private:
    GLuint id_;

private:
    explicit Program(GLuint id);
};

}