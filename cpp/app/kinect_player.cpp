// This file is overall quite obsolete.
// Ideally, the playback functionality should be merged into kinect_sender_demo.cpp.

#include <filesystem>
#include "k4arecord/playback.hpp"
#include "kh_core.h"
#include "kh_sender.h"
#include "kh_vp8.h"
#include "kh_trvl.h"

namespace kh
{
std::vector<std::string> get_filenames_from_folder_path(std::string folder_path)
{
    std::vector<std::string> filenames;
    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
        std::string filename = entry.path().filename().string();
        if (filename == ".gitignore")
            continue;
        if (entry.is_directory())
            continue;
        filenames.push_back(filename);
    }

    return filenames;
}

// Sends Azure Kinect frames through a TCP port.
void play_azure_kinect_frames(std::string path, int port)
{
    const int TARGET_BITRATE = 2000;
    const short CHANGE_THRESHOLD = 10;
    const int INVALID_THRESHOLD = 2;

    std::cout << "Start sending Azure Kinect frames (port: " << port << ")." << std::endl;

    auto playback = k4a::playback::open(path.c_str());
    auto calibration = playback.get_calibration();

    Vp8Encoder vp8_encoder(calibration.color_camera_calibration.resolution_width,
                           calibration.color_camera_calibration.resolution_height,
                           TARGET_BITRATE);

    int depth_frame_width = calibration.depth_camera_calibration.resolution_width;
    int depth_frame_height = calibration.depth_camera_calibration.resolution_height;
    int depth_frame_size = depth_frame_width * depth_frame_height;
    
    TrvlEncoder depth_encoder(depth_frame_size, CHANGE_THRESHOLD, INVALID_THRESHOLD);

    // Creating a tcp socket with the port and waiting for a connection.
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    auto socket = acceptor.accept();

    std::cout << "Accepted a client!" << std::endl;

    // Sender is a class that will use the socket to send frames to the receiver that has the socket connected to this socket.
    Sender sender(std::move(socket));
    // The sender sends the KinectIntrinsics, so the renderer from the receiver side can prepare rendering Kinect frames.
    sender.send(calibration);

    // The amount of frames this sender will send before receiveing a feedback from a receiver.
    const int MAXIMUM_FRAME_ID_DIFF = 2;
    // frame_id is the ID of the frame the sender sends.
    int frame_id = 0;
    // receiver_frame_id is the ID that the receiver sent back saying it received the frame of that ID.
    int receiver_frame_id = 0;

    // Variables for profiling the sender.
    auto start = std::chrono::system_clock::now();
    int frame_count = 0;
    size_t frame_size = 0;
    for (;;) {
        // Try receiving a frame ID from the receiver and update receiver_frame_id if possible.
        auto receive_result = sender.receive();
        if (receive_result) {
            int cursor = 0;

            // Currently, message_type is not getting used.
            auto message_type = (*receive_result)[0];
            cursor += 1;

            if (message_type == 0) {
                memcpy(&receiver_frame_id, receive_result->data() + cursor, 4);
            }
        }

        // If more than MAXIMUM_FRAME_ID_DIFF frames are sent to the receiver without receiver_frame_id getting updated,
        // stop sending more.
        if (frame_id - receiver_frame_id > MAXIMUM_FRAME_ID_DIFF)
            continue;

        k4a::capture capture;
        if (!playback.get_next_capture(&capture)) {
            playback = k4a::playback::open(path.c_str());
            if (!playback.get_next_capture(&capture)) {
                std::cout << "Reset of the playback did not work." << std::endl;
            }
        }

        auto color_image = capture.get_color_image();
        if (!color_image)
            continue;

        auto depth_image = capture.get_depth_image();
        if (!depth_image)
            continue;

        if (color_image.get_format() != K4A_IMAGE_FORMAT_COLOR_YUY2) {
            std::cout << "Only K4A_IMAGE_FORMAT_COLOR_YUY2 is supported as the color format." << std::endl;
            return;
        }

        auto yuv_image = createYuvImageFromAzureKinectYuy2Buffer(color_image.get_buffer(),
                                                                 color_image.get_width_pixels(),
                                                                 color_image.get_height_pixels(),
                                                                 color_image.get_stride_bytes());
        auto vp8_frame = vp8_encoder.encode(yuv_image);

        // Compress the depth pixels.
        auto depth_encoder_frame = depth_encoder.encode(reinterpret_cast<short*>(depth_image.get_buffer()));

        // Print profile measures every 100 frames.
        if (frame_id % 100 == 0) {
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> diff = end - start;
            std::cout << "Sending frame " << frame_id << ", "
                      << "FPS: " << frame_count / diff.count() << ", "
                      << "Bandwidth: " << frame_size / (diff.count() * 131072) << " Mbps.\r"; // 131072 = 1024 * 1024 / 8
            start = end;
            frame_count = 0;
            frame_size = 0;
        }

        // Try sending the frame. Escape the loop if there is a network error.
        // TODO: Send the real timestamp instead 0.0f.
        try {
            sender.send(frame_id++, 0.0f, vp8_frame, reinterpret_cast<uint8_t*>(depth_encoder_frame.data()), depth_encoder_frame.size());
        } catch (std::exception & e) {
            std::cout << e.what() << std::endl;
            break;
        }

        // Updating variables for profiling.
        ++frame_count;
        //frame_size += vp8_frame.size() + rvl_frame.size();
        frame_size += vp8_frame.size() + depth_encoder_frame.size();
    }

    std::cout << "Stopped sending Kinect frames." << std::endl;
}

// Repeats collecting the port number from the user and calling _send_frames() with it.
void send_frames()
{
    // TODO: Fix this to make it work both in Visual Studio and as an exe file.
    const std::string PLAYBACK_FOLDER_PATH = "../../../../playback/";

    for (;;) {
        std::vector<std::string> filenames(get_filenames_from_folder_path(PLAYBACK_FOLDER_PATH));

        std::cout << "Input filenames inside the playback folder:" << std::endl;
        for (int i = 0; i < filenames.size(); ++i) {
            std::cout << "\t(" << i << ") " << filenames[i] << std::endl;
        }

        int filename_index;
        for (;;) {
            std::cout << "Enter filename index: ";
            std::cin >> filename_index;
            if (filename_index >= 0 && filename_index < filenames.size())
                break;

            std::cout << "Invliad index." << std::endl;
        }

        std::string filename = filenames[filename_index];
        std::string file_path = PLAYBACK_FOLDER_PATH + filename;

        std::cout << "Enter a port number to start sending frames: ";
        std::string line;
        std::getline(std::cin, line);
        // The default port (the port when nothing is entered) is 7777.
        int port = line.empty() ? 7777 : std::stoi(line);

        try {
            play_azure_kinect_frames(file_path, port);
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
}

int main()
{
    kh::send_frames();
    return 0;
}