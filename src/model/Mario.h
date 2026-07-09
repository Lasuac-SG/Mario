// 马里奥实体：以像素坐标维护物理状态；碰撞检测时借助 TileMap 转换到瓦片格坐标。纯逻辑，无渲染。

#ifndef MARIO_MODEL_MARIO_H
#define MARIO_MODEL_MARIO_H

#include "common/EventId.h"       // MarioState, Direction
#include "model/PhysicsConfig.h"  // PositionType, VelocityType, TimeType

class TileMap;  // 仅按引用使用，前置声明即可（定义见 Mario.cpp）

class Mario {
 public:
  // 重置到指定像素坐标（左上角），清空速度与输入意图。
  void reset(PositionType x, PositionType y);

  // 起跳：仅在着地时生效。
  void jump();

  // 单个固定子步长推进：加速度 → 分轴移动与碰撞解算 → 状态刷新。
  void step(TimeType dt, const TileMap& map);

  // === 输入意图（同一时刻只保留一个方向；轻量，内联）===
  void setMoveLeft(bool on) {
    moveLeft_ = on;
    if (on) moveRight_ = false;
  }
  void setMoveRight(bool on) {
    moveRight_ = on;
    if (on) moveLeft_ = false;
  }
  void stop() {
    moveLeft_ = false;
    moveRight_ = false;
  }

  // === 只读访问（供 GameModel 的 player* getter 转发；轻量，内联）===
  PositionType x() const { return x_; }
  PositionType y() const { return y_; }
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

  // AABB 覆盖瓦片格范围时的极小内缩，避免正好贴边多算一格。
  static constexpr PositionType kEps = 1e-3f;

  // 物理状态（像素坐标，x_/y_ 为左上角）。
  PositionType x_ = 0.f;
  PositionType y_ = 0.f;
  VelocityType vx_ = 0.f;
  VelocityType vy_ = 0.f;
  bool onGround_ = false;
  Direction facing_ = Direction::RIGHT;
  MarioState state_ = MarioState::IDLE;

  // 输入意图。
  bool moveLeft_ = false;
  bool moveRight_ = false;
};

#endif  // MARIO_MODEL_MARIO_H
