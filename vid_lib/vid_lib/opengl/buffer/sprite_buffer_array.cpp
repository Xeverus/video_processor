#include <vid_lib/opengl/buffer/sprite_buffer_array.h>

#include <vid_lib/opengl/buffer/buffer.h>

namespace vid_lib::opengl::buffer
{
namespace
{

const void* MakeBufferOffset(const int offset)
{
    return reinterpret_cast<const void*>(offset);
}

std::unique_ptr<Buffer> MakeBuffer(const std::vector<sprite::Sprite>& sprites)
{
    auto sprite_buffer = std::make_unique<Buffer>(sprites);
    sprite_buffer->Bind();

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, MakeBufferOffset(0));
    glVertexAttribDivisor(0, 1);

    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, MakeBufferOffset(8));
    glVertexAttribDivisor(1, 1);

    return sprite_buffer;
}

}

SpriteBufferArray::SpriteBufferArray(const std::vector<sprite::Sprite>& sprites)
    : id_(0)
    , sprites_count_(sprites.size())
{
    glGenVertexArrays(1, &id_);
    glBindVertexArray(id_);
    sprites_buffer_ = MakeBuffer(sprites);
}

SpriteBufferArray::~SpriteBufferArray()
{
    glDeleteVertexArrays(1, &id_);
}

void SpriteBufferArray::Render() const
{
    Render(0, sprites_count_);
}

void SpriteBufferArray::Render(const std::size_t first_sprite) const
{
    Render(first_sprite, sprites_count_ - first_sprite);
}

void SpriteBufferArray::Render(const std::size_t first_sprite,
                               const std::size_t sprites_count) const
{
    glBindVertexArray(id_);
    glDrawArraysInstancedBaseInstance(GL_TRIANGLE_STRIP,
                                      0,
                                      4,
                                      static_cast<GLsizei>(sprites_count),
                                      static_cast<GLsizei>(first_sprite));
}

}