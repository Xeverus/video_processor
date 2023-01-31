#pragma once

#include <vector>

#include <vid_lib/opengl/opengl.h>

namespace vid_lib::opengl::buffer
{

class Buffer
{
public:
    template <typename T>
    explicit Buffer(const std::vector<T>& data);

    ~Buffer();

    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) = default;
    Buffer operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) = default;

    void Bind() const;

private:
    GLuint id_;

private:
    Buffer(const void* data, std::size_t bytesize);
};

template <typename T>
Buffer::Buffer(const std::vector<T>& data)
    : Buffer(data.data(), data.size() * sizeof(data[0]))
{
}

}