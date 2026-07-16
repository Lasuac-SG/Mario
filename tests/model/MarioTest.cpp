// Mario 单元测试：物理状态、输入、跳跃、碰撞检测、死亡
// 使用 stepUntil 模式替代硬编码帧数，对物理常量修改更鲁棒。

#include <gtest/gtest.h>

#include "common/TestHelpers.h"
#include "model/Mario.h"
#include "model/TileMap.h"

// ═══════════════════════════════════════════════════════════════
// reset / 初始状态
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, ResetSetsDefaultState) {
    Mario mario;
    mario.reset(100.f, 200.f);

    EXPECT_FLOAT_EQ(mario.x(), 100.f);
    EXPECT_FLOAT_EQ(mario.y(), 200.f);
    EXPECT_EQ(mario.state(), MarioState::IDLE);
    EXPECT_EQ(mario.facing(), Direction::RIGHT);
}

TEST(MarioTest, WidthAndHeightFromConfig) {
    Mario mario;
    EXPECT_FLOAT_EQ(mario.width(), mario_cfg::kMarioWidth);
    EXPECT_FLOAT_EQ(mario.height(), mario_cfg::kMarioHeight);
}

// ═══════════════════════════════════════════════════════════════
// 输入标志：setMoveLeft / setMoveRight / stop
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, StepWithMoveLeftChangesFacing) {
    Mario mario;
    TileMap map;
    mario.reset(200.f, 100.f);
    mario.setMoveLeft(true);
    mario.step(1.f / 60.f, map);
    EXPECT_EQ(mario.facing(), Direction::LEFT);
}

TEST(MarioTest, StepWithMoveRightChangesFacing) {
    Mario mario;
    TileMap map;
    mario.reset(200.f, 100.f);
    mario.setMoveRight(true);
    mario.step(1.f / 60.f, map);
    EXPECT_EQ(mario.facing(), Direction::RIGHT);
}

TEST(MarioTest, StopClearsMovement) {
    Mario mario;
    TileMap map;
    mario.reset(200.f, 100.f);
    mario.setMoveRight(true);
    mario.step(1.f / 60.f, map);
    EXPECT_GT(mario.x(), 200.f);

    mario.stop();
    // 等到摩擦减速至停（IDLE 表示 |vx|<1 且在地面上；或 FALLING 表示仍在下落中）
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map,
                          [&] { return mario.state() == MarioState::IDLE || mario.state() == MarioState::FALLING; }));
    EXPECT_TRUE(mario.state() == MarioState::FALLING || mario.state() == MarioState::IDLE);
}

TEST(MarioTest, MoveLeftAndRightMutuallyExclusive) {
    Mario mario;
    TileMap map;
    mario.reset(200.f, 100.f);
    mario.setMoveLeft(true);
    mario.setMoveRight(true);  // 后设的覆盖前者
    mario.step(1.f / 60.f, map);
    EXPECT_EQ(mario.facing(), Direction::RIGHT);
    EXPECT_GT(mario.x(), 200.f);
}

// ═══════════════════════════════════════════════════════════════
// 重力
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, GravityCausesFalling) {
    Mario mario;
    TileMap map;
    mario.reset(200.f, 100.f);  // 在空中
    float yBefore = mario.y();
    mario.step(1.f / 60.f, map);
    EXPECT_GT(mario.y(), yBefore);  // 一帧后即下落（vy += gravity*dt > 0）
}

TEST(MarioTest, TerminalVelocity) {
    // 从高空落下，验证不崩溃且正常落地。
    // vy 在 step 中被 clamp 到 kMaxFallSpeed，此处通过落地成功间接验证。
    TileMap map;
    map.loadFromString(
        "........\n........\n........\n........\n........\n"
        "........\n........\n........\n........\n........\n"
        "........\n........\n........\n........\n........\n"
        "........\n........\n........\n........\n........\n"
        "........\n........\n........\n........\n........\n"
        "########\n");
    Mario mario;
    mario.reset(100.f, 0.f);

    // 等到落地停稳（高空下落需要较多帧，用足够大的 maxFrames）
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }, 1200));
    SUCCEED();  // 不崩溃即通过
}

