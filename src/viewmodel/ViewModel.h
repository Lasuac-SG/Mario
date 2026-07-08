//

#ifndef MARIO_VIEWMODEL_H
#define MARIO_VIEWMODEL_H

#include "common/EventTrigger.h"
class ViewModel {
 public:
  ViewModel() noexcept {}
  ~ViewModel() noexcept {};

 private:
  EventTrigger view_Trigger_;
};

#endif  // MARIO_VIEWMODEL_H
