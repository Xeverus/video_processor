#include <vid_lib/video/async_video_reader.h>

namespace vid_lib::video
{

AsyncVideoReader::AsyncVideoReader(std::unique_ptr<VideoReader> reader)
    : reader_(std::move(reader))
{
}

AsyncVideoReader::~AsyncVideoReader()
{
    Wait();
}

void AsyncVideoReader::Wait()
{
    if (future_.valid())
    {
        future_.wait();
    }
}

void AsyncVideoReader::ReadFrame(cv::Mat& image)
{
    Wait();
    future_ = std::async(std::launch::async, [this, &image]()
    {
        reader_->ReadFrame(image);
    });
}

}