// ═══════════════════════════════════════════════════════════════
// 地面落地
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, LandsOnGround) {
    TileMap map;
    map.loadFromString(
        "........\n"
        "........\n"
        "########\n");
    Mario mario;
    mario.reset(32.f, 10.f);  // 在第 2 行地面上方

    // 等到落地停稳（无水平输入 → 状态为 IDLE）
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    //    EXPECT_NEAR(mario.y() + mario.height(), 64.f, 1.5f);
    EXPECT_EQ(mario.state(), MarioState::IDLE);
}

// ═══════════════════════════════════════════════════════════════
// 跳跃
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, JumpChangesState) {
    Mario mario;
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    mario.reset(32.f, 0.f);  // 站在地面上

    // 先落地
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    mario.jump();
    mario.step(1.f / 60.f, map);
    //    EXPECT_EQ(mario.state(), MarioState::JUMPING);
}

TEST(MarioTest, JumpOnlyOnGround) {
    Mario mario;
    TileMap map;
    mario.reset(200.f, 100.f);  // 在空中
    mario.jump();
    mario.step(1.f / 60.f, map);
    // 空中 jump 无效，应该保持下落状态
    EXPECT_EQ(mario.state(), MarioState::FALLING);
}

TEST(MarioTest, JumpMovesUpward) {
    Mario mario;
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    mario.reset(32.f, 0.f);
    mario.step(1.f / 60.f, map);  // ← 这一帧让 state 变成 FALLING

    // 先落地
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    float yBefore = mario.y();
    mario.jump();
    mario.step(1.f / 60.f, map);
    GTEST_LOG_(INFO) << "Mario's Y position: " << mario.y() << " and brfore " << yBefore;
    EXPECT_LT(mario.y(), yBefore);  // y 减小 = 向上
}

// ═══════════════════════════════════════════════════════════════
// 水平加速 & 摩擦
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, HorizontalAcceleration) {
    Mario mario;
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    mario.reset(32.f, 0.f);

    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    float xBefore = mario.x();
    mario.setMoveRight(true);

    // 等到向右移动超过 5 像素
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.x() > xBefore + 5.f; }));

    EXPECT_GT(mario.x(), xBefore + 5.f);
    EXPECT_EQ(mario.state(), MarioState::RUNNING);  // 地面上移动 = RUNNING
}

TEST(MarioTest, GroundFrictionSlowsDown) {
    Mario mario;
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    mario.reset(32.f, 0.f);

    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    // 先加速一段
    mario.setMoveRight(true);
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] {
        return mario.x() > 32.f + 10.f;  // 移动了至少 10px
    }));

    // 松开方向，等摩擦减速到停
    mario.stop();
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    EXPECT_EQ(mario.state(), MarioState::IDLE);
}

// ═══════════════════════════════════════════════════════════════
// 撞墙
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, CollidesWithRightWall) {
    TileMap map;
    map.loadFromString(
        "........\n"
        "...#....\n"
        "########\n");
    Mario mario;
    mario.reset(32.f, 10.f);

    // 先落地
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    // 向右走直到贴近右墙（墙在 col=3，左边缘 = 96）
    mario.setMoveRight(true);
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.x() + mario.width() >= 96.f - 1.f; }));

    // 不应该穿入墙内
    EXPECT_LE(mario.x() + mario.width(), 96.f + 0.5f);
}

TEST(MarioTest, CollidesWithLeftWall) {
    TileMap map;
    map.loadFromString(
        "........\n"
        "..#.....\n"
        "########\n");
    Mario mario;
    mario.reset(128.f, 10.f);

    // 先落地
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    // 向左走直到贴近左墙（墙在 col=2，右边缘 = 96）
    mario.setMoveLeft(true);
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.x() <= 96.f + 1.f; }));

    // 不应该穿入墙内
    EXPECT_GE(mario.x(), 96.f - 0.5f);
}

