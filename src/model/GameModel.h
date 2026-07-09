// Model 层门面：组合关卡(TileMap)与马里奥(Mario)，实现接口文档约定的 Model 契约。
//
// 依赖方向：Model → common（单向）。不认识 ViewModel/View，仅通过 modelTrigger 被动地被订阅。
// 对外数据均为「绝对世界像素坐标」，不含任何相机/相对变换——相对坐标由 ViewModel 换算。
//   - 通知：modelTrigger 在状态变化时 fire(STATE_CHANGED)，ViewModel 订阅后重绘。
//   - 数据：playerX/Y/W/H、playerState/Facing、tiles()、levelWidthPx/HeightPx，供 ViewModel 拉取。
//
// 不引入 SFML，实现细节见 GameModel.cpp。

#ifndef MARIO_GAMEMODEL_H
#define MARIO_GAMEMODEL_H

#include <string>
#include <vector>

#include "common/EventId.h"       // MarioState, Direction
#include "common/EventTrigger.h"  // EventTrigger
#include "model/Mario.h"
#include "model/PhysicsConfig.h"  // PositionType, TimeType
#include "model/Tile.h"
#include "model/TileMap.h"

class GameModel {
 public:
  GameModel();

  // === 游戏驱动（ViewModel::tick → 此处）===
  void update(TimeType dt);

  // === 输入（InputCommand → 此处，只改意图，下一帧 update 生效；内联转发）===
  void setMoveLeft(bool on) { mario_.setMoveLeft(on); }
  void setMoveRight(bool on) { mario_.setMoveRight(on); }
  void setMoveStop() { mario_.stop(); }
  void jump() { mario_.jump(); }

  // 重置：重载关卡（曾指定关卡文件则重读，失败保留原地图）+ 归位 + 重建瓦片 + 通知。
  void reset();

  // 可选（非契约必需）：切换关卡文件，立即重载并归位。
  bool loadLevelFromFile(const std::string& path);

  // === 玩家数据（绝对像素坐标；供 ViewModel 换算相对坐标后交给 View）===
  PositionType playerX() const { return mario_.x(); }
  PositionType playerY() const { return mario_.y(); }
  PositionType playerW() const { return mario_.width(); }
  PositionType playerH() const { return mario_.height(); }
  MarioState playerState() const { return mario_.state(); }
  Direction playerFacing() const { return mario_.facing(); }

  // === 关卡数据（绝对像素坐标）===
  const std::vector<Tile>& tiles() const { return tiles_; }  // 实心瓦片列表
  PositionType levelWidthPx() const { return tileMap_.widthPx(); }
  PositionType levelHeightPx() const { return tileMap_.heightPx(); }

  // === 事件（ViewModel 订阅；EventTrigger 本身即 std::function 观察者）===
  EventTrigger modelTrigger;

 private:
  void notifyChanged();
  void rebuildTiles();  // 从 TileMap 重建实心瓦片缓存（仅关卡变化时调用）

  TileMap tileMap_;
  Mario mario_;
  std::vector<Tile> tiles_;  // 缓存的实心瓦片（绝对像素），供 tiles() 返回 const&
  std::string levelFile_;    // 空 = 使用内置默认关卡
};

#endif  // MARIO_GAMEMODEL_H
