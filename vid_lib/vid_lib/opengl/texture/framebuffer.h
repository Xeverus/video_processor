#pragma once

#include <vid_lib/opengl/opengl.h>

namespace vid_lib::opengl::texture
{

class Framebuffer
{
public:
    static Framebuffer MakeNew(int width, int height);

    static Framebuffer WrapDefault(int width, int height);

    Framebuffer(const Framebuffer& other) = delete;

    Framebuffer operator=(const Framebuffer& other) = delete;

    void Bind() const;
    void BindTexture() const;

private:
    GLuint framebuffer_id_;
    GLuint color_texture_id_;
    int width_;
    int height_;

private:
    explicit Framebuffer(GLuint framebuffer_id, GLuint color_texture_id, int width, int height);
};

}