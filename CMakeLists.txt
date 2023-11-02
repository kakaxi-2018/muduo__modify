cmake_minimum_required(VERSION 3.5)
project(muduo_modify_version)

# mymuduo 最终编译成 so 动态库，设置动态库的路径，放在根目录的 lib 文件夹下面
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 设置调试信息，以及启动 C++11 语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")

# 定义参与编译的源代码文件
aux_source_directory(. SRC_LIST)
# 编译生成动态库 mymuduo
add_library(muduo_modify_version SHARED ${SRC_LIST})