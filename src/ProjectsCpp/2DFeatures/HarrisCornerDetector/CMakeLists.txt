MACRO(add_example name)
    ADD_EXECUTABLE(${name} ${name}.cpp)
    TARGET_LINK_LIBRARIES(${name} ${OpenCV_LIBS})
ENDMACRO()

# Harris角点检测
add_example(2d_features_harris_corner_detector)