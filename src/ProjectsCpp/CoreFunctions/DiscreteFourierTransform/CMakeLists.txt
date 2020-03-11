MACRO(add_example name)
    ADD_EXECUTABLE(${name} ${name}.cpp)
    TARGET_LINK_LIBRARIES(${name} ${OpenCV_LIBS})
ENDMACRO()

# 傅里叶变换
add_example(discrete_fourier_transform)