// 变大蘑菇道具：从问号块顶出后水平移动 + 受重力，遇实心墙掉头，遇空缺自然下落（不掉头）。
// 被马里奥接触后由 GameModel 置 active=false。header-only，随 GameModel 一起编译。

#ifndef MARIO_MODEL_MUSHROOM_H
#define MARIO_MODEL_MUSHROOM_H

#include <algorithm>

#include "common/EventId.h"       // Direction
#include "model/PhysicsConfig.h"
#include "model/TileMap.h"

class Mushroom {
 public:
  // 出生：默认向右移动。
  void reset(PositionType x, PositionType y, Direction dir = Direction::RIGHT) noexcept {
    x_ = x;
    y_ = y;
    vx_ = (dir == Direction::LEFT ? -1.0f : 1.0f) * mario_cfg::kMushroomSpeed;
    vy_ = 0.0f;
    active_ = true;
  }

  void step(TimeType dt, const TileMap& map) noexcept {
    if (!active_) return;
    // 竖直：重力 + 落地（遇空缺则继续下落，不掉头）
    vy_ = std::min(vy_ + mario_cfg::kGravity * dt, mario_cfg::kMaxFallSpeed);
    y_ += vy_ * dt;
    resolveVertical(map);
    // 水平：匀速 + 撞墙掉头
    x_ += vx_ * dt;
    resolveHorizontal(map);
  }

  void consume() noexcept { active_ = false; }

  bool active() const noexcept { return active_; }
  PositionType x() const noexcept { return x_; }
  PositionType y() const noexcept { return y_; }
  PositionType width() const noexcept { return mario_cfg::kMushroomWidth; }
  PositionType height() const noexcept { return mario_cfg::kMushroomHeight; }

 private:
  static constexpr PositionType kEps = 1e-3f;

  void resolveVertical(const TileMap& map) noexcept {
    const int c0 = map.toCol(x_);
    const int c1 = map.toCol(x_ + width() - kEps);
    if (vy_ > 0.0f) {
      const int row = map.toRow(y_ + height() - kEps);
      for (int c = c0; c <= c1; ++c) {
        if (map.isSolid(c, row)) {
          y_ = row * mario_cfg::kTileSize - height();
          vy_ = 0.0f;
          return;
        }
      }
    }
  }

  void resolveHorizontal(const TileMap& map) noexcept {
    const int r0 = map.toRow(y_);
    const int r1 = map.toRow(y_ + height() - kEps);
    if (vx_ > 0.0f) {
      const int col = map.toCol(x_ + width() - kEps);
      for (int r = r0; r <= r1; ++r) {
        if (map.isSolid(col, r)) {
          x_ = col * mario_cfg::kTileSize - width();
          vx_ = -vx_;
          return;
        }
      }
    } else if (vx_ < 0.0f) {
      const int col = map.toCol(x_);
      for (int r = r0; r <= r1; ++r) {
        if (map.isSolid(col, r)) {
          x_ = (col + 1) * mario_cfg::kTileSize;
          vx_ = -vx_;
          return;
        }
      }
    }
  }

  PositionType x_ = 0.0f;
  PositionType y_ = 0.0f;
  VelocityType vx_ = 0.0f;
  VelocityType vy_ = 0.0f;
  bool active_ = true;
};

#endif  // MARIO_MODEL_MUSHROOM_H
