// Enemy 单元测试：巡逻、撞墙掉头、悬崖掉头、重力落地、死亡
// 使用 stepUntil 模式替代硬编码帧数。

#include "common/TestHelpers.h"
#include "model/Enemy.h"
#include "model/TileMap.h"

#include <gtest/gtest.h>

// ═══════════════════════════════════════════════════════════════
// reset / 初始状态
// ═══════════════════════════════════════════════════════════════

TEST(EnemyTest, ResetSetsPositionAndAlive) {
    Enemy enemy;
    enemy.reset(100.f, 200.f, Direction::LEFT);

    EXPECT_FLOAT_EQ(enemy.x(), 100.f);
    EXPECT_FLOAT_EQ(enemy.y(), 200.f);
    EXPECT_TRUE(enemy.alive());
}

TEST(EnemyTest, ResetFacingLeftMovesLeft) {
    Enemy enemy;
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    enemy.reset(100.f, 0.f, Direction::LEFT);
    enemy.step(1.f / 60.f, map);

    EXPECT_EQ(enemy.facing(), Direction::LEFT);
    EXPECT_LT(enemy.x(), 100.f);  // 向左移动
}

TEST(EnemyTest, ResetFacingRightMovesRight) {
    Enemy enemy;
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    enemy.reset(100.f, 0.f, Direction::RIGHT);
    enemy.step(1.f / 60.f, map);

    EXPECT_EQ(enemy.facing(), Direction::RIGHT);
    EXPECT_GT(enemy.x(), 100.f);  // 向右移动
}

TEST(EnemyTest, DimensionsFromConfig) {
    Enemy enemy;
    EXPECT_FLOAT_EQ(enemy.width(), mario_cfg::kEnemyWidth);
    EXPECT_FLOAT_EQ(enemy.height(), mario_cfg::kEnemyHeight);
}

// ═══════════════════════════════════════════════════════════════
// 重力落地
// ═══════════════════════════════════════════════════════════════

TEST(EnemyTest, GravityFallsDown) {
    Enemy enemy;
    TileMap map;
    map.loadFromString(
        "........\n"
        "........\n"
        "........\n"
        "########\n");
    enemy.reset(32.f, 0.f, Direction::RIGHT);

    float yBefore = enemy.y();
    enemy.step(1.f / 60.f, map);
    EXPECT_GT(enemy.y(), yBefore);  // 一帧即下落
}

TEST(EnemyTest, LandsOnGroundAndPatrols) {
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    Enemy enemy;
    enemy.reset(32.f, 0.f, Direction::RIGHT);

    // 等到落到地面上（脚底贴近 y=32 的地面线）
    ASSERT_TRUE(stepUntil(enemy, 1.f / 60.f, map, [&] {
        return enemy.y() + enemy.height() >= 32.f - 1.f;
    }));

    EXPECT_NEAR(enemy.y() + enemy.height(), 32.f, 1.5f);
    EXPECT_TRUE(enemy.alive());
}

// ═══════════════════════════════════════════════════════════════
// 撞墙掉头
// ═══════════════════════════════════════════════════════════════

TEST(EnemyTest, TurnsAtRightWall) {
    TileMap map;
    map.loadFromString(
        "........\n"
        "#......#\n"
        "########\n");
    Enemy enemy;
    enemy.reset(50.f, 10.f, Direction::RIGHT);  // 向右走向右墙

    // 等到撞墙掉头（facing 变为 LEFT）
    ASSERT_TRUE(stepUntil(enemy, 1.f / 60.f, map, [&] {
        return enemy.facing() == Direction::LEFT;
    }));

    EXPECT_EQ(enemy.facing(), Direction::LEFT);
}

TEST(EnemyTest, TurnsAtLeftWall) {
    TileMap map;
    map.loadFromString(
        "........\n"
        "#......#\n"
        "########\n");
    Enemy enemy;
    enemy.reset(150.f, 10.f, Direction::LEFT);  // 向左走向左墙

    // 等到撞墙掉头（facing 变为 RIGHT）
    ASSERT_TRUE(stepUntil(enemy, 1.f / 60.f, map, [&] {
        return enemy.facing() == Direction::RIGHT;
    }));

    EXPECT_EQ(enemy.facing(), Direction::RIGHT);
}

// ═══════════════════════════════════════════════════════════════
// 悬崖掉头
// ═══════════════════════════════════════════════════════════════

TEST(EnemyTest, TurnsAtLedge) {
    TileMap map;
    map.loadFromString(
        "........\n"
        "#####...\n"   // 平台在左边，右边悬空
        "########\n");
    Enemy enemy;
    enemy.reset(32.f, 0.f, Direction::RIGHT);

    // 等到悬崖掉头（facing 变为 LEFT）
    ASSERT_TRUE(stepUntil(enemy, 1.f / 60.f, map, [&] {
        return enemy.facing() == Direction::LEFT;
    }));

    EXPECT_EQ(enemy.facing(), Direction::LEFT);
}

// ═══════════════════════════════════════════════════════════════
// kill
// ═══════════════════════════════════════════════════════════════

TEST(EnemyTest, KillSetsAliveFalse) {
    Enemy enemy;
    enemy.reset(100.f, 200.f, Direction::RIGHT);
    EXPECT_TRUE(enemy.alive());

    enemy.kill();
    EXPECT_FALSE(enemy.alive());
}

TEST(EnemyTest, DeadEnemyDoesNotMove) {
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    Enemy enemy;
    enemy.reset(100.f, 0.f, Direction::RIGHT);

    // 先落地
    ASSERT_TRUE(stepUntil(enemy, 1.f / 60.f, map, [&] {
        return enemy.y() + enemy.height() >= 32.f - 1.f;
    }));

    enemy.kill();
    float xAfterKill = enemy.x();
    float yAfterKill = enemy.y();

    // 死亡后不应移动：stepUntil 以 "位置变了" 为条件，应超时（返回 false）
    bool moved = stepUntil(enemy, 1.f / 60.f, map, [&] {
        return enemy.x() != xAfterKill || enemy.y() != yAfterKill;
    }, 60);
    EXPECT_FALSE(moved);

    // 双重确认位置未变
    EXPECT_FLOAT_EQ(enemy.x(), xAfterKill);
    EXPECT_FLOAT_EQ(enemy.y(), yAfterKill);
}
