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

  void setMoveLeft(bool on) {
    if (deathInProgress_ || goalReached_) return;
    mario_.setMoveLeft(on);
  }
  void setMoveRight(bool on) {
    if (deathInProgress_ || goalReached_) return;
    mario_.setMoveRight(on);
  }
  void setMoveStop() {
    if (deathInProgress_ || goalReached_) return;
    mario_.stop();
  }
  void jump() {
    if (deathInProgress_ || goalReached_) return;
    if (mario_.jump()) notifyChanged(Event::MARIO_JUMPED);  // 真正离地才上报(供跳跃音效)
  }

  void reset();
  bool loadLevelFromFile(const std::string& path);
  bool testLoadLevelFromString(const std::string& text);

  PositionType playerX() const { return mario_.x(); }
  PositionType playerY() const { return mario_.y(); }
  PositionType playerW() const { return mario_.width(); }
  PositionType playerH() const { return mario_.height(); }
  MarioState playerState() const { return mario_.state(); }
  Direction playerFacing() const { return mario_.facing(); }

  const std::vector<Tile>& tiles() const { return tiles_; }
  const std::vector<Enemy>& enemies() const { return enemies_; }
  const std::vector<Coin>& coinItems() const { return coinItems_; }
  const std::vector<Mushroom>& mushrooms() const { return mushrooms_; }
  bool playerBig() const { return mario_.big(); }
  PositionType levelWidthPx() const { return tileMap_.widthPx(); }
  PositionType levelHeightPx() const { return tileMap_.heightPx(); }

  int score() const { return score_; }
  int coins() const { return coins_; }
  int lives() const { return lives_; }
  int timeRemaining() const { return static_cast<int>(std::ceil(std::max(0.0f, timeRemaining_))); }
  std::string world() const { return "1-1"; }

  PositionType goalX() const { return goalX_; }
  PositionType goalY() const { return goalY_; }
  PositionType goalW() const { return goalW_; }
  PositionType goalH() const { return goalH_; }
  bool goalReached() const { return goalReached_; }
  bool won() const { return goalReached_; }

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
  bool checkGoalReached() const;
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
};

#endif  // MARIO_GAMEMODEL_H