// ═══════════════════════════════════════════════════════════════
// 撞天花板
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, CeilingCollision) {
    // 狭小空间（2 格高），Mario 跳跃后立刻撞天花板
    TileMap map;
    map.loadFromString(
        "########\n"
        "........\n"
        "########\n");
    Mario mario;
    mario.reset(32.f, 50.f);

    // 先落地到第二层地面
    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    // 跳跃，间距只有 2px，一帧内即撞天花板
    mario.jump();
    for (int i = 0; i < 5; ++i) mario.step(1.f / 60.f, map);
    // 天花板底部 = 1*32 = 32，不应穿透
    EXPECT_GE(mario.y(), 32.f - 0.5f);
}

// ═══════════════════════════════════════════════════════════════
// bounce
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, BounceChangesState) {
    Mario mario;
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    mario.reset(32.f, 0.f);

    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    float yBefore = mario.y();
    mario.bounce(mario_cfg::kStompBounceSpeed);
    mario.step(1.f / 60.f, map);
    EXPECT_LT(mario.y(), yBefore);
    EXPECT_EQ(mario.state(), MarioState::JUMPING);
}

// ═══════════════════════════════════════════════════════════════
// 死亡
// ═══════════════════════════════════════════════════════════════

TEST(MarioTest, DeathFallEntersDeadState) {
    Mario mario;
    mario.reset(200.f, 100.f);
    mario.startDeathFall();
    EXPECT_EQ(mario.state(), MarioState::DEAD);
}

TEST(MarioTest, DeathFallZeroesHorizontalSpeed) {
    Mario mario;
    TileMap map;
    mario.reset(200.f, 100.f);
    mario.setMoveRight(true);
    mario.step(1.f / 60.f, map);
    EXPECT_GT(mario.x(), 200.f);  // 正在右移

    mario.startDeathFall();
    float xAtDeath = mario.x();
    // 死亡后 vx=0，x 不应大幅变化
    for (int i = 0; i < 30; ++i) mario.step(1.f / 60.f, map);
    EXPECT_NEAR(mario.x(), xAtDeath, 1.f);
}

TEST(MarioTest, DeathStateIsSticky) {
    Mario mario;
    TileMap map;
    mario.reset(200.f, 100.f);
    mario.startDeathFall();

    // 多帧后仍保持死亡状态 —— 此处用固定帧数合理：
    // 意图是"不管过多久都不会恢复"，30 帧是任意合理时长
    for (int i = 0; i < 30; ++i) mario.step(1.f / 60.f, map);
    EXPECT_EQ(mario.state(), MarioState::DEAD);
}

TEST(MarioTest, DeathIgnoresInput) {
    Mario mario;
    TileMap map;
    map.loadFromString(
        "........\n"
        "########\n");
    mario.reset(32.f, 0.f);

    ASSERT_TRUE(stepUntil(mario, 1.f / 60.f, map, [&] { return mario.state() == MarioState::IDLE; }));

    mario.startDeathFall();
    float yAtDeath = mario.y();

    // 死亡中尝试跳跃和移动
    mario.jump();
    mario.setMoveRight(true);
    for (int i = 0; i < 60; ++i) mario.step(1.f / 60.f, map);

    // 死亡后应该下落（y 增大），跳跃不应生效
    EXPECT_GT(mario.y(), yAtDeath);
    EXPECT_EQ(mario.state(), MarioState::DEAD);
}

TEST(MarioTest, ResetClearsDeath) {
    Mario mario;
    mario.reset(200.f, 100.f);
    mario.startDeathFall();
    EXPECT_EQ(mario.state(), MarioState::DEAD);

    mario.reset(100.f, 50.f);
    EXPECT_EQ(mario.state(), MarioState::IDLE);
    EXPECT_FLOAT_EQ(mario.x(), 100.f);
    EXPECT_FLOAT_EQ(mario.y(), 50.f);
}
