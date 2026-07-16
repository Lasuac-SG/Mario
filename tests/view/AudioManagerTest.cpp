// AudioManager 单元测试：BGM、SFX、事件回调逻辑
// 使用编译期宏获取测试音频文件路径。

#include <gtest/gtest.h>

#include "view/audio/AudioManager.h"

// ═══════════════════════════════════════════════════════════════
// 测试夹具：预加载一个 SFX，用于验证回调链路
// ═══════════════════════════════════════════════════════════════

class AudioManagerSFXTest : public ::testing::Test {
   protected:
    void SetUp() override {
        ASSERT_TRUE(audio_.loadSFX("COIN_COLLECTED", AUDIO_SFX_TEST_FILE));
    }

    AudioManager audio_;
};

// ═══════════════════════════════════════════════════════════════
// BGM —— 默认状态
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 构造后未播放BGM) {
    AudioManager audio;
    EXPECT_FALSE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// BGM —— invalid paths
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
// BGM —— valid paths
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

    bool secondResult = audio.playBGM(AUDIO_TEST_FILE);
    EXPECT_TRUE(secondResult);
    EXPECT_TRUE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// BGM —— stopBGM
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
// BGM —— setBGMVolume
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 播放中设置音量安全) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE, 50.f));
    EXPECT_NO_THROW(audio.setBGMVolume(80.f));
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, 未加载时设置音量安全) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.setBGMVolume(50.f));
}

TEST(AudioManagerTest, 设置边界音量值) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));

    EXPECT_NO_THROW(audio.setBGMVolume(0.f));
    EXPECT_TRUE(audio.isBGMPlaying());

    EXPECT_NO_THROW(audio.setBGMVolume(100.f));
    EXPECT_TRUE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// BGM —— 自定义音量
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 默认音量启动成功) {
    AudioManager audio;
    bool result = audio.playBGM(AUDIO_TEST_FILE);
    EXPECT_TRUE(result);
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, 自定义音量启动成功) {
    AudioManager audio;
    bool result = audio.playBGM(AUDIO_TEST_FILE, 75.f);
    EXPECT_TRUE(result);
    EXPECT_TRUE(audio.isBGMPlaying());
}

// ═══════════════════════════════════════════════════════════════
// SFX —— loadSFX
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 加载无效SFX路径返回false) {
    AudioManager audio;
    EXPECT_FALSE(audio.loadSFX("test", "./nonexistent/sfx.ogg"));
}

TEST(AudioManagerTest, 加载空路径返回false) {
    AudioManager audio;
    EXPECT_FALSE(audio.loadSFX("test", ""));
}

TEST(AudioManagerTest, 同名重复加载后者覆盖前者) {
    AudioManager audio;
    EXPECT_TRUE(audio.loadSFX("sfx", AUDIO_SFX_TEST_FILE));
    // 同名再次加载同一文件应当成功
    EXPECT_TRUE(audio.loadSFX("sfx", AUDIO_SFX_TEST_FILE));
}

// ═══════════════════════════════════════════════════════════════
// SFX —— loadAllSFX
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, loadAllSFX无崩溃) {
    AudioManager audio;
    // 测试环境中 audio/sound/ 路径可能不存在，部分文件加载失败不影响方法本身
    EXPECT_NO_THROW(audio.loadAllSFX());
}

// ═══════════════════════════════════════════════════════════════
// SFX —— playSFX
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 播放未加载名称安全无崩溃) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.playSFX("nonexistent"));
}

TEST(AudioManagerTest, 播放空名称安全无崩溃) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.playSFX(""));
}

TEST_F(AudioManagerSFXTest, 播放已加载SFX无崩溃) {
    EXPECT_NO_THROW(audio_.playSFX("COIN_COLLECTED"));
}

TEST_F(AudioManagerSFXTest, 多次播放同一SFX无崩溃) {
    for (int i = 0; i < 10; ++i) {
        EXPECT_NO_THROW(audio_.playSFX("COIN_COLLECTED"));
    }
}

// ═══════════════════════════════════════════════════════════════
// SFX —— setSFXVolume / stopAllSFX
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 未加载时设置SFX音量安全) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.setSFXVolume(50.f));
    EXPECT_NO_THROW(audio.setSFXVolume(0.f));
    EXPECT_NO_THROW(audio.setSFXVolume(100.f));
}

TEST(AudioManagerTest, 未加载时stopAllSFX安全) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.stopAllSFX());
}

TEST_F(AudioManagerSFXTest, 播放后stopAllSFX安全) {
    audio_.playSFX("COIN_COLLECTED");
    EXPECT_NO_THROW(audio_.stopAllSFX());
}

// ═══════════════════════════════════════════════════════════════
// 事件回调 —— getNotification / onEvent
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, getNotification返回非空可调用对象) {
    AudioManager audio;
    auto fn = audio.getNotification();
    EXPECT_TRUE(static_cast<bool>(fn));
}

TEST(AudioManagerTest, 未加载SFX时回调已映射事件无崩溃) {
    AudioManager audio;
    auto fn = audio.getNotification();

    // 所有 eventSFXMap 中映射的事件，即使 SFX 未加载也不应崩溃
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::MARIO_JUMPED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::ENEMY_STOMPED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::MARIO_DIED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::GOAL_REACHED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::BLOCK_BUMPED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::BRICK_BROKEN)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::MUSHROOM_COLLECTED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::MARIO_GROWN)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::MARIO_SHRUNK)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::MARIO_HURT)));
}

TEST(AudioManagerTest, 回调未映射事件无崩溃) {
    AudioManager audio;
    auto fn = audio.getNotification();

    // STATE_CHANGED / LEVEL_RESET / LEVEL_LOADED 不在 eventSFXMap 中
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::STATE_CHANGED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::LEVEL_RESET)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::LEVEL_LOADED)));

    // 任意非 Event 值的 uint32_t 也不应崩溃
    EXPECT_NO_THROW(fn(static_cast<EventType>(999)));
}

TEST_F(AudioManagerSFXTest, 已加载SFX时回调触发播放无崩溃) {
    auto fn = audio_.getNotification();

    // COIN_COLLECTED 映射到已加载的 "COIN_COLLECTED" SFX，播放不应崩溃
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
}

TEST_F(AudioManagerSFXTest, 连续回调触发多次播放无崩溃) {
    auto fn = audio_.getNotification();

    // 模拟密集事件
    for (int i = 0; i < 5; ++i) {
        EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
    }
}

TEST_F(AudioManagerSFXTest, 多事件混合回调无崩溃) {
    auto fn = audio_.getNotification();

    // COIN_COLLECTED 在 map 中（已加载 SFX）→ 播放
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
    // STATE_CHANGED 不在 map 中 → 跳过
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::STATE_CHANGED)));
    // 再次触发 COIN_COLLECTED
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
    // 未映射事件
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::LEVEL_RESET)));
}

// ═══════════════════════════════════════════════════════════════
// 回调与 BGM 独立共存
// ═══════════════════════════════════════════════════════════════

TEST(AudioManagerTest, 回调不影响BGM状态) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));
    ASSERT_TRUE(audio.isBGMPlaying());

    auto fn = audio.getNotification();
    fn(static_cast<EventType>(Event::COIN_COLLECTED));  // 无效事件但不应影响 BGM

    EXPECT_TRUE(audio.isBGMPlaying());
}
