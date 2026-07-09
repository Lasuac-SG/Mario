// 实心瓦片（绝对世界像素坐标）。Model 组装缓存，ViewModel 读取后换算为相对坐标的 TileView。
// 是 Model 与 ViewModel 之间的数据载体，不被 View 直接使用。

#ifndef MARIO_MODEL_TILE_H
#define MARIO_MODEL_TILE_H

#include "common/EventId.h"  // TileType
#include "common/Type.h"     // PositionType

struct Tile {
  PositionType x = 0.f;  // 左上角绝对像素坐标（世界坐标）
  PositionType y = 0.f;
  PositionType w = 0.f;
  PositionType h = 0.f;
  TileType type = TileType::EMPTY;
};

#endif  // MARIO_MODEL_TILE_H
