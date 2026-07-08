add_executable(mario
        main.cpp
        common/EventTrigger.cpp
)

# 统一头文件路径, 相对src/目录的路径
target_include_directories(mario PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_precompile_headers(mario PRIVATE precompile.h)

target_link_libraries(mario PRIVATE fltk) # 外部库示例

