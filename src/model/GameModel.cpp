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
constexpr int kGoalScoreBonus = 1000;
constexpr PositionType kGoalPoleWidth = 24.0f;
constexpr PositionType kGoalPoleHeight = 160.0f;

// 默认加载的关卡文件（相对可执行文件工作目录）。切换关卡改这一行即可，例如 "map/map2.txt"。
// 文件缺失时回退到 TileMap 的内置极简兜底关卡。
constexpr const char* kLevelFile = "map/map1.txt";
}

GameModel::GameModel() {
  levelFile_ = kLevelFile;  // reset() 会据此加载关卡文件
  reset();
}

void GameModel::update(TimeType dt) {
  if (goalReached_) {
    notifyChanged();
    return;
  }

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
      handleBlockBump();
      for (auto& e : enemies_) e.step(step, tileMap_);
      for (auto& m : mushrooms_) m.step(step, tileMap_);
      collectCoins();
      collectMushrooms();
      if (resolveEnemyCollisions()) {
        beginDeath();
        break;
      }
      if (checkGoalReached()) {
        beginGoalClear();
        break;
      }
      remaining -= step;
    }
  }

  notifyChanged();
}

void GameModel::reset() {
  if (!levelFile_.empty()) tileMap_.loadFromFile(levelFile_);
  lives_ = kInitialLives;
  resetLevelState();
  notifyChanged(Event::LEVEL_RESET);
}

bool GameModel::loadLevelFromFile(const std::string& path) {
  if (!tileMap_.loadFromFile(path)) return false;
  levelFile_ = path;
  lives_ = kInitialLives;
  resetLevelState();
  notifyChanged(Event::LEVEL_LOADED);
  return true;
}

bool GameModel::testLoadLevelFromString(const std::string& text) {
  if (!tileMap_.loadFromString(text)) return false;
  levelFile_.clear();
  lives_ = kInitialLives;
  resetLevelState();
  notifyChanged(Event::LEVEL_LOADED);
  return true;
}

void GameModel::resetLevelState() {
  score_ = 0;
  coins_ = 0;
  timeRemaining_ = kInitialTimeSeconds;
  deathInProgress_ = false;
  deathElapsed_ = 0.0f;
  goalReached_ = false;
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  rebuildTiles();
  spawnEnemies();
  spawnCoins();
  mushrooms_.clear();
  usedQuestions_.clear();
  setupGoal();
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
  notifyChanged(Event::MARIO_DIED);
}

void GameModel::respawnAfterDeath() {
  resetLevelState();
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

void GameModel::spawnCoins() {
  coinItems_.clear();
  const PositionType ts = mario_cfg::kTileSize;
  const auto& spawns = tileMap_.coinSpawns();
  coinItems_.reserve(spawns.size());
  for (const auto& s : spawns) {
    Coin c;
    c.w = mario_cfg::kCoinWidth;
    c.h = mario_cfg::kCoinHeight;
    c.x = s.col * ts + (ts - c.w) * 0.5f;
    c.y = s.row * ts + (ts - c.h) * 0.5f;
    c.alive = true;
    coinItems_.push_back(c);
  }
}

void GameModel::setupGoal() {
  goalW_ = kGoalPoleWidth;
  goalH_ = kGoalPoleHeight;

  if (!tileMap_.hasGoal()) {
    goalX_ = std::max(0.0f, tileMap_.widthPx() - 3.0f * mario_cfg::kTileSize);
    goalY_ = std::max(0.0f, tileMap_.heightPx() - goalH_ - mario_cfg::kTileSize);
    return;
  }

  const PositionType ts = mario_cfg::kTileSize;
  const PositionType cellX = tileMap_.goalX();
  const PositionType cellY = tileMap_.goalY();
  goalX_ = cellX + (ts - goalW_) * 0.5f;
  goalY_ = cellY + ts - goalH_;
}

void GameModel::collectCoins() {
  const PositionType mx = mario_.x();
  const PositionType my = mario_.y();
  const PositionType mw = mario_.width();
  const PositionType mh = mario_.height();
  for (auto& c : coinItems_) {
    if (!c.alive) continue;
    const bool overlap = mx < c.x + c.w && mx + mw > c.x && my < c.y + c.h && my + mh > c.y;
    if (overlap) {
      c.alive = false;
      ++coins_;
      score_ += mario_cfg::kCoinScore;
      notifyChanged(Event::COIN_COLLECTED);
    }
  }
}

bool GameModel::resolveEnemyCollisions() {
  if (mario_.invincible()) return false;
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
      notifyChanged(Event::ENEMY_STOMPED);
    } else if (mario_.big()) {
      mario_.shrink();
      notifyChanged(Event::MARIO_SHRUNK);
      return false;
    } else {
      return true;
    }
  }
  return false;
}

void GameModel::handleBlockBump() {
  if (!mario_.headBumped()) return;
  const int row = mario_.headBumpRow();
  const int c0 = tileMap_.toCol(mario_.x());
  const int c1 = tileMap_.toCol(mario_.x() + mario_.width() - 0.001f);
  bool tilesDirty = false;
  for (int c = c0; c <= c1; ++c) {
    const TileType t = tileMap_.at(c, row);
    if (t == TileType::QUESTION) {
      const int key = row * tileMap_.cols() + c;
      if (usedQuestions_.insert(key).second) {
        spawnMushroomAt(c, row);
        notifyChanged(Event::BLOCK_BUMPED);
      }
    } else if (t == TileType::BRICK && mario_.big()) {
      tileMap_.setTile(c, row, TileType::EMPTY);
      score_ += mario_cfg::kBrickScore;
      tilesDirty = true;
      notifyChanged(Event::BRICK_BROKEN);
    }
  }
  if (tilesDirty) rebuildTiles();
}

void GameModel::spawnMushroomAt(int col, int row) {
  const PositionType ts = mario_cfg::kTileSize;
  Mushroom m;
  const PositionType x = col * ts + (ts - mario_cfg::kMushroomWidth) * 0.5f;
  const PositionType y = row * ts - mario_cfg::kMushroomHeight;
  m.reset(x, y, Direction::RIGHT);
  mushrooms_.push_back(m);
}

void GameModel::collectMushrooms() {
  const PositionType mx = mario_.x();
  const PositionType my = mario_.y();
  const PositionType mw = mario_.width();
  const PositionType mh = mario_.height();
  for (auto& m : mushrooms_) {
    if (!m.active()) continue;
    const bool overlap =
        mx < m.x() + m.width() && mx + mw > m.x() && my < m.y() + m.height() && my + mh > m.y();
    if (overlap) {
      m.consume();
      notifyChanged(Event::MUSHROOM_COLLECTED);
      if (!mario_.big()) {
        mario_.grow();
        score_ += mario_cfg::kMushroomScore;
        notifyChanged(Event::MARIO_GROWN);
      }
    }
  }
}

bool GameModel::checkGoalReached() const {
  const PositionType mx = mario_.x();
  const PositionType my = mario_.y();
  const PositionType mw = mario_.width();
  const PositionType mh = mario_.height();

  const PositionType triggerX = goalX_ - 8.0f;
  const PositionType triggerW = goalW_ + 16.0f;
  return mx < triggerX + triggerW && mx + mw > triggerX && my < goalY_ + goalH_ && my + mh > goalY_;
}

void GameModel::beginGoalClear() {
  if (goalReached_) return;
  goalReached_ = true;
  mario_.stop();
  score_ += kGoalScoreBonus;
  notifyChanged(Event::GOAL_REACHED);
}

void GameModel::notifyChanged(Event ev) {
  modelTrigger.fire(static_cast<EventType>(ev));
}
