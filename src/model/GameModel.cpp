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
  if (deathInProgress_) {
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
      beginDeath();
      notifyChanged();
      return;
    }

    TimeType remaining = dt;
    while (remaining > 0.f) {
      const TimeType step = std::min(remaining, mario_cfg::kMaxStep);
      mario_.step(step, tileMap_);
      for (auto& e : enemies_) e.step(step, tileMap_);
      if (resolveEnemyCollisions()) {
        beginDeath();
        break;
      }
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
  deathInProgress_ = false;
  deathElapsed_ = 0.0f;

  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  rebuildTiles();
  spawnEnemies();
  notifyChanged();
}

bool GameModel::loadLevelFromFile(const std::string& path) {
  if (!tileMap_.loadFromFile(path)) return false;
  levelFile_ = path;
  score_ = 0;
  coins_ = 0;
  lives_ = kInitialLives;
  timeRemaining_ = kInitialTimeSeconds;
  deathInProgress_ = false;
  deathElapsed_ = 0.0f;
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  rebuildTiles();
  spawnEnemies();
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

void GameModel::beginDeath() {
  deathInProgress_ = true;
  deathElapsed_ = 0.0f;
  mario_.startDeathFall();
}

void GameModel::respawnAfterDeath() {
  deathInProgress_ = false;
  deathElapsed_ = 0.0f;
  timeRemaining_ = kInitialTimeSeconds;
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
}

void GameModel::spawnEnemies() {
  enemies_.clear();
  const PositionType ts = mario_cfg::kTileSize;
  const auto& spawns = tileMap_.enemySpawns();
  enemies_.resize(spawns.size());
  for (std::size_t i = 0; i < spawns.size(); ++i) {
    const PositionType x = spawns[i].col * ts + (ts - mario_cfg::kEnemyWidth) * 0.5f;
    const PositionType y = (spawns[i].row + 1) * ts - mario_cfg::kEnemyHeight;
    enemies_[i].reset(x, y, Direction::LEFT);
  }
}

bool GameModel::resolveEnemyCollisions() {
  const PositionType mx = mario_.x();
  const PositionType my = mario_.y();
  const PositionType mw = mario_.width();
  const PositionType mh = mario_.height();
  for (auto& e : enemies_) {
    if (!e.alive()) continue;
    const PositionType ex = e.x();
    const PositionType ey = e.y();
    const PositionType ew = e.width();
    const PositionType eh = e.height();
    const bool overlap = mx < ex + ew && mx + mw > ex && my < ey + eh && my + mh > ey;
    if (!overlap) continue;

    const bool stomp = mario_.vy() > 0.0f && (my + mh) < ey + eh * 0.5f;
    if (stomp) {
      e.kill();
      mario_.bounce(mario_cfg::kStompBounceSpeed);
      score_ += mario_cfg::kStompScore;
    } else {
      return true;
    }
  }
  return false;
}

void GameModel::notifyChanged() {
  modelTrigger.fire(static_cast<EventType>(ModelEvent::STATE_CHANGED));
}
