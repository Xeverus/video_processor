#include <vid_lib/opengl/texture/framebuffer.h>

#include <stdexcept>

namespace vid_lib::opengl::texture
{
namespace
{

GLuint MakeOpenGlFramebuffer(const int width, const int height, const GLuint color_texture_id)
{
    GLuint framebuffer_id = 0;
    glGenFramebuffers(1, &framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    glBindTexture(GL_TEXTURE_2D, color_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture_id, 0);

    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("Failed to create framebuffer");
    }

    return framebuffer_id;
}

}

Framebuffer Framebuffer::MakeNew(const int width, const int height)
{
    GLuint color_texture_id = 0;
    glGenTextures(1, &color_texture_id);
    const auto framebuffer_id = MakeOpenGlFramebuffer(width, height, color_texture_id);
    return Framebuffer(framebuffer_id, color_texture_id, width, height);
}

Framebuffer Framebuffer::WrapDefault(const int width, const int height)
{
    return Framebuffer(0, 0, width, height);
}

Framebuffer::Framebuffer(const GLuint framebuffer_id, const GLuint color_texture_id, const int width, const int height)
    : framebuffer_id_(framebuffer_id)
    , color_texture_id_(color_texture_id)
    , width_(width)
    , height_(height)
{
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
    glViewport(0, 0, width_, height_);
}

void Framebuffer::BindTexture() const
{
    if (color_texture_id_ == 0)
    {
        throw std::runtime_error("invalid texture id; probably trying to bind default texture of default framebuffer");
    }

    glBindTexture(GL_TEXTURE_2D, color_texture_id_);
}

}