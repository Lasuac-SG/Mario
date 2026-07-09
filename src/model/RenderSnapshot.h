// View/ViewModel/SFML 无关的纯数据快照。坐标一律为「绝对世界像素坐标」，
// Model 不做任何相机/相对坐标变换——相对（屏幕）坐标由 ViewModel 换算后再交给 View。
// header-only，仅依赖 common 枚举与 model 的数值类型别名。

#ifndef MARIO_MODEL_RENDERSNAPSHOT_H
#define MARIO_MODEL_RENDERSNAPSHOT_H

#include <vector>

#include "common/EventId.h"       // MarioState, Direction, TileType
#include "model/PhysicsConfig.h"  // PositionType

// 马里奥快照（每帧动态）。渲染所需：位置 + 尺寸 + 动画状态 + 朝向。
struct MarioView {
  PositionType x = 0.f;  // 左上角绝对像素坐标（世界坐标）
  PositionType y = 0.f;
  PositionType width = 0.f;
  PositionType height = 0.f;
  MarioState state = MarioState::IDLE;  // 选动画帧（IDLE/RUNNING/JUMPING/FALLING）
  Direction facing = Direction::RIGHT;  // 精灵翻转方向
};

// 关卡快照（仅在加载/重置时变化）。
struct LevelView {
  int cols = 0;
  int rows = 0;
  PositionType tileSize = 0.f;
  PositionType widthPx = 0.f;   // 关卡整体绝对尺寸，供 ViewModel 计算相机/视口边界
  PositionType heightPx = 0.f;
  std::vector<TileType> tiles;  // 行主序：tiles[row * cols + col]
};

#endif  // MARIO_MODEL_RENDERSNAPSHOT_H
