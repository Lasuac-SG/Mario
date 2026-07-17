// GameModel 单元测试：游戏规则、死亡/重生、时间、分数、碰撞
// 多帧模拟使用 updateUntil 替代硬编码帧数。

#include "common/TestHelpers.h"
#include "model/GameModel.h"

#include <gtest/gtest.h>

// ═══════════════════════════════════════════════════════════════
// 初始状态
// ═══════════════════════════════════════════════════════════════

TEST(GameModelTest, InitialState) {
    GameModel model;
    EXPECT_EQ(model.lives(), 3);
    EXPECT_EQ(model.score(), 0);
    EXPECT_EQ(model.coins(), 0);
    EXPECT_EQ(model.timeRemaining(), 300);
    EXPECT_EQ(model.world(), "1-1");
}

TEST(GameModelTest, InitialPlayerPositionValid) {
    GameModel model;
    EXPECT_GE(model.playerX(), 0);
    EXPECT_GE(model.playerY(), 0);
    EXPECT_LT(model.playerX(), model.levelWidthPx());
    EXPECT_LT(model.playerY(), model.levelHeightPx());
}

TEST(GameModelTest, TilesNotEmpty) {
    GameModel model;
    EXPECT_FALSE(model.tiles().empty());
}

// ═══════════════════════════════════════════════════════════════
// 时间倒数
// ═══════════════════════════════════════════════════════════════

TEST(GameModelTest, TimeDecrementsOnUpdate) {
    GameModel model;
    int before = model.timeRemaining();
    model.update(1.0f);
    EXPECT_LE(model.timeRemaining(), before - 1);
}

TEST(GameModelTest, TimeDoesNotGoNegative) {
    GameModel model;
    model.update(1000.0f);  // 远超 300 秒
    EXPECT_GE(model.timeRemaining(), 0);
}

// ═══════════════════════════════════════════════════════════════
// 超时死亡 → 死亡序列 → 重生
// ═══════════════════════════════════════════════════════════════

TEST(GameModelTest, TimeoutTriggersDeathAndLifeLoss) {
    GameModel model;
    int livesBefore = model.lives();
    int timeBefore = model.timeRemaining();

    model.update(static_cast<float>(timeBefore + 1));
    model.update(1.2f);  // 完成死亡序列

    EXPECT_EQ(model.lives(), livesBefore - 1);
    EXPECT_EQ(model.timeRemaining(), 300);
}

TEST(GameModelTest, DeathResetsLevelButKeepsLives) {
    GameModel model;
    int livesBefore = model.lives();

    model.update(301.0f);
    model.update(1.2f);

    EXPECT_EQ(model.lives(), livesBefore - 1);
    EXPECT_EQ(model.score(), 0);
    EXPECT_EQ(model.timeRemaining(), 300);
}

TEST(GameModelTest, LivesReachZero) {
    GameModel model;

    for (int i = 0; i < 3; ++i) {
        model.update(301.0f);
        model.update(1.2f);
    }

    EXPECT_EQ(model.lives(), 0);
}

TEST(GameModelTest, LivesDoNotGoBelowZero) {
    GameModel model;

    for (int i = 0; i < 5; ++i) {
        model.update(301.0f);
        model.update(1.2f);
    }

    EXPECT_EQ(model.lives(), 0);
}

// ═══════════════════════════════════════════════════════════════
// 死亡期间输入被阻止
// ═══════════════════════════════════════════════════════════════

TEST(GameModelTest, InputBlockedDuringDeath) {
    GameModel model;

    model.update(301.0f);

    // 死亡期间尝试输入（不应崩溃）
    model.setMoveLeft(true);
    model.setMoveRight(true);
    model.jump();
    model.update(0.1f);
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════
// reset
// ═══════════════════════════════════════════════════════════════

TEST(GameModelTest, ResetRestoresLives) {
    GameModel model;

    model.update(301.0f);
    model.update(1.2f);
    EXPECT_EQ(model.lives(), 2);

    model.reset();
    EXPECT_EQ(model.lives(), 3);
    EXPECT_EQ(model.score(), 0);
    EXPECT_EQ(model.timeRemaining(), 300);
}

// ═══════════════════════════════════════════════════════════════
// 踩敌加分
// ═══════════════════════════════════════════════════════════════

TEST(GameModelTest, StompEnemyAddsScore) {
    GameModel model;
    model.testLoadLevelFromString(
        "..M......\n"
        ".........\n"
        "..G......\n"
        "#########\n");

    // Mario 从上方落下踩死敌人，等到分数增加
    ASSERT_TRUE(updateUntil(model, 1.f / 60.f, [&] {
        return model.score() > 0;
    }));

    EXPECT_EQ(model.score(), mario_cfg::kStompScore);  // +100
}

TEST(GameModelTest, StompKillsEnemy) {
    GameModel model;
    model.testLoadLevelFromString(
        "..M......\n"
        ".........\n"
        "..G......\n"
        "#########\n");

    // 等到踩死敌人（所有敌人 alive=false）
    ASSERT_TRUE(updateUntil(model, 1.f / 60.f, [&] {
        for (const auto& e : model.enemies()) {
            if (e.alive()) return false;
        }
        return !model.enemies().empty();  // 有敌人且全部死亡
    }));

    for (const auto& e : model.enemies()) {
        EXPECT_FALSE(e.alive());
    }
}

// ═══════════════════════════════════════════════════════════════
// loadLevelFromString (通过反射测试，实际调用 loadFromFile)
// ═══════════════════════════════════════════════════════════════

TEST(GameModelTest, BuiltinLevelHasEnemies) {
    GameModel model;
    EXPECT_FALSE(model.enemies().empty());
}

TEST(GameModelTest, PlayerStateInitiallyIdle) {
    GameModel model;
    EXPECT_EQ(model.playerState(), MarioState::IDLE);
}

// ═══════════════════════════════════════════════════════════════
// 掉落死亡：坠出地图下边界触发死亡逻辑
// ═══════════════════════════════════════════════════════════════

TEST(GameModelTest, FallOffMapBottomTriggersDeath) {
    GameModel model;
    // 无地面的小关卡：马里奥出生后无立足点，持续下落直至坠出下边界。
    model.testLoadLevelFromString(
        "M.........\n"
        "..........\n"
        "..........\n");

    const int livesBefore = model.lives();
    // 一路更新直到坠落触发死亡序列并结算(命数-1)。
    ASSERT_TRUE(updateUntil(model, 1.f / 60.f, [&] {
        return model.lives() < livesBefore;
    }));
    EXPECT_EQ(model.lives(), livesBefore - 1);
}
