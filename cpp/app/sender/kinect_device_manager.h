#pragma once

#include <iostream>
#include <random>
#include <readerwriterqueue/readerwriterqueue.h>
#include "native/kh_native.h"
#include "video_sender_utils.h"

// These header files are from a Microsoft's Azure Kinect sample project.
#include "PointCloudGenerator.h"
#include "FloorDetector.h"

namespace kh
{

struct VideoDeviceManagerState
{
    int frame_id{0};
    TimePoint last_frame_time_point{TimePoint::now()};
};

struct KinectDeviceManagerSummary
{
    TimePoint start_time{TimePoint::now()};
    float shadow_removal_ms_sum{0.0f};
    float transformation_ms_sum{0.0f};
    float yuv_conversion_ms_sum{0.0f};
    float color_encoder_ms_sum{0.0f};
    float depth_encoder_ms_sum{0.0f};
    int frame_count{0};
    int byte_count{0};
    int keyframe_count{0};
    int frame_id{0};
};

class KinectDeviceManager
{
public:
    // Color encoder also uses the depth width/height since color pixels get transformed to the depth camera.
    KinectDeviceManager(const int session_id, const asio::ip::udp::endpoint remote_endpoint, KinectDevice&& kinect_device);
    void update(const TimePoint& session_start_time,
                UdpSocket& udp_socket,
                VideoParityPacketStorage& video_parity_packet_storage,
                ReceiverState& receiver_state,
                KinectDeviceManagerSummary& summary);
private:
    const int session_id_;
    const asio::ip::udp::endpoint remote_endpoint_;
    std::mt19937 random_number_generator_;
    KinectDevice kinect_device_;
    k4a::calibration calibration_;
    k4a::transformation transformation_;
    Vp8Encoder color_encoder_;
    TrvlEncoder depth_encoder_;
    OcclusionRemover occlusion_remover_;
    Samples::PointCloudGenerator point_cloud_generator_;
    VideoDeviceManagerState state_;
};
}