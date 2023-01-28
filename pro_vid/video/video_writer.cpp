#include "VideoWriter.h"

#include <iostream>

namespace provid::video
{

VideoWriter::VideoWriter(const std::string& filepath, int width, int height, double fps)
    : video_writer_(filepath,
                    cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                    fps,
                    cv::Size(width, height))
{
    if (!video_writer_.isOpened())
    {
        throw std::runtime_error("Failed to open '" + filepath + "'");
    }
}

void VideoWriter::WriteFrame(const cv::Mat& frame)
{
    video_writer_.write(frame);
}

}