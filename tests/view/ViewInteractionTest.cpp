#include <gtest/gtest.h>

#include "view/ViewInteraction.h"

TEST(ViewInteractionTest, ClickInsideLevel1SelectsLevel1) {
    const sf::FloatRect level1({100.0f, 200.0f}, {150.0f, 40.0f});
    const sf::FloatRect level2({100.0f, 280.0f}, {150.0f, 40.0f});

    const auto selected = levelFromMenuClick(level1, level2, {120.0f, 220.0f});

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(*selected, 1);
}

TEST(ViewInteractionTest, ClickInsideLevel2SelectsLevel2) {
    const sf::FloatRect level1({100.0f, 200.0f}, {150.0f, 40.0f});
    const sf::FloatRect level2({100.0f, 280.0f}, {150.0f, 40.0f});

    const auto selected = levelFromMenuClick(level1, level2, {120.0f, 300.0f});

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(*selected, 2);
}

TEST(ViewInteractionTest, ClickOutsideHotzonesSelectsNothing) {
    const sf::FloatRect level1({100.0f, 200.0f}, {150.0f, 40.0f});
    const sf::FloatRect level2({100.0f, 280.0f}, {150.0f, 40.0f});

    const auto selected = levelFromMenuClick(level1, level2, {20.0f, 20.0f});

    EXPECT_FALSE(selected.has_value());
}

TEST(ViewInteractionTest, NumberOneKeySelectsLevel1) {
    const auto selected = levelFromMenuKey(sf::Keyboard::Key::Num1);

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(*selected, 1);
}

TEST(ViewInteractionTest, NumpadTwoKeySelectsLevel2) {
    const auto selected = levelFromMenuKey(sf::Keyboard::Key::Numpad2);

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(*selected, 2);
}

TEST(ViewInteractionTest, OtherKeysDoNotSelectLevel) {
    const auto selected = levelFromMenuKey(sf::Keyboard::Key::Escape);

    EXPECT_FALSE(selected.has_value());
}
