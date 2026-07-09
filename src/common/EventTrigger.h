// delclare PropertyTrigger for function call-back

#ifndef MARIO_EVENTTRIGGER_H_BACK
#define MARIO_EVENTTRIGGER_H_BACK

#include "common/Type.h"

class EventTrigger {
public:
  EventTrigger() noexcept {}
  ~EventTrigger() noexcept {}
  EventTrigger& operator=(const EventTrigger&) = delete;
  void clear_notificationList() noexcept;
  uintptr_t add_notification(Notify_Funtion&& func_ptr) noexcept;
  void remove_notification(uintptr_t notificationFunctionIndex) noexcept;

public:
  void fire(EventType) noexcept;

private:
  std::vector<Notify_Funtion> notificationList_;
};

#endif  // MARIO_EVENTTRIGGER_H_BACK
