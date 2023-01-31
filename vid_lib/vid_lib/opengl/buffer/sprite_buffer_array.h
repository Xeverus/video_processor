#pragma once

#include <vector>
#include <memory>

#include <vid_lib/opengl/opengl.h>

#include <vid_lib/sprite/sprite.h>

namespace vid_lib::opengl::buffer
{
class Buffer;

class SpriteBufferArray
{
public:
    explicit SpriteBufferArray(const std::vector<sprite::Sprite>& sprites);
    ~SpriteBufferArray();

    SpriteBufferArray(const SpriteBufferArray& other) = delete;
    SpriteBufferArray(SpriteBufferArray&& other) = default;
    SpriteBufferArray operator=(const SpriteBufferArray& other) = delete;
    SpriteBufferArray& operator=(SpriteBufferArray&& other) = default;

    void Render() const;
    void Render(std::size_t first_sprite) const;
    void Render(std::size_t first_sprite, std::size_t sprites_count) const;

private:
    GLuint id_;
    std::unique_ptr<Buffer> sprites_buffer_;
    std::size_t sprites_count_;
};

}