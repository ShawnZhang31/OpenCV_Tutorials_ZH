CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
# 定义工程
project(OfficeTutorials)

# 配置OpenCV库
set(OpenCV_DIR ~/Works/libs/opencv-release/cv-412/install/lib/cmake/opencv4/)
find_package(OpenCV REQUIRED)

# 定义C++的版本
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#add_definitions(-std=c++11)
# 当前版本的CPP支持的特性
#foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
#    message("${i}")
#endforeach()

# 包含OpenCV的头文件
include_directories(${OpenCV_INCLUDE_DIRS})

# python
find_package(Python3 COMPONENTS Development NumPy)
include_directories(${Python3_INCLUDE_DIRS})
include_directories(${Python3_NumPy_INCLUDE_DIRS})
##message(${Python3_INCLUDE_DIRS})
##message(${Python3_NumPy_INCLUDE_DIRS})
#include_directories(/Library/Frameworks/Python.framework/Versions/3.7/include/python3.7m)
#include_directories(/Library/Frameworks/Python.framework/Versions/3.7/lib/python3.7/site-packages/numpy/core/include)
#link_libraries(/Library/Frameworks/Python.framework/Versions/3.7/lib python3.7)
#message(${Python3_LIBRARIES})
# 核心模块的内容
add_subdirectory(CoreFunctions)
# photo模块
add_subdirectory(PhotoModule)
# 2D特征模块
add_subdirectory(2DFeatures)