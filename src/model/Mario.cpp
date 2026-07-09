#include "model/Mario.h"

#include <algorithm>
#include <cmath>

#include "model/TileMap.h"

void Mario::reset(PositionType x, PositionType y) {
  x_ = x;
  y_ = y;
  vx_ = 0.f;
  vy_ = 0.f;
  onGround_ = false;
  facing_ = Direction::RIGHT;
  state_ = MarioState::IDLE;
  moveLeft_ = false;
  moveRight_ = false;
}

void Mario::jump() {
  if (onGround_) {
    vy_ = -mario_cfg::kJumpSpeed;
    onGround_ = false;
  }
}

void Mario::step(TimeType dt, const TileMap& map) {
  applyHorizontal(dt);
  applyGravity(dt);

  // 分轴解算：先水平后竖直，避免卡墙/穿地。
  x_ += vx_ * dt;
  resolveHorizontal(map);
  y_ += vy_ * dt;
  resolveVertical(map);

  updateState();
}

void Mario::applyHorizontal(TimeType dt) {
  if (moveLeft_) {
    vx_ = std::max(vx_ - mario_cfg::kMoveAccel * dt, -mario_cfg::kMoveMaxSpeed);
    facing_ = Direction::LEFT;
  } else if (moveRight_) {
    vx_ = std::min(vx_ + mario_cfg::kMoveAccel * dt, mario_cfg::kMoveMaxSpeed);
    facing_ = Direction::RIGHT;
  } else {
    // 无输入：按摩擦减速到 0。
    const VelocityType fric = (onGround_ ? mario_cfg::kGroundFriction : mario_cfg::kAirFriction) * dt;
    if (vx_ > 0.f) {
      vx_ = std::max(0.f, vx_ - fric);
    } else if (vx_ < 0.f) {
      vx_ = std::min(0.f, vx_ + fric);
    }
  }
}

void Mario::applyGravity(TimeType dt) {
  vy_ = std::min(vy_ + mario_cfg::kGravity * dt, mario_cfg::kMaxFallSpeed);
}

void Mario::resolveHorizontal(const TileMap& map) {
  const int r0 = map.toRow(y_);
  const int r1 = map.toRow(y_ + height() - kEps);
  if (vx_ > 0.f) {  // 向右：检测右边界所在列
    const int col = map.toCol(x_ + width() - kEps);
    for (int r = r0; r <= r1; ++r) {
      if (map.isSolid(col, r)) {
        x_ = col * mario_cfg::kTileSize - width();
        vx_ = 0.f;
        break;
      }
    }
  } else if (vx_ < 0.f) {  // 向左：检测左边界所在列
    const int col = map.toCol(x_);
    for (int r = r0; r <= r1; ++r) {
      if (map.isSolid(col, r)) {
        x_ = (col + 1) * mario_cfg::kTileSize;
        vx_ = 0.f;
        break;
      }
    }
  }
}

void Mario::resolveVertical(const TileMap& map) {
  const int c0 = map.toCol(x_);
  const int c1 = map.toCol(x_ + width() - kEps);
  if (vy_ > 0.f) {  // 下落：检测脚下所在行
    const int row = map.toRow(y_ + height() - kEps);
    for (int c = c0; c <= c1; ++c) {
      if (map.isSolid(c, row)) {
        y_ = row * mario_cfg::kTileSize - height();
        vy_ = 0.f;
        onGround_ = true;
        return;
      }
    }
    onGround_ = false;
  } else if (vy_ < 0.f) {  // 上升：检测头顶所在行
    const int row = map.toRow(y_);
    for (int c = c0; c <= c1; ++c) {
      if (map.isSolid(c, row)) {
        y_ = (row + 1) * mario_cfg::kTileSize;
        vy_ = 0.f;
        return;
      }
    }
    onGround_ = false;
  }
}

void Mario::updateState() {
  if (!onGround_) {
    state_ = (vy_ < 0.f) ? MarioState::JUMPING : MarioState::FALLING;
  } else {
    state_ = (std::fabs(vx_) > 1.0f) ? MarioState::RUNNING : MarioState::IDLE;
  }
}
