//

#ifndef MARIO_GAMEMODEL_H
#define MARIO_GAMEMODEL_H

#include "common/EventTrigger.h"

class GameModel {
 public:
  GameModel() noexcept {};
  ~GameModel() noexcept {};

 private:
  EventTrigger viewmodel_Trigger_;
};

#endif  // MARIO_GAMEMODEL_H
