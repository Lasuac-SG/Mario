add_executable(mario
        main.cpp
        common/EventTrigger.cpp
        model/TileMap.cpp
        model/Mario.cpp
        model/GameModel.cpp
        viewmodel/ViewModel.cpp
        viewmodel/command/Commands.cpp
        view/View.cpp
        view/audio/AudioManager.cpp
        view/input/InputHandler.cpp
        view/renderer/GameRenderer.cpp
        app/App.cpp
)

# 统一头文件路径, 相对src/目录的路径
target_include_directories(mario PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_precompile_headers(mario PRIVATE precompile.h)

target_link_libraries(mario PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

add_custom_command(
        TARGET mario POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/picture
        $<TARGET_FILE_DIR:mario>/picture
        COMMENT "Copying picture folder to app directory"
)

add_custom_command(
        TARGET mario POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/audio
        $<TARGET_FILE_DIR:mario>/audio
        COMMENT "Copying audio folder to app directory"
)