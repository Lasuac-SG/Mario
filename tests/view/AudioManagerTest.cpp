// AudioManager 单元测试：BGM 播放、停止、音量控制
// 使用编译期宏 AUDIO_TEST_FILE 获取测试音频文件路径。

#include <gtest/gtest.h>

#include "view/audio/AudioManager.h"

// ═══════════════════════════════════════════════════════════════
// 默认状态
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 构造后未播放BGM) {
    AudioManager audio;
    EXPECT_FALSE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// playBGM —— 无效路径
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 无效路径返回false且未播放) {
    AudioManager audio;
    bool result = audio.playBGM("./nonexistent/bgm.ogg");
    EXPECT_FALSE(result);
    EXPECT_FALSE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, 空路径返回false且未播放) {
    AudioManager audio;
    bool result = audio.playBGM("");
    EXPECT_FALSE(result);
    EXPECT_FALSE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// playBGM —— 有效路径
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 有效路径返回true且开始播放) {
    AudioManager audio;
    bool result = audio.playBGM(AUDIO_TEST_FILE);
    EXPECT_TRUE(result);
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, 重复调用playBGM仍然播放) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));
    EXPECT_TRUE(audio.isBGMPlaying());

    // 第二次调用应该也成功（重新加载同一文件）
    bool secondResult = audio.playBGM(AUDIO_TEST_FILE);
    EXPECT_TRUE(secondResult);
    EXPECT_TRUE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// stopBGM
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 停止播放后isBGMPlaying返回false) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));
    ASSERT_TRUE(audio.isBGMPlaying());

    audio.stopBGM();
    EXPECT_FALSE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, 未加载时stopBGM安全无崩溃) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.stopBGM());
    EXPECT_FALSE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// setBGMVolume
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 播放中设置音量安全) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE, 50.f));
    EXPECT_NO_THROW(audio.setBGMVolume(80.f));
    EXPECT_TRUE(audio.isBGMPlaying());  // 调整音量不影响播放状态
}

TEST(AudioManagerTest, 未加载时设置音量安全) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.setBGMVolume(50.f));
}

TEST(AudioManagerTest, 设置边界音量值) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));

    EXPECT_NO_THROW(audio.setBGMVolume(0.f));    // 静音
    EXPECT_TRUE(audio.isBGMPlaying());

    EXPECT_NO_THROW(audio.setBGMVolume(100.f));  // 最大音量
    EXPECT_TRUE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// playBGM —— 自定义音量
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 默认音量启动成功) {
    AudioManager audio;
    bool result = audio.playBGM(AUDIO_TEST_FILE);  // 不传 volume，使用默认值 30.f
    EXPECT_TRUE(result);
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, 自定义音量启动成功) {
    AudioManager audio;
    bool result = audio.playBGM(AUDIO_TEST_FILE, 75.f);
    EXPECT_TRUE(result);
    EXPECT_TRUE(audio.isBGMPlaying());
}
