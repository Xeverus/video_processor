#pragma once

#include <future>
#include <memory>

#include <vid_lib/video/video_reader.h>

namespace vid_lib::video
{

class AsyncVideoReader
{
public:
    explicit AsyncVideoReader(std::unique_ptr<VideoReader> reader);
    ~AsyncVideoReader();

    void Wait();
    void ReadFrame(cv::Mat& image);

private:
    std::unique_ptr<vid_lib::video::VideoReader> reader_;
    std::future<cv::Mat> future_;
};

}