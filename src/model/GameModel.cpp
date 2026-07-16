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
      handleBlockBump();  // 顶问号块出蘑菇 / 大马里奥砸砖块
      for (auto& e : enemies_) e.step(step, tileMap_);
      for (auto& m : mushrooms_) m.step(step, tileMap_);
      collectCoins();       // 拾取重叠的金币
      collectMushrooms();   // 吃蘑菇变大
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
  lives_ = kInitialLives;
  resetLevelState();
  notifyChanged();
}

bool GameModel::loadLevelFromFile(const std::string& path) {
  if (!tileMap_.loadFromFile(path)) return false;
  levelFile_ = path;
  lives_ = kInitialLives;
  resetLevelState();
  notifyChanged();
  return true;
}

bool GameModel::testLoadLevelFromString(const std::string& text) {
  if (!tileMap_.loadFromString(text)) return false;
  levelFile_.clear();
  lives_ = kInitialLives;
  resetLevelState();
  notifyChanged();
  return true;
}

// 整关状态重置（分数/金币/时间/马里奥/瓦片/敌人全部恢复初始），不含命数。
// 供初始 reset 与死亡复活共用——死亡后同样做整关重置，只保留已扣的命数。
void GameModel::resetLevelState() {
  score_ = 0;
  coins_ = 0;
  timeRemaining_ = kInitialTimeSeconds;
  deathInProgress_ = false;
  deathElapsed_ = 0.0f;
  mario_.reset(tileMap_.spawnX(), tileMap_.spawnY());
  rebuildTiles();
  spawnEnemies();   // 之前被消灭的敌人全部回来
  spawnCoins();     // 之前拾取的金币全部恢复
  mushrooms_.clear();
  usedQuestions_.clear();
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
  // 死亡后整关重置：分数/金币/敌人/时间/位置全部恢复初始，仅保留已扣的命数。
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

// 依关卡出生点重建金币：在出生格内居中放置。
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

// 马里奥 AABB 与金币重叠即拾取：金币消失、金币数 +1、加分。
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
    }
  }
}

bool GameModel::resolveEnemyCollisions() {
  if (mario_.invincible()) return false;  // 变小后短暂无敌，期间不受敌人影响
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
    } else if (mario_.big()) {
      mario_.shrink();  // 大马里奥碰敌 → 变小(带短暂无敌)，非死亡
      return false;
    } else {
      return true;      // 小马里奥碰敌 → 死亡
    }
  }
  return false;
}

// 顶到问号块 → 上方出蘑菇(每块仅一次)；大马里奥顶到砖块 → 砸碎。
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
      if (usedQuestions_.insert(key).second) {  // 首次顶该问号块
        spawnMushroomAt(c, row);
      }
    } else if (t == TileType::BRICK && mario_.big()) {
      tileMap_.setTile(c, row, TileType::EMPTY);  // 大马里奥砸碎砖块
      score_ += mario_cfg::kBrickScore;
      tilesDirty = true;
    }
  }
  if (tilesDirty) rebuildTiles();
}

// 在问号块(col,row)顶上生成一只向右移动的变大蘑菇。
void GameModel::spawnMushroomAt(int col, int row) {
  const PositionType ts = mario_cfg::kTileSize;
  Mushroom m;
  const PositionType x = col * ts + (ts - mario_cfg::kMushroomWidth) * 0.5f;
  const PositionType y = row * ts - mario_cfg::kMushroomHeight;  // 站在问号块顶面
  m.reset(x, y, Direction::RIGHT);
  mushrooms_.push_back(m);
}

// 马里奥吃到蘑菇：小 → 变大 + 加分；已大 → 无效果(仍消耗蘑菇)。
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
      if (!mario_.big()) {
        mario_.grow();
        score_ += mario_cfg::kMushroomScore;
      }
    }
  }
}

void GameModel::notifyChanged() {
  modelTrigger.fire(static_cast<EventType>(ModelEvent::STATE_CHANGED));
}
