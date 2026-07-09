// delclare PropertyTrigger for function call-back

#ifndef MARIO_EVENTTRIGGER_H_BACK
#define MARIO_EVENTTRIGGER_H_BACK

// TODO: 这里的eventId应该使用枚举类进行明确, 但是这里类型应该怎么说?
using EventType = uint32_t;
using NotificationFunction = std::function<void(EventType)>;

class EventTrigger {
 public:
  EventTrigger() noexcept {}
  ~EventTrigger() noexcept {}
  EventTrigger& operator=(const EventTrigger&) = delete;
  void clear_notificationList() noexcept;
  uintptr_t add_notification(NotificationFunction&& pn);
  // 这里移除的逻辑不清楚
  void remove_notification(uintptr_t notificationFunctionIndex);

 public:
  void fire(EventType) noexcept;

 private:
  std::vector<NotificationFunction> notificationList_;
};

#endif  // MARIO_EVENTTRIGGER_H_BACK
