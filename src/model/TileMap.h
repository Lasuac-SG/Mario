#ifndef MARIO_MODEL_TILEMAP_H
#define MARIO_MODEL_TILEMAP_H

#include <cmath>
#include <cstddef>
#include <istream>
#include <string>
#include <vector>

#include "common/EventId.h"
#include "model/PhysicsConfig.h"

class TileMap {
 public:
  TileMap();

  bool loadFromString(const std::string& text);
  bool loadFromFile(const std::string& path);
  bool loadFromStream(std::istream& in);

  int cols() const { return cols_; }
  int rows() const { return rows_; }

  TileType at(int col, int row) const {
    if (col < 0 || row < 0 || col >= cols_ || row >= rows_) return TileType::EMPTY;
    return tiles_[static_cast<std::size_t>(row) * cols_ + col];
  }

  bool isSolid(int col, int row) const {
    if (col < 0 || col >= cols_) return true;
    if (row >= rows_) return true;
    if (row < 0) return false;
    return isSolidTile(at(col, row));
  }

  static bool isSolidTile(TileType t) {
    return t == TileType::GROUND || t == TileType::PIPE || t == TileType::PLATFORM ||
           t == TileType::QUESTION || t == TileType::BRICK;
  }

  void setTile(int col, int row, TileType t) {
    if (col < 0 || row < 0 || col >= cols_ || row >= rows_) return;
    tiles_[static_cast<std::size_t>(row) * cols_ + col] = t;
  }

  int toCol(PositionType x) const { return static_cast<int>(std::floor(x / mario_cfg::kTileSize)); }
  int toRow(PositionType y) const { return static_cast<int>(std::floor(y / mario_cfg::kTileSize)); }

  PositionType widthPx() const { return cols_ * mario_cfg::kTileSize; }
  PositionType heightPx() const { return rows_ * mario_cfg::kTileSize; }

  PositionType spawnX() const { return spawnCol_ * mario_cfg::kTileSize; }
  PositionType spawnY() const { return spawnRow_ * mario_cfg::kTileSize; }

  struct SpawnCell {
    int col;
    int row;
  };
  const std::vector<SpawnCell>& enemySpawns() const { return enemySpawns_; }
  const std::vector<SpawnCell>& coinSpawns() const { return coinSpawns_; }

  bool hasGoal() const { return hasGoal_; }
  PositionType goalX() const { return goalCol_ * mario_cfg::kTileSize; }
  PositionType goalY() const { return goalRow_ * mario_cfg::kTileSize; }

 private:
  std::vector<TileType> tiles_;
  std::vector<SpawnCell> enemySpawns_;
  std::vector<SpawnCell> coinSpawns_;
  int cols_ = 0;
  int rows_ = 0;
  int spawnCol_ = 1;
  int spawnRow_ = 0;
  bool hasGoal_ = false;
  int goalCol_ = 0;
  int goalRow_ = 0;
};

#endif  // MARIO_MODEL_TILEMAP_H
