#include "model/GameModel.h"

#include <algorithm>
#include <cstddef>

#include "common/EventId.h"       // ModelEvent
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
  notifyChanged();
}

bool GameModel::loadLevelFromFile(const std::string& path) {
  if (!tileMap_.loadFromFile(path)) return false;
  levelFile_ = path;
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  notifyChanged();
  return true;
}

MarioView GameModel::marioView() const {
  MarioView mv;
  mv.x = mario_.x();
  mv.y = mario_.y();
  mv.width = mario_.width();
  mv.height = mario_.height();
  mv.state = mario_.state();
  mv.facing = mario_.facing();
  return mv;
}

LevelView GameModel::levelView() const {
  LevelView lv;
  lv.cols = tileMap_.cols();
  lv.rows = tileMap_.rows();
  lv.tileSize = mario_cfg::kTileSize;
  lv.widthPx = tileMap_.widthPx();  // 关卡整体绝对尺寸，供 ViewModel 算相机边界
  lv.heightPx = tileMap_.heightPx();
  lv.tiles.reserve(static_cast<std::size_t>(lv.cols) * lv.rows);
  for (int r = 0; r < lv.rows; ++r) {
    for (int c = 0; c < lv.cols; ++c) {
      lv.tiles.push_back(tileMap_.at(c, r));
    }
  }
  return lv;
}

void GameModel::notifyChanged() {
  modelTrigger.fire(static_cast<EventType>(ModelEvent::STATE_CHANGED));
}
