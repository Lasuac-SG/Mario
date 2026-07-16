find_package(GTest CONFIG REQUIRED)

enable_testing()

# Common include path —— 与主项目一致，使用 src/ 相对路径
set(TEST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)

# ── common 层测试 ──────────────────────────────────────────────
add_executable(test_common
    common/EventTriggerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/common/EventTrigger.cpp
)
target_precompile_headers(test_common PRIVATE ${CMAKE_SOURCE_DIR}/src/precompile.h)
target_include_directories(test_common PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_common PRIVATE SFML::Graphics SFML::Window SFML::System)
target_link_libraries(test_common PRIVATE GTest::gtest_main)

# ── model 层测试 ───────────────────────────────────────────────
add_executable(test_model
    model/TileMapTest.cpp
    model/MarioTest.cpp
    model/EnemyTest.cpp
    model/GameModelTest.cpp
    ${CMAKE_SOURCE_DIR}/src/model/TileMap.cpp
    ${CMAKE_SOURCE_DIR}/src/model/Mario.cpp
    ${CMAKE_SOURCE_DIR}/src/model/GameModel.cpp
    ${CMAKE_SOURCE_DIR}/src/common/EventTrigger.cpp
)
target_precompile_headers(test_model PRIVATE ${CMAKE_SOURCE_DIR}/src/precompile.h)
target_include_directories(test_model PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(test_model PRIVATE SFML::Graphics SFML::Window SFML::System)
target_link_libraries(test_model PRIVATE GTest::gtest_main)

# ── view 层测试 ─────────────────────────────────────────────────
add_executable(test_audio
    view/AudioManagerTest.cpp
    ${CMAKE_SOURCE_DIR}/src/view/audio/AudioManager.cpp
)
target_precompile_headers(test_audio PRIVATE ${CMAKE_SOURCE_DIR}/src/precompile.h)
target_include_directories(test_audio PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/tests)
target_compile_definitions(test_audio PRIVATE
    AUDIO_TEST_FILE="${CMAKE_SOURCE_DIR}/audio/bgm.ogg"
    AUDIO_SFX_TEST_FILE="${CMAKE_SOURCE_DIR}/audio/sound/COIN_COLLECTED.ogg"
)
target_link_libraries(test_audio PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)
target_link_libraries(test_audio PRIVATE GTest::gtest_main)

# ── CTest 注册 ─────────────────────────────────────────────────
include(GoogleTest)
# 在 ctest 运行时(而非构建时)枚举用例：避免 Windows 下构建阶段运行测试 exe
# 因找不到 SFML/GTest 的 DLL 而失败(0xc0000139)。
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
gtest_discover_tests(test_common)
gtest_discover_tests(test_model)
gtest_discover_tests(test_audio)
