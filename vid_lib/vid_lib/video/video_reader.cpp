#include <vid_lib/video/video_reader.h>

#include <iostream>

namespace vid_lib::video
{

VideoReader::VideoReader(const std::string& filepath)
    : video_capture_(filepath)
{
    if (!video_capture_.isOpened())
    {
        throw std::runtime_error("Failed to open '" + filepath + "'");
    }

    WriteDetailsTo(std::cout);
}

void VideoReader::ReadFrame(cv::Mat& image)
{
    video_capture_.read(image);
}

void VideoReader::WriteDetailsTo(std::ostream& stream) const
{
    std::cout
        << "resolution : " << GetFrameWidth() << 'x' << GetFrameHeight() << std::endl
        << "frame count: " << GetFrameCount() << std::endl
        << "frame per s: " << GetFramesPerSecond() << std::endl
        << "auto orient: " << video_capture_.get(cv::CAP_PROP_ORIENTATION_AUTO) << std::endl;
}

int VideoReader::GetFrameWidth() const
{
    return static_cast<int>(video_capture_.get(cv::CAP_PROP_FRAME_WIDTH));
}

int VideoReader::GetFrameHeight() const
{
    return static_cast<int>(video_capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
}

int VideoReader::GetFrameCount() const
{
    return static_cast<int>(video_capture_.get(cv::CAP_PROP_FRAME_COUNT));
}

double VideoReader::GetFramesPerSecond() const
{
    return video_capture_.get(cv::CAP_PROP_FPS);
}

}