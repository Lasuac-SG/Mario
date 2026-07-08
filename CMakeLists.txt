cmake_minimum_required(VERSION 4.2)

project(Mario)

find_package(FLTK CONFIG REQUIRED) # 引入外部库示例

add_subdirectory(src) # 构建在 bin 文件夹中  TODO 路径待确定
