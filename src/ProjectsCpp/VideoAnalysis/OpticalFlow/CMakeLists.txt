macro(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ${OpenCV_LIBS})
endmacro()

# 稀疏光流
add_example(VA_optical_flow)

# 稠密光流
add_example(VA_optical_flow_dense)