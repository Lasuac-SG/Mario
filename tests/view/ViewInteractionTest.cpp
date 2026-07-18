#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "view/ViewInteraction.h"

namespace {

bool demoModeEnabled() {
    const char* value = std::getenv("TEST_VIEW_DEMO");
    return value && value[0] != '\0' && value[0] != '0';
}

void demoStep(const char* message) {
    if (!demoModeEnabled()) return;
    std::cout << "[demo] " << message << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
}

}  // namespace

TEST(ViewInteractionTest, ClickInsideLevel1SelectsLevel1) {
    const sf::FloatRect level1({100.0f, 200.0f}, {150.0f, 40.0f});
    const sf::FloatRect level2({100.0f, 280.0f}, {150.0f, 40.0f});

    demoStep("Define level 1 and level 2 clickable hotzones.");
    demoStep("Simulate click at (120, 220) inside level 1.");
    const auto selected = levelFromMenuClick(level1, level2, {120.0f, 220.0f});

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(*selected, 1);
}

TEST(ViewInteractionTest, ClickInsideLevel2SelectsLevel2) {
    const sf::FloatRect level1({100.0f, 200.0f}, {150.0f, 40.0f});
    const sf::FloatRect level2({100.0f, 280.0f}, {150.0f, 40.0f});

    demoStep("Define level 1 and level 2 clickable hotzones.");
    demoStep("Simulate click at (120, 300) inside level 2.");
    const auto selected = levelFromMenuClick(level1, level2, {120.0f, 300.0f});

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(*selected, 2);
}

TEST(ViewInteractionTest, ClickOutsideHotzonesSelectsNothing) {
    const sf::FloatRect level1({100.0f, 200.0f}, {150.0f, 40.0f});
    const sf::FloatRect level2({100.0f, 280.0f}, {150.0f, 40.0f});

    demoStep("Simulate click at (20, 20) outside both hotzones.");
    const auto selected = levelFromMenuClick(level1, level2, {20.0f, 20.0f});

    EXPECT_FALSE(selected.has_value());
}

TEST(ViewInteractionTest, NumberOneKeySelectsLevel1) {
    demoStep("Simulate pressing keyboard key 1.");
    const auto selected = levelFromMenuKey(sf::Keyboard::Key::Num1);

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(*selected, 1);
}

TEST(ViewInteractionTest, NumpadTwoKeySelectsLevel2) {
    demoStep("Simulate pressing numpad key 2.");
    const auto selected = levelFromMenuKey(sf::Keyboard::Key::Numpad2);

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(*selected, 2);
}

TEST(ViewInteractionTest, OtherKeysDoNotSelectLevel) {
    demoStep("Simulate pressing Escape, which should not select any level.");
    const auto selected = levelFromMenuKey(sf::Keyboard::Key::Escape);

    EXPECT_FALSE(selected.has_value());
}
