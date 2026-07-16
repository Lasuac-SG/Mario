#ifndef MARIO_MODEL_MARIO_H
#define MARIO_MODEL_MARIO_H

#include "common/EventId.h"
#include "model/PhysicsConfig.h"

class TileMap;

class Mario {
 public:
  void reset(PositionType x, PositionType y) noexcept;
  bool jump() noexcept;  // 返回是否真的起跳(着地且非死亡态才成立)
  void endJump() noexcept;  // 松开跳键：若仍在上升则截断上升速度，实现"按得越久跳得越高"
  void step(TimeType dt, const TileMap& map) noexcept;
  void startDeathFall() noexcept;

  // 踩死敌人后的小反弹（GameModel 调用）。
  void bounce(VelocityType speed) noexcept {
    if (deathFalling_) return;
    vy_ = -speed;
    onGround_ = false;
  }

  void setMoveLeft(bool on) noexcept {
    if (deathFalling_) return;
    moveLeft_ = on;
    if (on) moveRight_ = false;
  }
  void setMoveRight(bool on) noexcept {
    if (deathFalling_) return;
    moveRight_ = on;
    if (on) moveLeft_ = false;
  }
  void stop() noexcept {
    moveLeft_ = false;
    moveRight_ = false;
  }

  // 变大/变小（吃蘑菇 / 碰敌）。脚底位置不变，尺寸向上增减；变小后短暂无敌。
  void grow() noexcept {
    if (big_) return;  // 已变大再吃无效果
    y_ -= (mario_cfg::kMarioBigHeight - mario_cfg::kMarioHeight);
    big_ = true;
  }
  void shrink() noexcept {
    if (!big_) return;
    y_ += (mario_cfg::kMarioBigHeight - mario_cfg::kMarioHeight);
    big_ = false;
    invincible_ = mario_cfg::kInvincibleSeconds;
  }

  PositionType x() const noexcept { return x_; }
  PositionType y() const noexcept { return y_; }
  VelocityType vy() const noexcept { return vy_; }
  PositionType width() const noexcept { return big_ ? mario_cfg::kMarioBigWidth : mario_cfg::kMarioWidth; }
  PositionType height() const noexcept { return big_ ? mario_cfg::kMarioBigHeight : mario_cfg::kMarioHeight; }
  MarioState state() const noexcept { return state_; }
  Direction facing() const noexcept { return facing_; }
  bool big() const noexcept { return big_; }
  bool invincible() const noexcept { return invincible_ > 0.f; }

  // 本步是否用头顶到了天花板，以及天花板所在瓦片行（供 GameModel 判定顶问号块/砸砖块）。
  bool headBumped() const noexcept { return headBump_; }
  int headBumpRow() const noexcept { return headBumpRow_; }

 private:
  void applyHorizontal(TimeType dt) noexcept;
  void applyGravity(TimeType dt) noexcept;
  void resolveHorizontal(const TileMap& map) noexcept;
  void resolveVertical(const TileMap& map) noexcept;
  void updateState() noexcept;

  static constexpr PositionType kEps = 1e-3f;

  PositionType x_ = 0.f;
  PositionType y_ = 0.f;
  VelocityType vx_ = 0.f;
  VelocityType vy_ = 0.f;
  bool onGround_ = false;
  Direction facing_ = Direction::RIGHT;
  MarioState state_ = MarioState::IDLE;
  bool deathFalling_ = false;
  bool big_ = false;
  TimeType invincible_ = 0.f;
  bool headBump_ = false;
  int headBumpRow_ = -1;

  bool moveLeft_ = false;
  bool moveRight_ = false;
};

#endif  // MARIO_MODEL_MARIO_H
