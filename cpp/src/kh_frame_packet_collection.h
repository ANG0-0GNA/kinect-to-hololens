#pragma once

#include "kh_frame_message.h"

namespace kh
{
class FramePacketCollection
{
public:
    FramePacketCollection(int frame_id, int packet_count);
    int frame_id() { return frame_id_; }
    int packet_count() { return packet_count_; }
    void addPacket(int packet_index, std::vector<uint8_t> packet);
    bool isFull();
    FrameMessage toMessage();
    int getCollectedPacketCount();

private:
    int frame_id_;
    int packet_count_;
    std::vector<std::vector<std::uint8_t>> packets_;
};
}