#ifndef MARIO_GAMEMODEL_H
#define MARIO_GAMEMODEL_H

#include <string>
#include <vector>

#include "common/EventId.h"
#include "common/EventTrigger.h"
#include "model/Coin.h"
#include "model/Enemy.h"
#include "model/Mario.h"
#include "model/PhysicsConfig.h"
#include "model/Tile.h"
#include "model/TileMap.h"

class GameModel {
 public:
  GameModel();

  void update(TimeType dt);

  void setMoveLeft(bool on) {
    if (deathInProgress_) return;
    mario_.setMoveLeft(on);
  }
  void setMoveRight(bool on) {
    if (deathInProgress_) return;
    mario_.setMoveRight(on);
  }
  void setMoveStop() {
    if (deathInProgress_) return;
    mario_.stop();
  }
  void jump() {
    if (deathInProgress_) return;
    mario_.jump();
  }

  void reset();
  bool loadLevelFromFile(const std::string& path);
  /// 仅用于单元测试：从字符串加载关卡，不依赖外部文件。
  bool testLoadLevelFromString(const std::string& text);

  PositionType playerX() const { return mario_.x(); }
  PositionType playerY() const { return mario_.y(); }
  PositionType playerW() const { return mario_.width(); }
  PositionType playerH() const { return mario_.height(); }
  MarioState playerState() const { return mario_.state(); }
  Direction playerFacing() const { return mario_.facing(); }

  const std::vector<Tile>& tiles() const { return tiles_; }
  const std::vector<Enemy>& enemies() const { return enemies_; }
  const std::vector<Coin>& coinItems() const { return coinItems_; }  // 地图金币(含已拾取，alive 标记)
  PositionType levelWidthPx() const { return tileMap_.widthPx(); }
  PositionType levelHeightPx() const { return tileMap_.heightPx(); }

  int score() const { return score_; }
  int coins() const { return coins_; }
  int lives() const { return lives_; }
  int timeRemaining() const { return static_cast<int>(std::ceil(std::max(0.0f, timeRemaining_))); }
  std::string world() const { return "1-1"; }

  EventTrigger modelTrigger;

 private:
  void notifyChanged();
  void rebuildTiles();
  void resetLevelState();            // 整关状态重置(不含命数)：reset 与死亡复活共用
  void spawnEnemies();               // 依关卡出生点重建敌人列表
  void spawnCoins();                 // 依关卡出生点重建金币列表
  void collectCoins();               // 马里奥拾取重叠的金币(消失+计数+加分)
  bool resolveEnemyCollisions();     // 处理马里奥-敌人碰撞；返回 true 表示马里奥死亡
  void beginDeath();
  void respawnAfterDeath();

  TileMap tileMap_;
  Mario mario_;
  std::vector<Tile> tiles_;
  std::vector<Enemy> enemies_;
  std::vector<Coin> coinItems_;
  std::string levelFile_;

  int score_ = 0;
  int coins_ = 0;
  int lives_ = 3;
  float timeRemaining_ = 300.0f;
  bool deathInProgress_ = false;
  float deathElapsed_ = 0.0f;
};

#endif  // MARIO_GAMEMODEL_H
