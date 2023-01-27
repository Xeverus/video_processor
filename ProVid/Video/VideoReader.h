#pragma once

#include <ostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

namespace provid::video
{

class VideoReader
{
public:
    explicit VideoReader(const std::string& filepath);

    int GetFrameWidth() const;
    int GetFrameHeight() const;
    int GetFrameCount() const;
    int GetFramesPerSecond() const;

    const cv::Mat& GetNextFrame();

    void WriteDetailsTo(std::ostream& stream) const;

private:
    cv::VideoCapture video_capture_;
    cv::Mat image_;
};

}