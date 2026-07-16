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
// 可变跳跃：松开跳键时若仍在上升，把上升速度截断到此上限（越早松开=越接近此值=跳得越低）。
// 取起跳初速度的一部分，保证最短点跳仍有约 1 格高度；按住不放则保留全速直到顶点=最高跳。
inline constexpr VelocityType kJumpCutSpeed = kJumpSpeed * 0.45f;

// 数值稳定：物理子步长上限；单帧 dt 超过则切分为多步，避免高速穿透。
inline constexpr TimeType kMaxStep = 1.0f / 120.0f;

// 敌人（棕蘑菇）：高 = 马里奥一半；宽略窄；水平匀速巡逻，不跳。
inline constexpr PositionType kEnemyWidth = 24.0f;
inline constexpr PositionType kEnemyHeight = kMarioHeight * 0.5f;  // 15
inline constexpr VelocityType kEnemyMoveSpeed = 90.0f;             // 巡逻速度
inline constexpr VelocityType kStompBounceSpeed = kJumpSpeed * 0.5f;  // 踩死后马里奥小反弹
inline constexpr int kStompScore = 100;                            // 踩死加分

// 金币：静态可拾取物，碰到即消失并加分。
inline constexpr PositionType kCoinWidth = 20.0f;
inline constexpr PositionType kCoinHeight = 20.0f;
inline constexpr int kCoinScore = 200;  // 每枚金币加分

// 大马里奥（吃变大蘑菇后）碰撞盒：更高，脚底不变、向上长高。
inline constexpr PositionType kMarioBigWidth = 28.0f;
inline constexpr PositionType kMarioBigHeight = 60.0f;
inline constexpr TimeType kInvincibleSeconds = 1.5f;  // 变小后短暂无敌

// 变大蘑菇道具：从问号块顶出后向右移动、受重力、遇墙掉头、遇空缺下落。
inline constexpr PositionType kMushroomWidth = 28.0f;
inline constexpr PositionType kMushroomHeight = 28.0f;
inline constexpr VelocityType kMushroomSpeed = 70.0f;

inline constexpr int kMushroomScore = 1000;  // 吃到变大蘑菇加分
inline constexpr int kBrickScore = 50;       // 大马里奥砸碎砖块加分

}  // namespace mario_cfg

#endif  // MARIO_MODEL_PHYSICSCONFIG_H
