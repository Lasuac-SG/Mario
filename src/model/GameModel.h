// Model 层门面：组合关卡(TileMap)与马里奥(Mario)，实现 ViewModel 约定的契约。
//
// 对上层的数据出口（均为「绝对世界像素坐标」，不含任何相机/相对变换）：
//   - 通知：modelTrigger 在状态变化时 fire(STATE_CHANGED)，ViewModel 订阅后重绘。
//   - 数据：marioView() / levelView() 返回框架无关的纯数据快照，供 ViewModel 拉取。
// ViewModel 拿到绝对坐标后自行换算成相对（屏幕）坐标再交给 View，从而解耦 Model↔View。
//
// 不引入 SFML，实现细节见 GameModel.cpp。

#ifndef MARIO_GAMEMODEL_H
#define MARIO_GAMEMODEL_H

#include <string>

#include "common/EventTrigger.h"   // EventTrigger
#include "model/Mario.h"
#include "model/PhysicsConfig.h"   // TimeType
#include "model/RenderSnapshot.h"  // MarioView, LevelView
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

  // 重置：重载关卡（曾指定关卡文件则重读，失败保留原地图），马里奥回出生点。
  void reset();

  // 可选（非契约必需）：切换关卡文件，立即重载并归位；成功记录路径供 reset 复用。
  bool loadLevelFromFile(const std::string& path);

  // === 数据快照（绝对世界像素坐标，供 ViewModel 拉取后换算相对坐标）===
  MarioView marioView() const;  // 每帧动态，逐帧拉取
  LevelView levelView() const;  // 仅 reset/加载时变化，可缓存

  // === 事件（ViewModel 订阅；EventTrigger 本身即 std::function 观察者）===
  EventTrigger modelTrigger;

 private:
  void notifyChanged();

  TileMap tileMap_;
  Mario mario_;
  std::string levelFile_;  // 空 = 使用内置默认关卡
};

#endif  // MARIO_GAMEMODEL_H
