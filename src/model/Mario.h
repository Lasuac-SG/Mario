#ifndef MARIO_MODEL_MARIO_H
#define MARIO_MODEL_MARIO_H

#include "common/EventId.h"
#include "model/PhysicsConfig.h"

class TileMap;

class Mario {
 public:
  void reset(PositionType x, PositionType y);
  void jump();
  void step(TimeType dt, const TileMap& map);
  void startDeathFall();

  // 踩死敌人后的小反弹（GameModel 调用）。
  void bounce(VelocityType speed) {
    if (deathFalling_) return;
    vy_ = -speed;
    onGround_ = false;
  }

  void setMoveLeft(bool on) {
    if (deathFalling_) return;
    moveLeft_ = on;
    if (on) moveRight_ = false;
  }
  void setMoveRight(bool on) {
    if (deathFalling_) return;
    moveRight_ = on;
    if (on) moveLeft_ = false;
  }
  void stop() {
    moveLeft_ = false;
    moveRight_ = false;
  }

  PositionType x() const { return x_; }
  PositionType y() const { return y_; }
  VelocityType vy() const { return vy_; }
  PositionType width() const { return mario_cfg::kMarioWidth; }
  PositionType height() const { return mario_cfg::kMarioHeight; }
  MarioState state() const { return state_; }
  Direction facing() const { return facing_; }

 private:
  void applyHorizontal(TimeType dt);
  void applyGravity(TimeType dt);
  void resolveHorizontal(const TileMap& map);
  void resolveVertical(const TileMap& map);
  void updateState();

  static constexpr PositionType kEps = 1e-3f;

  PositionType x_ = 0.f;
  PositionType y_ = 0.f;
  VelocityType vx_ = 0.f;
  VelocityType vy_ = 0.f;
  bool onGround_ = false;
  Direction facing_ = Direction::RIGHT;
  MarioState state_ = MarioState::IDLE;
  bool deathFalling_ = false;

  bool moveLeft_ = false;
  bool moveRight_ = false;
};

#endif  // MARIO_MODEL_MARIO_H
