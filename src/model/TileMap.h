// 瓦片地图：行主序存储 TileType，负责文本关卡解析与像素↔瓦片格换算。纯逻辑，无渲染。

#ifndef MARIO_MODEL_TILEMAP_H
#define MARIO_MODEL_TILEMAP_H

#include <cmath>
#include <cstddef>
#include <istream>
#include <string>
#include <vector>

#include "common/EventId.h"       // TileType
#include "model/PhysicsConfig.h"  // PositionType

class TileMap {
 public:
  TileMap();  // 构造即加载内置默认关卡

  // === 关卡加载（失败返回 false 且不改动当前地图）===
  // 文本格式：'.'/空格=EMPTY，'#'=GROUND，'P'=PIPE，'='=PLATFORM，'M'/'m'=出生点(该格记为 EMPTY)。
  bool loadFromString(const std::string& text);
  bool loadFromFile(const std::string& path);
  bool loadFromStream(std::istream& in);

  // === 查询（轻量，内联）===
  int cols() const { return cols_; }
  int rows() const { return rows_; }

  // 越界返回 EMPTY（供上层安全遍历）。
  TileType at(int col, int row) const {
    if (col < 0 || row < 0 || col >= cols_ || row >= rows_) return TileType::EMPTY;
    return tiles_[static_cast<std::size_t>(row) * cols_ + col];
  }

  // 碰撞用：左右与底部越界视为实心墙防止跑出世界；顶部开放。
  bool isSolid(int col, int row) const {
    if (col < 0 || col >= cols_) return true;
    if (row >= rows_) return true;
    if (row < 0) return false;
    return isSolidTile(at(col, row));
  }

  static bool isSolidTile(TileType t) {
    return t == TileType::GROUND || t == TileType::PIPE || t == TileType::PLATFORM;
  }

  // 像素 → 瓦片格坐标。
  int toCol(PositionType x) const { return static_cast<int>(std::floor(x / mario_cfg::kTileSize)); }
  int toRow(PositionType y) const { return static_cast<int>(std::floor(y / mario_cfg::kTileSize)); }

  PositionType widthPx() const { return cols_ * mario_cfg::kTileSize; }
  PositionType heightPx() const { return rows_ * mario_cfg::kTileSize; }

  // 马里奥出生点（像素，左上角）。
  PositionType spawnX() const { return spawnCol_ * mario_cfg::kTileSize; }
  PositionType spawnY() const { return spawnRow_ * mario_cfg::kTileSize; }

  // 敌人出生点（瓦片格坐标）。文本中的 'G'/'g' 标记，可多只；该格记为 EMPTY。
  struct SpawnCell {
    int col;
    int row;
  };
  const std::vector<SpawnCell>& enemySpawns() const { return enemySpawns_; }

  // 金币出生点（瓦片格坐标）。文本中的 'C'/'c' 标记，可多枚；该格记为 EMPTY。
  const std::vector<SpawnCell>& coinSpawns() const { return coinSpawns_; }

 private:
  std::vector<TileType> tiles_;
  std::vector<SpawnCell> enemySpawns_;
  std::vector<SpawnCell> coinSpawns_;
  int cols_ = 0;
  int rows_ = 0;
  int spawnCol_ = 1;
  int spawnRow_ = 0;
};

#endif  // MARIO_MODEL_TILEMAP_H
