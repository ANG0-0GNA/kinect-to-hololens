add_executable(KinectReaderApp
  kinect_reader_app.cpp
  helper/opencv_helper.h
  helper/filesystem_helper.h
)
target_include_directories(KinectReaderApp PRIVATE
  "${AZURE_KINECT_DIR}/sdk/include"
)
target_link_libraries(KinectReaderApp
  KinectToHololensNative
  ${OpenCV_LIBS}
  ${Libvpx_LIB}
)
set_target_properties(KinectReaderApp PROPERTIES
  CXX_STANDARD 17
)
add_custom_command(TARGET KinectReaderApp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AZURE_KINECT_BIN}/depthengine_2_0.dll"
  $<TARGET_FILE_DIR:KinectReaderApp>
)
add_custom_command(TARGET KinectReaderApp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AZURE_KINECT_BIN}/k4a.dll"
  $<TARGET_FILE_DIR:KinectReaderApp>
)
add_custom_command(TARGET KinectReaderApp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AZURE_KINECT_BIN}/k4arecord.dll"
  $<TARGET_FILE_DIR:KinectReaderApp>
)

add_executable(KinectSenderApp
  kinect_sender_app.cpp
  helper/soundio_helper.h
  sender/kinect_audio_sender.h
  sender/kinect_video_sender.h
  sender/kinect_video_sender.cpp
  sender/receiver_packet_receiver.h
  sender/video_sender_utils.h
  sender/remote_receiver.h
  helper/imgui_helper.h
  resources/KinectSenderApp.rc
)
target_include_directories(KinectSenderApp PRIVATE
  "${AZURE_KINECT_DIR}/sdk/include"
  ${AZURE_KINECT_SAMPLES_DIR}
  ${IMGUI_EXAMPLES_DIR}
)
target_link_libraries(KinectSenderApp
  KinectToHololensNative
  ${Libvpx_LIB}
  AzureKinectSamples
  ImguiExamples
  imgui::imgui
)
set_target_properties(KinectSenderApp PROPERTIES
  CXX_STANDARD 17
)
add_custom_command(TARGET KinectSenderApp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AZURE_KINECT_BIN}/depthengine_2_0.dll"
  $<TARGET_FILE_DIR:KinectSenderApp>
)
add_custom_command(TARGET KinectSenderApp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AZURE_KINECT_BIN}/k4a.dll"
  $<TARGET_FILE_DIR:KinectSenderApp>
)
add_custom_command(TARGET KinectSenderApp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${AZURE_KINECT_BIN}/k4arecord.dll"
  $<TARGET_FILE_DIR:KinectSenderApp>
)

add_executable(KinectReceiverApp
  kinect_receiver_app.cpp
  helper/opencv_helper.h
  helper/soundio_helper.h
  receiver/audio_packet_receiver.h
  receiver/sender_packet_receiver.h
  receiver/video_message_assembler.h
  receiver/video_message_assembler.cpp
  receiver/video_renderer.h
  receiver/video_renderer_state.h
)
target_include_directories(KinectReceiverApp PRIVATE
  "${AZURE_KINECT_DIR}/sdk/include"
)
target_link_libraries(KinectReceiverApp
  ${OpenCV_LIBS}
  KinectToHololensNative
)
set_target_properties(KinectReceiverApp PROPERTIES
  CXX_STANDARD 17
)

add_executable(KinectListenerApp
  kinect_listener_app.cpp
  helper/soundio_helper.h
)
target_include_directories(KinectListenerApp PRIVATE
  "${AZURE_KINECT_DIR}/sdk/include"
)
target_link_libraries(KinectListenerApp
  KinectToHololensNative
)
set_target_properties(KinectListenerApp PROPERTIES
  CXX_STANDARD 17
)