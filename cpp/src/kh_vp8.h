#pragma once

#include <vpx/vp8cx.h>
#include <vpx/vpx_codec.h>
#include "kh_core.h"

namespace kh
{
// A wrapper class for libvpx, encoding color pixels into the VP8 codec.
class Vp8Encoder
{
public:
    Vp8Encoder(int width, int height, int target_bitrate);
    ~Vp8Encoder();
    std::vector<uint8_t> encode(YuvImage& yuv_image, bool keyframe);

private:
    vpx_codec_ctx_t codec_;
    vpx_image_t image_;
    int frame_index_;
};

// A wrapper class for FFMpeg, decoding colors pixels in the VP8 codec.
class Vp8Decoder
{
public:
    Vp8Decoder();
    ~Vp8Decoder();
    FFmpegFrame decode(uint8_t* vp8_frame_data, size_t vp8_frame_size);

private:
    AVPacket* packet_;
    AVCodec* codec_;
    AVCodecParserContext* codec_parser_context_;
    AVCodecContext* codec_context_;
};
}