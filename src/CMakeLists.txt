add_executable(mario
        main.cpp
        app/App.cpp
        common/EventTrigger.cpp
        model/GameModel.cpp
        viewmodel/ViewModel.cpp
        view/View.cpp
        view/SimpleRenderer.cpp
)

# 统一头文件路径, 相对src/目录的路径
target_include_directories(mario PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_precompile_headers(mario PRIVATE precompile.h)

target_link_libraries(mario PRIVATE SFML::Graphics SFML::Window SFML::System)