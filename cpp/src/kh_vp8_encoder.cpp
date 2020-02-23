#include "kh_vp8.h"

#include <iostream>

namespace kh
{
// The keyframe_interval_ was chosen arbitrarily.
Vp8Encoder::Vp8Encoder(int width, int height, int target_bitrate)
    : codec_{}, image_{}, frame_index_{0}
{
    vpx_codec_iface_t* (*const codec_interface)() = &vpx_codec_vp8_cx;
    vpx_codec_enc_cfg_t configuration;

    vpx_codec_err_t res = vpx_codec_enc_config_default(codec_interface(), &configuration, 0);
    if (res != VPX_CODEC_OK)
        throw std::exception("Error from vpx_codec_enc_config_default.");

    // From https://developers.google.com/media/vp9/live-encoding
    // See also https://www.webmproject.org/docs/encoder-parameters/
    configuration.g_w = width;
    configuration.g_h = height;
    configuration.rc_target_bitrate = target_bitrate;

    //configuration.g_threads = 8;
    configuration.g_threads = 4;
    configuration.g_lag_in_frames = 0;
    configuration.rc_min_quantizer = 4;
    configuration.rc_max_quantizer = 48;
    configuration.g_error_resilient = 1;

    //configuration.rc_end_usage = VPX_CBR;

    vpx_codec_ctx_t codec;
    res = vpx_codec_enc_init(&codec, codec_interface(), &configuration, 0);
    if (res != VPX_CODEC_OK)
        throw std::exception("Error from vpx_codec_enc_init.");

    vpx_codec_control(&codec, VP8E_SET_CPUUSED, 6);
    vpx_codec_control(&codec, VP8E_SET_STATIC_THRESHOLD, 0);
    vpx_codec_control(&codec, VP8E_SET_MAX_INTRA_BITRATE_PCT, 300);

    vpx_image_t image;
    if (!vpx_img_alloc(&image, VPX_IMG_FMT_I420, configuration.g_w, configuration.g_h, 32))
        throw std::exception("Error from vpx_img_alloc.");

    codec_ = codec;
    image_ = image;
}

Vp8Encoder::~Vp8Encoder()
{
    vpx_img_free(&image_);
    if (vpx_codec_destroy(&codec_))
        std::cout << "Error from vpx_codec_destroy." << std::endl;
}

// Encoding YuvImage with the color pixels with libvpx.
std::vector<uint8_t> Vp8Encoder::encode(YuvImage& yuv_image, bool keyframe)
{
    image_.planes[VPX_PLANE_Y] = yuv_image.y_channel().data();
    image_.planes[VPX_PLANE_U] = yuv_image.u_channel().data();
    image_.planes[VPX_PLANE_V] = yuv_image.v_channel().data();

    image_.stride[VPX_PLANE_Y] = yuv_image.width();
    image_.stride[VPX_PLANE_U] = yuv_image.width() / 2;
    image_.stride[VPX_PLANE_V] = yuv_image.width() / 2;

    const int flags{keyframe ? VPX_EFLAG_FORCE_KF : 0};
    const vpx_codec_err_t res{vpx_codec_encode(&codec_, &image_, frame_index_++, 1, flags, VPX_DL_REALTIME)};

    if (res != VPX_CODEC_OK) {
        std::cout << "vpx_codec_error: " << vpx_codec_error(&codec_) << std::endl;
        std::cout << "vpx_codec_error_detail: " << vpx_codec_error_detail(&codec_) << std::endl;
        throw std::exception("Error from vpx_codec_encode.");
    }

    vpx_codec_iter_t iter{nullptr};
    const vpx_codec_cx_pkt_t* pkt{nullptr};
    std::vector<uint8_t> bytes;
    while (pkt = vpx_codec_get_cx_data(&codec_, &iter)) {
        if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
            if (!bytes.empty())
                throw std::exception("Multiple frames found from a packet.");

            const int keyframe{(pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0};
            bytes.resize(pkt->data.frame.sz);
            memcpy(bytes.data(), (uint8_t*)pkt->data.frame.buf, pkt->data.frame.sz);
        }
    }

    return bytes;
}
}