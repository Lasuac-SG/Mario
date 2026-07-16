#ifndef MARIO_GAMEMODEL_H
#define MARIO_GAMEMODEL_H

#include <cmath>
#include <string>
#include <unordered_set>
#include <vector>

#include "common/EventId.h"
#include "common/EventTrigger.h"
#include "model/Coin.h"
#include "model/Enemy.h"
#include "model/Mario.h"
#include "model/Mushroom.h"
#include "model/PhysicsConfig.h"
#include "model/Tile.h"
#include "model/TileMap.h"

class GameModel {
 public:
  GameModel();

  void update(TimeType dt);

  void setMoveLeft(bool on) noexcept {
    if (deathInProgress_ || goalReached_ || gameOver_) return;
    mario_.setMoveLeft(on);
  }
  void setMoveRight(bool on) noexcept {
    if (deathInProgress_ || goalReached_ || gameOver_) return;
    mario_.setMoveRight(on);
  }
  void setMoveStop() noexcept {
    if (deathInProgress_ || goalReached_ || gameOver_) return;
    mario_.stop();
  }
  void jump() {
    if (deathInProgress_ || goalReached_ || gameOver_) return;
    if (mario_.jump()) notifyChanged(Event::MARIO_JUMPED);  // 真正离地才上报(供跳跃音效)
  }
  void endJump() noexcept {  // 松开跳键：可变跳跃高度(松键截断)。仅调速度，无通知/分配，故 noexcept
    if (deathInProgress_ || goalReached_ || gameOver_) return;
    mario_.endJump();
  }

  void reset();
  void startGame(int mapId);  // 从开始菜单选择地图(1/2)后载入对应关卡并开始
  bool loadLevelFromFile(const std::string& path);
  bool testLoadLevelFromString(const std::string& text);

  PositionType playerX() const noexcept { return mario_.x(); }
  PositionType playerY() const noexcept { return mario_.y(); }
  PositionType playerW() const noexcept { return mario_.width(); }
  PositionType playerH() const noexcept { return mario_.height(); }
  MarioState playerState() const noexcept { return mario_.state(); }
  Direction playerFacing() const noexcept { return mario_.facing(); }

  const std::vector<Tile>& tiles() const noexcept { return tiles_; }
  const std::vector<Enemy>& enemies() const noexcept { return enemies_; }
  const std::vector<Coin>& coinItems() const noexcept { return coinItems_; }
  const std::vector<Mushroom>& mushrooms() const noexcept { return mushrooms_; }
  bool playerBig() const noexcept { return mario_.big(); }
  PositionType levelWidthPx() const noexcept { return tileMap_.widthPx(); }
  PositionType levelHeightPx() const noexcept { return tileMap_.heightPx(); }

  int score() const noexcept { return score_; }
  int coins() const noexcept { return coins_; }
  int lives() const noexcept { return lives_; }
  int timeRemaining() const noexcept { return static_cast<int>(std::ceil(std::max(0.0f, timeRemaining_))); }
  std::string world() const { return "1-1"; }

  PositionType goalX() const noexcept { return goalX_; }
  PositionType goalY() const noexcept { return goalY_; }
  PositionType goalW() const noexcept { return goalW_; }
  PositionType goalH() const noexcept { return goalH_; }
  bool goalReached() const noexcept { return goalReached_; }
  bool won() const noexcept { return goalReached_; }
  bool gameOver() const noexcept { return gameOver_; }  // 命数耗尽：View 据此播放 Game Over 动画/显示重开按钮

  EventTrigger modelTrigger;

 private:
  void notifyChanged(Event ev = Event::STATE_CHANGED);  // 细粒度事件；默认为常规帧更新
  void rebuildTiles();
  void resetLevelState();
  void spawnEnemies();
  void spawnCoins();
  void setupGoal();
  void collectCoins();
  void handleBlockBump();
  void spawnMushroomAt(int col, int row);
  void collectMushrooms();
  bool resolveEnemyCollisions();
  bool checkGoalReached() const noexcept;
  void beginGoalClear();
  void beginDeath();
  void respawnAfterDeath();

  TileMap tileMap_;
  Mario mario_;
  std::vector<Tile> tiles_;
  std::vector<Enemy> enemies_;
  std::vector<Coin> coinItems_;
  std::vector<Mushroom> mushrooms_;
  std::unordered_set<int> usedQuestions_;
  std::string levelFile_;

  int score_ = 0;
  int coins_ = 0;
  int lives_ = 3;
  float timeRemaining_ = 300.0f;
  bool deathInProgress_ = false;
  float deathElapsed_ = 0.0f;

  PositionType goalX_ = 0.0f;
  PositionType goalY_ = 0.0f;
  PositionType goalW_ = 24.0f;
  PositionType goalH_ = 160.0f;
  bool goalReached_ = false;
  bool gameOver_ = false;
};

#endif  // MARIO_GAMEMODEL_H
