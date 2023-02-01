#pragma once

#include <ostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

namespace vid_lib::video
{

class VideoWriter
{
public:
    VideoWriter(const std::string& filepath, int width, int height, double fps);

    void WriteFrame(const cv::Mat& frame);

private:
    cv::VideoWriter video_writer_;
};

}