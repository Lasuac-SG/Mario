// TileMap 单元测试

#include "model/TileMap.h"

#include <gtest/gtest.h>

// ── 默认构造 ─────────────────────────────────────────────────

TEST(TileMapTest, DefaultConstructorLoadsBuiltin) {
    TileMap map;
    EXPECT_GT(map.cols(), 0);
    EXPECT_GT(map.rows(), 0);
    EXPECT_GT(map.widthPx(), 0);
    EXPECT_GT(map.heightPx(), 0);
}

TEST(TileMapTest, DimensionsAreConsistent) {
    TileMap map;
    EXPECT_EQ(map.widthPx(), map.cols() * mario_cfg::kTileSize);
    EXPECT_EQ(map.heightPx(), map.rows() * mario_cfg::kTileSize);
}

// ── at() 访问 ────────────────────────────────────────────────

TEST(TileMapTest, AtReturnsGroundForHash) {
    TileMap map;
    // 内置关卡底行为 #
    const int bottomRow = map.rows() - 1;
    EXPECT_EQ(map.at(0, bottomRow), TileType::GROUND);
}

TEST(TileMapTest, AtReturnsEmptyForOutOfBounds) {
    TileMap map;
    EXPECT_EQ(map.at(-1, -1), TileType::EMPTY);
    EXPECT_EQ(map.at(map.cols(), map.rows()), TileType::EMPTY);
}

// ── isSolid ──────────────────────────────────────────────────

TEST(TileMapTest, IsSolidTrueForGround) {
    TileMap map;
    const int bottomRow = map.rows() - 1;
    EXPECT_TRUE(map.isSolid(0, bottomRow));
}

TEST(TileMapTest, IsSolidEdgeWalls) {
    TileMap map;
    const int midRow = map.rows() / 2;
    // 左边界 (col = -1) 返回固体（世界墙）
    EXPECT_TRUE(map.isSolid(-1, midRow));
    // 右边界 (col = cols) 返回固体
    EXPECT_TRUE(map.isSolid(map.cols(), midRow));
    // 下边界 (row = rows) 返回固体
    EXPECT_TRUE(map.isSolid(0, map.rows()));
    // 顶部 (row = -1) 开放
    EXPECT_FALSE(map.isSolid(0, -1));
}

// ── isSolidTile 静态方法 ─────────────────────────────────────

TEST(TileMapTest, IsSolidTileForEachType) {
    EXPECT_FALSE(TileMap::isSolidTile(TileType::EMPTY));
    EXPECT_TRUE(TileMap::isSolidTile(TileType::GROUND));
    EXPECT_TRUE(TileMap::isSolidTile(TileType::PIPE));
    EXPECT_TRUE(TileMap::isSolidTile(TileType::PLATFORM));
}

// ── toCol / toRow 坐标转换 ───────────────────────────────────

TEST(TileMapTest, ToColRowZero) {
    TileMap map;
    EXPECT_EQ(map.toCol(0), 0);
    EXPECT_EQ(map.toRow(0), 0);
}

TEST(TileMapTest, ToColRowAtTileSize) {
    TileMap map;
    const auto ts = mario_cfg::kTileSize;
    EXPECT_EQ(map.toCol(ts), 1);
    EXPECT_EQ(map.toRow(ts), 1);
}

TEST(TileMapTest, ToColRowFractional) {
    TileMap map;
    const auto ts = mario_cfg::kTileSize;
    // 0.5 格落在第 0 格（floor）
    EXPECT_EQ(map.toCol(ts * 0.5f), 0);
    EXPECT_EQ(map.toRow(ts * 0.5f), 0);
}

// ── loadFromString ───────────────────────────────────────────

TEST(TileMapTest, LoadFromStringBasic) {
    TileMap map;
    ASSERT_TRUE(map.loadFromString("##\n.."));
    EXPECT_EQ(map.cols(), 2);
    EXPECT_EQ(map.rows(), 2);
    EXPECT_EQ(map.at(0, 0), TileType::GROUND);
    EXPECT_EQ(map.at(1, 0), TileType::GROUND);
    EXPECT_EQ(map.at(0, 1), TileType::EMPTY);
    EXPECT_EQ(map.at(1, 1), TileType::EMPTY);
}

TEST(TileMapTest, LoadFromStringEmptyFails) {
    TileMap map;
    // 记录加载前的状态
    const auto oldCols = map.cols();
    EXPECT_FALSE(map.loadFromString(""));
    // 加载失败后地图不应被修改（事务性）
    EXPECT_EQ(map.cols(), oldCols);
}

TEST(TileMapTest, LoadFromStringWithSpawnMarker) {
    TileMap map;
    ASSERT_TRUE(map.loadFromString("M."));
    EXPECT_EQ(map.at(0, 0), TileType::EMPTY);  // 出生格保持 EMPTY
    EXPECT_EQ(map.spawnX(), 0);
    EXPECT_EQ(map.spawnY(), 0);
}

// ── spawn 默认值 ─────────────────────────────────────────────

TEST(TileMapTest, SpawnWithinBounds) {
    TileMap map;
    EXPECT_GE(map.spawnX(), 0);
    EXPECT_GE(map.spawnY(), 0);
    EXPECT_LT(map.spawnX(), map.widthPx());
    EXPECT_LT(map.spawnY(), map.heightPx());
}
