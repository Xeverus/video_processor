#pragma once

#include <future>
#include <memory>

#include <vid_lib/video/video_writer.h>

namespace vid_lib::video
{

class AsyncVideoWriter
{
public:
    explicit AsyncVideoWriter(std::unique_ptr<VideoWriter> writer);
    ~AsyncVideoWriter();

    void Wait();
    void WriteFrame(const cv::Mat& image);

private:
    std::unique_ptr<vid_lib::video::VideoWriter> writer_;
    std::future<void> future_;
};

}