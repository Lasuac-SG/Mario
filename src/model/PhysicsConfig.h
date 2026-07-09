// 物理与网格常量 + 语义化数值类型别名：集中于此便于统一调参。
// 全部为 namespace 作用域的 constexpr（内部链接），header-only 多处包含无 ODR 问题。

#ifndef MARIO_MODEL_PHYSICSCONFIG_H
#define MARIO_MODEL_PHYSICSCONFIG_H

#include "common/Type.h"  // 复用 PositionType

// 语义化类型别名（与 common/Type.h 的 PositionType 同风格），提高可读性。
using VelocityType = float;      // 速度：像素/秒
using AccelerationType = float;  // 加速度：像素/秒^2
using TimeType = float;          // 时间：秒（如帧间隔 dt）

namespace mario_cfg {

// 瓦片像素尺寸（正方形）。像素坐标 / kTileSize 即瓦片格坐标。
inline constexpr PositionType kTileSize = 32.0f;

// 马里奥碰撞盒尺寸（像素），略小于一格以便通过窄缝。
inline constexpr PositionType kMarioWidth = 28.0f;
inline constexpr PositionType kMarioHeight = 30.0f;

// 水平运动。
inline constexpr VelocityType kMoveMaxSpeed = 300.0f;        // 最大水平速度
inline constexpr AccelerationType kMoveAccel = 1800.0f;      // 有方向输入时的加速度
inline constexpr AccelerationType kGroundFriction = 2000.0f; // 无输入时地面减速度
inline constexpr AccelerationType kAirFriction = 400.0f;     // 无输入时空中减速度

// 竖直运动（向下为正）。
inline constexpr AccelerationType kGravity = 2200.0f;   // 重力加速度
inline constexpr VelocityType kJumpSpeed = 760.0f;      // 起跳初速度（正值，施加时取负号向上）
inline constexpr VelocityType kMaxFallSpeed = 1200.0f;  // 终端下落速度

// 数值稳定：物理子步长上限；单帧 dt 超过则切分为多步，避免高速穿透。
inline constexpr TimeType kMaxStep = 1.0f / 120.0f;

}  // namespace mario_cfg

#endif  // MARIO_MODEL_PHYSICSCONFIG_H
