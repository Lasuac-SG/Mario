#include "model/GameModel.h"

#include <algorithm>
#include <cmath>

#include "common/EventId.h"
#include "common/Type.h"
#include "model/PhysicsConfig.h"

namespace {
constexpr float kInitialTimeSeconds = 300.0f;
constexpr int kInitialLives = 3;
constexpr float kDeathSequenceSeconds = 1.2f;
}

GameModel::GameModel() { reset(); }

void GameModel::update(TimeType dt) {
  if (timeoutDeathInProgress_) {
    if (dt > 0.f) {
      TimeType remaining = dt;
      while (remaining > 0.f) {
        const TimeType step = std::min(remaining, mario_cfg::kMaxStep);
        mario_.step(step, tileMap_);
        deathElapsed_ += step;
        remaining -= step;
      }
    }

    if (deathElapsed_ >= kDeathSequenceSeconds) {
      lives_ = std::max(0, lives_ - 1);
      respawnAfterDeath();
    }

    notifyChanged();
    return;
  }

  if (dt > 0.f) {
    timeRemaining_ = std::max(0.0f, timeRemaining_ - dt);
    if (timeRemaining_ <= 0.0f) {
      beginTimeoutDeath();
      notifyChanged();
      return;
    }

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

  score_ = 0;
  coins_ = 0;
  lives_ = kInitialLives;
  timeRemaining_ = kInitialTimeSeconds;
  timeoutDeathInProgress_ = false;
  deathElapsed_ = 0.0f;

  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  rebuildTiles();
  notifyChanged();
}

bool GameModel::loadLevelFromFile(const std::string& path) {
  if (!tileMap_.loadFromFile(path)) return false;
  levelFile_ = path;
  score_ = 0;
  coins_ = 0;
  lives_ = kInitialLives;
  timeRemaining_ = kInitialTimeSeconds;
  timeoutDeathInProgress_ = false;
  deathElapsed_ = 0.0f;
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  rebuildTiles();
  notifyChanged();
  return true;
}

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

void GameModel::beginTimeoutDeath() {
  timeoutDeathInProgress_ = true;
  deathElapsed_ = 0.0f;
  mario_.startDeathFall();
}

void GameModel::respawnAfterDeath() {
  timeoutDeathInProgress_ = false;
  deathElapsed_ = 0.0f;
  timeRemaining_ = kInitialTimeSeconds;
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
}

void GameModel::notifyChanged() {
  modelTrigger.fire(static_cast<EventType>(ModelEvent::STATE_CHANGED));
}
