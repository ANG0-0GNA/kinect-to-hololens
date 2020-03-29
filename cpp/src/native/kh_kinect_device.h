#pragma once

#include <optional>
#include <k4a/k4a.hpp>

namespace kh
{
class KinectFrame
{
public:
    KinectFrame(k4a::image&& color_image, k4a::image&& depth_image, k4a_imu_sample_t imu_sample);
    k4a::image& color_image() { return color_image_; }
    k4a::image& depth_image() { return depth_image_; }
    k4a_imu_sample_t& imu_sample() { return imu_sample_; }

private:
    k4a::image color_image_;
    k4a::image depth_image_;
    k4a_imu_sample_t imu_sample_;
};

// For having an interface combining devices and playbacks one day in the future...
class KinectDevice
{
public:
    KinectDevice(k4a_device_configuration_t configuration, std::chrono::milliseconds timeout);
    KinectDevice();
    void start();
    k4a::calibration getCalibration();
    std::optional<KinectFrame> getFrame();

private:
    k4a::device device_;
    k4a_device_configuration_t configuration_;
    std::chrono::milliseconds timeout_;
};
}