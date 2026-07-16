// 金币：地图上的静态可拾取物（绝对世界像素坐标）。无行为，纯数据；被拾取后 alive=false。
// header-only（随 GameModel 一起编译，不新增编译单元）。

#ifndef MARIO_MODEL_COIN_H
#define MARIO_MODEL_COIN_H

#include "common/Type.h"  // PositionType

struct Coin {
  PositionType x = 0.0f;
  PositionType y = 0.0f;
  PositionType w = 0.0f;
  PositionType h = 0.0f;
  bool alive = true;
};

#endif  // MARIO_MODEL_COIN_H
