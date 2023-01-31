#include <vid_lib/opengl/buffer/buffer.h>

#include <stdexcept>

namespace vid_lib::opengl::buffer
{

Buffer::~Buffer()
{
    glDeleteBuffers(1, &id_);
}

void Buffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id_);
}

Buffer::Buffer(const void* data, std::size_t bytesize)
{
    glGenBuffers(1, &id_);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytesize), data, GL_STATIC_DRAW);
}

}