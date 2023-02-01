#include <vid_lib/video/async_video_writer.h>

namespace vid_lib::video
{

AsyncVideoWriter::AsyncVideoWriter(std::unique_ptr<VideoWriter> writer)
    : writer_(std::move(writer))
{
}

AsyncVideoWriter::~AsyncVideoWriter()
{
    Wait();
}

void AsyncVideoWriter::Wait()
{
    if (future_.valid())
    {
        future_.wait();
    }
}

void AsyncVideoWriter::WriteFrame(const cv::Mat& image)
{
    Wait();
    future_ = std::async(std::launch::async, [this, image]()
    {
        writer_->WriteFrame(image);
    });
}

}