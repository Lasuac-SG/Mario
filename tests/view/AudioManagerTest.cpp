#include <gtest/gtest.h>

#include "view/audio/AudioManager.h"

class AudioManagerSFXTest : public ::testing::Test {
   protected:
    void SetUp() override {
        ASSERT_TRUE(audio_.loadSFX("COIN_COLLECTED", AUDIO_SFX_TEST_FILE));
    }

    AudioManager audio_;
};

TEST(AudioManagerTest, StartsWithNoBGMPlaying) {
    AudioManager audio;
    EXPECT_FALSE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, InvalidBGMPathReturnsFalseAndDoesNotPlay) {
    AudioManager audio;
    const bool result = audio.playBGM("./nonexistent/bgm.ogg");
    EXPECT_FALSE(result);
    EXPECT_FALSE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, EmptyBGMPathReturnsFalseAndDoesNotPlay) {
    AudioManager audio;
    const bool result = audio.playBGM("");
    EXPECT_FALSE(result);
    EXPECT_FALSE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, ValidBGMPathStartsPlayback) {
    AudioManager audio;
    const bool result = audio.playBGM(AUDIO_TEST_FILE);
    EXPECT_TRUE(result);
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, RepeatedPlayBGMKeepsPlaybackActive) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));
    EXPECT_TRUE(audio.isBGMPlaying());

    const bool secondResult = audio.playBGM(AUDIO_TEST_FILE);
    EXPECT_TRUE(secondResult);
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, StopBGMStopsPlayback) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));
    ASSERT_TRUE(audio.isBGMPlaying());

    audio.stopBGM();
    EXPECT_FALSE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, StopBGMIsSafeWhenNothingIsPlaying) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.stopBGM());
    EXPECT_FALSE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, SetBGMVolumeIsSafeDuringPlayback) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE, 50.f));
    EXPECT_NO_THROW(audio.setBGMVolume(80.f));
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, SetBGMVolumeIsSafeBeforePlayback) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.setBGMVolume(50.f));
}

TEST(AudioManagerTest, SetBGMVolumeAcceptsBoundaryValues) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));

    EXPECT_NO_THROW(audio.setBGMVolume(0.f));
    EXPECT_TRUE(audio.isBGMPlaying());

    EXPECT_NO_THROW(audio.setBGMVolume(100.f));
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, DefaultBGMVolumeStillStartsPlayback) {
    AudioManager audio;
    const bool result = audio.playBGM(AUDIO_TEST_FILE);
    EXPECT_TRUE(result);
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, CustomBGMVolumeStillStartsPlayback) {
    AudioManager audio;
    const bool result = audio.playBGM(AUDIO_TEST_FILE, 75.f);
    EXPECT_TRUE(result);
    EXPECT_TRUE(audio.isBGMPlaying());
}

TEST(AudioManagerTest, LoadSFXRejectsInvalidPath) {
    AudioManager audio;
    EXPECT_FALSE(audio.loadSFX("test", "./nonexistent/sfx.ogg"));
}

TEST(AudioManagerTest, LoadSFXRejectsEmptyPath) {
    AudioManager audio;
    EXPECT_FALSE(audio.loadSFX("test", ""));
}

TEST(AudioManagerTest, LoadSFXAllowsReplacingTheSameName) {
    AudioManager audio;
    EXPECT_TRUE(audio.loadSFX("sfx", AUDIO_SFX_TEST_FILE));
    EXPECT_TRUE(audio.loadSFX("sfx", AUDIO_SFX_TEST_FILE));
}

TEST(AudioManagerTest, LoadAllSFXDoesNotThrow) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.loadAllSFX());
}

TEST(AudioManagerTest, PlaySFXIgnoresUnknownName) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.playSFX("nonexistent"));
}

TEST(AudioManagerTest, PlaySFXIgnoresEmptyName) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.playSFX(""));
}

TEST_F(AudioManagerSFXTest, PlayLoadedSFXDoesNotThrow) {
    EXPECT_NO_THROW(audio_.playSFX("COIN_COLLECTED"));
}

TEST_F(AudioManagerSFXTest, RepeatedSFXPlaybackDoesNotThrow) {
    for (int i = 0; i < 10; ++i) {
        EXPECT_NO_THROW(audio_.playSFX("COIN_COLLECTED"));
    }
}

TEST(AudioManagerTest, SetSFXVolumeIsSafeBeforeLoad) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.setSFXVolume(50.f));
    EXPECT_NO_THROW(audio.setSFXVolume(0.f));
    EXPECT_NO_THROW(audio.setSFXVolume(100.f));
}

TEST(AudioManagerTest, StopAllSFXIsSafeBeforeLoad) {
    AudioManager audio;
    EXPECT_NO_THROW(audio.stopAllSFX());
}

TEST_F(AudioManagerSFXTest, StopAllSFXIsSafeAfterPlayback) {
    audio_.playSFX("COIN_COLLECTED");
    EXPECT_NO_THROW(audio_.stopAllSFX());
}

TEST(AudioManagerTest, GetNotificationReturnsCallable) {
    AudioManager audio;
    const auto fn = audio.getNotification();
    EXPECT_TRUE(static_cast<bool>(fn));
}

TEST(AudioManagerTest, NotificationIsSafeForMappedEventsWithoutLoadedSFX) {
    AudioManager audio;
    const auto fn = audio.getNotification();

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

TEST(AudioManagerTest, NotificationIsSafeForUnmappedEvents) {
    AudioManager audio;
    const auto fn = audio.getNotification();

    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::STATE_CHANGED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::LEVEL_RESET)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::LEVEL_LOADED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(999)));
}

TEST_F(AudioManagerSFXTest, NotificationPlaysLoadedMappedSFXWithoutThrowing) {
    const auto fn = audio_.getNotification();
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
}

TEST_F(AudioManagerSFXTest, NotificationHandlesRepeatedMappedEvents) {
    const auto fn = audio_.getNotification();

    for (int i = 0; i < 5; ++i) {
        EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
    }
}

TEST_F(AudioManagerSFXTest, NotificationHandlesMixedMappedAndUnmappedEvents) {
    const auto fn = audio_.getNotification();

    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::STATE_CHANGED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::COIN_COLLECTED)));
    EXPECT_NO_THROW(fn(static_cast<EventType>(Event::LEVEL_RESET)));
}

TEST(AudioManagerTest, NotificationDoesNotInterruptBGMPlayback) {
    AudioManager audio;
    ASSERT_TRUE(audio.playBGM(AUDIO_TEST_FILE));
    ASSERT_TRUE(audio.isBGMPlaying());

    const auto fn = audio.getNotification();
    fn(static_cast<EventType>(Event::COIN_COLLECTED));

    EXPECT_TRUE(audio.isBGMPlaying());
}
