MACRO(add_example name)
    ADD_EXECUTABLE(${name} ${name}.cpp)
    TARGET_LINK_LIBRARIES(${name} ${OpenCV_LIBS})
ENDMACRO()

add_example(2d_features_homography_pose_from_homography)

add_example(2d_features_homography_perspective_correction)

add_example(2d_features_homography_from_camera_displacement)