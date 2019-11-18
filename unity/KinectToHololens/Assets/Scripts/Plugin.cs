﻿using System;
using System.Runtime.InteropServices;

// Class with static methods that are bridging to the external functions of KinectToHololensPlugin.dll.
public static class Plugin
{
    private const string DllName = "KinectToHololensPlugin";

    [DllImport(DllName)]
    public static extern bool has_unity_interfaces();

    [DllImport(DllName)]
    public static extern bool has_unity_graphics();

    [DllImport(DllName)]
    public static extern bool has_d3d11_device();

    [DllImport(DllName)]
    public static extern IntPtr get_render_event_function_pointer();

    [DllImport(DllName)]
    public static extern void texture_group_reset();

    [DllImport(DllName)]
    public static extern bool texture_group_is_initialized();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_get_y_texture_view();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_get_u_texture_view();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_get_v_texture_view();

    [DllImport(DllName)]
    public static extern IntPtr texture_group_get_depth_texture_view();

    [DllImport(DllName)]
    public static extern int texture_group_get_color_width();

    [DllImport(DllName)]
    public static extern void texture_group_set_color_width(int color_width);

    [DllImport(DllName)]
    public static extern int texture_group_get_color_height();

    [DllImport(DllName)]
    public static extern void texture_group_set_color_height(int color_height);

    [DllImport(DllName)]
    public static extern int texture_group_get_depth_width();

    [DllImport(DllName)]
    public static extern void texture_group_set_depth_width(int depth_width);

    [DllImport(DllName)]
    public static extern int texture_group_get_depth_height();

    [DllImport(DllName)]
    public static extern void texture_group_set_depth_height(int depth_height);

    [DllImport(DllName)]
    public static extern void texture_group_init_depth_encoder(int depth_compression_type);

    [DllImport(DllName)]
    public static extern void texture_group_set_ffmpeg_frame(IntPtr ffmpeg_frame_ptr);

    //[DllImport(DllName)]
    //public static extern void texture_group_set_rvl_frame(IntPtr rvl_frame_data, int rvl_frame_size);

    //[DllImport(DllName)]
    //public static extern void texture_group_set_depth_encoder_frame(IntPtr depth_encoder_frame_data, int depth_encoder_frame_size);

    [DllImport(DllName)]
    public static extern void texture_group_decode_depth_encoder_frame(IntPtr depth_encoder_frame_data, int depth_encoder_frame_size);

    [DllImport(DllName)]
    public static extern IntPtr create_vp8_decoder();

    [DllImport(DllName)]
    public static extern void delete_vp8_decoder(IntPtr ptr);

    [DllImport(DllName)]
    public static extern IntPtr vp8_decoder_decode(IntPtr decoder_ptr, IntPtr frame_ptr, int frame_size);

    [DllImport(DllName)]
    public static extern void delete_ffmpeg_frame(IntPtr ptr);
}
