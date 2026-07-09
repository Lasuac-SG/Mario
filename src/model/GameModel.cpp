#include "model/GameModel.h"

#include <algorithm>

#include "common/EventId.h"       // ModelEvent, TileType
#include "common/Type.h"          // EventType
#include "model/PhysicsConfig.h"  // mario_cfg::*

GameModel::GameModel() { reset(); }

void GameModel::update(TimeType dt) {
  if (dt > 0.f) {
    // 固定子步长切分，避免大 dt 时高速穿透。
    TimeType remaining = dt;
    while (remaining > 0.f) {
      const TimeType step = std::min(remaining, mario_cfg::kMaxStep);
      mario_.step(step, tileMap_);
      remaining -= step;
    }
  }
  notifyChanged();
}

void GameModel::reset() {
  if (!levelFile_.empty()) tileMap_.loadFromFile(levelFile_);
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  rebuildTiles();
  notifyChanged();
}

bool GameModel::loadLevelFromFile(const std::string& path) {
  if (!tileMap_.loadFromFile(path)) return false;
  levelFile_ = path;
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  rebuildTiles();
  notifyChanged();
  return true;
}

// 遍历瓦片地图，把实心瓦片按「绝对像素坐标」收集成 Tile 列表并缓存。
void GameModel::rebuildTiles() {
  tiles_.clear();
  const PositionType ts = mario_cfg::kTileSize;
  for (int r = 0; r < tileMap_.rows(); ++r) {
    for (int c = 0; c < tileMap_.cols(); ++c) {
      const TileType t = tileMap_.at(c, r);
      if (TileMap::isSolidTile(t)) {
        tiles_.push_back(Tile{c * ts, r * ts, ts, ts, t});
      }
    }
  }
}

void GameModel::notifyChanged() {
  modelTrigger.fire(static_cast<EventType>(ModelEvent::STATE_CHANGED));
}
