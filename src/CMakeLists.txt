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
        view/ViewInteraction.cpp
        app/App.cpp
)

add_executable(input_test_window
        tools/InputIntegrationWindow.cpp
        view/input/InputHandler.cpp
        view/ViewInteraction.cpp
)

add_executable(mario_autoplay_demo
        tools/AutoPlayDemo.cpp
        common/EventTrigger.cpp
        model/TileMap.cpp
        model/Mario.cpp
        model/GameModel.cpp
        viewmodel/ViewModel.cpp
        viewmodel/command/Commands.cpp
        view/View.cpp
        view/ViewInteraction.cpp
        view/audio/AudioManager.cpp
        view/input/InputHandler.cpp
        view/renderer/GameRenderer.cpp
)

# 统一头文件路径, 相对src/目录的路径
target_include_directories(mario PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(input_test_window PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(mario_autoplay_demo PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_precompile_headers(mario PRIVATE precompile.h)
target_precompile_headers(input_test_window PRIVATE precompile.h)
target_precompile_headers(mario_autoplay_demo PRIVATE precompile.h)

target_link_libraries(mario PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)
target_link_libraries(input_test_window PRIVATE SFML::Graphics SFML::Window SFML::System)
target_link_libraries(mario_autoplay_demo PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

add_custom_command(
        TARGET mario POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:mario>/assets
        COMMENT "Copying assets folder to app directory"
)

add_custom_command(
        TARGET mario_autoplay_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:mario_autoplay_demo>/assets
        COMMENT "Copying assets folder to app directory"
)
