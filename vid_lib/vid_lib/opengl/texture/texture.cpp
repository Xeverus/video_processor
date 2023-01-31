#include <vid_lib/opengl/texture/texture.h>

#include <opencv2/imgcodecs.hpp>

namespace vid_lib::opengl::texture
{

std::unique_ptr<Texture> Texture::MakeFromFile(const std::string& filepath)
{
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    auto texture = std::unique_ptr<Texture>(new Texture(texture_id));
    texture->Update(cv::imread(filepath));

    return std::move(texture);
}

Texture::~Texture()
{
    glDeleteTextures(1, &id_);
}

Texture::Texture(const GLuint id)
    : id_(id)
{
}

void Texture::Update(const cv::Mat& image) const
{
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

std::unique_ptr<Texture> Texture::MakeEmpty()
{
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);

    return std::unique_ptr<Texture>(new Texture(texture_id));
}

}