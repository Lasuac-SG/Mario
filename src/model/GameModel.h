#ifndef MARIO_GAMEMODEL_H
#define MARIO_GAMEMODEL_H

#include <string>
#include <vector>

#include "common/EventId.h"
#include "common/EventTrigger.h"
#include "model/Mario.h"
#include "model/PhysicsConfig.h"
#include "model/Tile.h"
#include "model/TileMap.h"

class GameModel {
 public:
  GameModel();

  void update(TimeType dt);

  void setMoveLeft(bool on) {
    if (timeoutDeathInProgress_) return;
    mario_.setMoveLeft(on);
  }
  void setMoveRight(bool on) {
    if (timeoutDeathInProgress_) return;
    mario_.setMoveRight(on);
  }
  void setMoveStop() {
    if (timeoutDeathInProgress_) return;
    mario_.stop();
  }
  void jump() {
    if (timeoutDeathInProgress_) return;
    mario_.jump();
  }

  void reset();
  bool loadLevelFromFile(const std::string& path);

  PositionType playerX() const { return mario_.x(); }
  PositionType playerY() const { return mario_.y(); }
  PositionType playerW() const { return mario_.width(); }
  PositionType playerH() const { return mario_.height(); }
  MarioState playerState() const { return mario_.state(); }
  Direction playerFacing() const { return mario_.facing(); }

  const std::vector<Tile>& tiles() const { return tiles_; }
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
  void beginTimeoutDeath();
  void respawnAfterDeath();

  TileMap tileMap_;
  Mario mario_;
  std::vector<Tile> tiles_;
  std::string levelFile_;

  int score_ = 0;
  int coins_ = 0;
  int lives_ = 3;
  float timeRemaining_ = 300.0f;
  bool timeoutDeathInProgress_ = false;
  float deathElapsed_ = 0.0f;
};

#endif  // MARIO_GAMEMODEL_H
