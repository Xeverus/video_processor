#pragma once

#include <memory>
#include <string>

#include <opencv2/core/mat.hpp>

#include <vid_lib/opengl/opengl.h>

namespace vid_lib::opengl::texture
{

class Texture
{
public:
    static std::unique_ptr<Texture> MakeFromFile(const std::string& filepath);
    static std::unique_ptr<Texture> MakeEmpty();

public:
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) = default;
    Texture operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) = default;

    ~Texture();

    void Update(const cv::Mat& image) const;

private:
    GLuint id_;

private:
    explicit Texture(GLuint id);
};

}