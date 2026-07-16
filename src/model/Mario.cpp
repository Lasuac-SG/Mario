#include "model/Mario.h"

#include <algorithm>
#include <cmath>

#include "model/TileMap.h"

void Mario::reset(PositionType x, PositionType y) noexcept {
  x_ = x;
  y_ = y;
  vx_ = 0.f;
  vy_ = 0.f;
  onGround_ = false;
  facing_ = Direction::RIGHT;
  state_ = MarioState::IDLE;
  deathFalling_ = false;
  big_ = false;
  invincible_ = 0.f;
  headBump_ = false;
  headBumpRow_ = -1;
  moveLeft_ = false;
  moveRight_ = false;
}

bool Mario::jump() noexcept {
  if (!deathFalling_ && onGround_) {
    vy_ = -mario_cfg::kJumpSpeed;
    onGround_ = false;
    return true;
  }
  return false;
}

void Mario::endJump() noexcept {
  // 仅在上升阶段生效：把向上速度截断到 kJumpCutSpeed。松开越早、剩余上升速度越大 → 被截得越多 → 跳得越低；
  // 按住不放直到接近顶点(vy_ 已自然衰减到小于该阈值)则不再截断 → 保留最高跳。死亡下落不受影响。
  if (deathFalling_) return;
  if (vy_ < -mario_cfg::kJumpCutSpeed) vy_ = -mario_cfg::kJumpCutSpeed;
}

void Mario::startDeathFall() noexcept {
  stop();
  deathFalling_ = true;
  onGround_ = false;
  vx_ = 0.f;
  vy_ = -mario_cfg::kJumpSpeed * 0.75f;
  state_ = MarioState::DEAD;
}

void Mario::step(TimeType dt, const TileMap& map) noexcept {
  headBump_ = false;                              // 每步重置头顶碰撞标记
  if (invincible_ > 0.f) invincible_ -= dt;       // 无敌计时衰减

  if (deathFalling_) {
    applyGravity(dt);
    y_ += vy_ * dt;
    state_ = MarioState::DEAD;
    return;
  }

  applyHorizontal(dt);
  applyGravity(dt);

  x_ += vx_ * dt;
  resolveHorizontal(map);
  y_ += vy_ * dt;
  resolveVertical(map);

  updateState();
}

void Mario::applyHorizontal(TimeType dt) noexcept {
  if (moveLeft_) {
    vx_ = std::max(vx_ - mario_cfg::kMoveAccel * dt, -mario_cfg::kMoveMaxSpeed);
    facing_ = Direction::LEFT;
  } else if (moveRight_) {
    vx_ = std::min(vx_ + mario_cfg::kMoveAccel * dt, mario_cfg::kMoveMaxSpeed);
    facing_ = Direction::RIGHT;
  } else {
    const VelocityType fric = (onGround_ ? mario_cfg::kGroundFriction : mario_cfg::kAirFriction) * dt;
    if (vx_ > 0.f) {
      vx_ = std::max(0.f, vx_ - fric);
    } else if (vx_ < 0.f) {
      vx_ = std::min(0.f, vx_ + fric);
    }
  }
}

void Mario::applyGravity(TimeType dt) noexcept {
  vy_ = std::min(vy_ + mario_cfg::kGravity * dt, mario_cfg::kMaxFallSpeed);
}

void Mario::resolveHorizontal(const TileMap& map) noexcept {
  const int r0 = map.toRow(y_);
  const int r1 = map.toRow(y_ + height() - kEps);
  if (vx_ > 0.f) {
    const int col = map.toCol(x_ + width() - kEps);
    for (int r = r0; r <= r1; ++r) {
      if (map.isSolid(col, r)) {
        x_ = col * mario_cfg::kTileSize - width();
        vx_ = 0.f;
        break;
      }
    }
  } else if (vx_ < 0.f) {
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

void Mario::resolveVertical(const TileMap& map) noexcept {
  const int c0 = map.toCol(x_);
  const int c1 = map.toCol(x_ + width() - kEps);
  if (vy_ > 0.f) {
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
  } else if (vy_ < 0.f) {
    const int row = map.toRow(y_);
    for (int c = c0; c <= c1; ++c) {
      if (map.isSolid(c, row)) {
        y_ = (row + 1) * mario_cfg::kTileSize;
        vy_ = 0.f;
        headBump_ = true;      // 记录顶到天花板
        headBumpRow_ = row;
        return;
      }
    }
    onGround_ = false;
  }
}

void Mario::updateState() noexcept {
  if (deathFalling_) {
    state_ = MarioState::DEAD;
    return;
  }

  if (!onGround_) {
    state_ = (vy_ < 0.f) ? MarioState::JUMPING : MarioState::FALLING;
  } else {
    state_ = (std::fabs(vx_) > 1.0f) ? MarioState::RUNNING : MarioState::IDLE;
  }
}
