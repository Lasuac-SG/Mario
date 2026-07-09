//
#include "common/EventTrigger.h"

void EventTrigger::clear_notificationList() noexcept {
  notificationList_.clear();
}

// TODO 这里返回index还是index+1, 这里使用的dirty block的逻辑
uintptr_t EventTrigger::add_notification(Notify_Funtion&& func_ptr) noexcept {
  uintptr_t index = 0;
  for (auto& fn : notificationList_) {
    if (fn == nullptr) {
      fn = std::move(func_ptr);
      return index;
    }
    index++;
  }
  notificationList_.push_back(std::move(func_ptr));
  return index;
}

void EventTrigger::remove_notification(uintptr_t notificationFunctionIndex) noexcept {
  assert(notificationFunctionIndex < notificationList_.size());
  notificationList_[notificationFunctionIndex] = nullptr;
}

void EventTrigger::fire(EventType eventId) noexcept {
  for (const auto&func : notificationList_) {
    if (func) {
      func(eventId);
    }
  }
}
