// EventTrigger 单元测试：覆盖注册、触发、移除、槽位复用等基本行为。

#include "common/EventTrigger.h"

#include <gtest/gtest.h>

// ── 基础注册 + 触发 ──────────────────────────────────────────

TEST(EventTriggerTest, AddAndFire) {
    EventTrigger trigger;
    int called = 0;
    EventType lastEvent = 0;

    trigger.add_notification([&](EventType e) {
        ++called;
        lastEvent = e;
    });
    trigger.fire(42);

    EXPECT_EQ(called, 1);
    EXPECT_EQ(lastEvent, 42);
}

TEST(EventTriggerTest, FireWithMultipleSubscribers) {
    EventTrigger trigger;
    int count = 0;

    trigger.add_notification([&](EventType) { ++count; });
    trigger.add_notification([&](EventType) { ++count; });
    trigger.add_notification([&](EventType) { ++count; });
    trigger.fire(0);

    EXPECT_EQ(count, 3);
}

// ── 移除订阅 ─────────────────────────────────────────────────

TEST(EventTriggerTest, RemoveNotification) {
    EventTrigger trigger;
    int called = 0;

    auto handle = trigger.add_notification([&](EventType) { ++called; });
    trigger.remove_notification(handle);
    trigger.fire(0);

    EXPECT_EQ(called, 0);
}

TEST(EventTriggerTest, RemoveOneKeepsOthers) {
    EventTrigger trigger;
    int a = 0, b = 0;

    auto ha = trigger.add_notification([&](EventType) { ++a; });
    /* hb = */ trigger.add_notification([&](EventType) { ++b; });
    trigger.remove_notification(ha);
    trigger.fire(0);

    EXPECT_EQ(a, 0);
    EXPECT_EQ(b, 1);
}

// ── 槽位复用（dirty-block 模式） ──────────────────────────────

TEST(EventTriggerTest, SlotReuseAfterRemove) {
    EventTrigger trigger;

    auto h0 = trigger.add_notification([&](EventType) {});
    auto h1 = trigger.add_notification([&](EventType) {});
    EXPECT_EQ(h0, 0);
    EXPECT_EQ(h1, 1);

    // 移除槽位 0 后，新注册应复用槽位 0
    trigger.remove_notification(h0);
    auto h2 = trigger.add_notification([&](EventType) {});
    EXPECT_EQ(h2, 0);  // 复用已释放的槽位

    // 再注册则追加到末尾
    auto h3 = trigger.add_notification([&](EventType) {});
    EXPECT_EQ(h3, 2);
}

// ── 清空 ─────────────────────────────────────────────────────

TEST(EventTriggerTest, ClearRemovesAll) {
    EventTrigger trigger;
    int called = 0;

    trigger.add_notification([&](EventType) { ++called; });
    trigger.add_notification([&](EventType) { ++called; });
    trigger.clear_notificationList();
    trigger.fire(0);

    EXPECT_EQ(called, 0);
}

// ── 空触发不崩溃 ─────────────────────────────────────────────

TEST(EventTriggerTest, FireOnEmptyTrigger) {
    EventTrigger trigger;
    trigger.fire(99);  // 不应崩溃
    // 通过即成功
}

// ── 多次添加删除稳定性 ───────────────────────────────────────

TEST(EventTriggerTest, AddRemoveCycle) {
    EventTrigger trigger;

    for (int i = 0; i < 10; ++i) {
        auto h = trigger.add_notification([&](EventType) {});
        trigger.remove_notification(h);
    }
    // 所有槽位为 null，fire 应跳过
    trigger.fire(0);
    // 通过即无崩溃
}
