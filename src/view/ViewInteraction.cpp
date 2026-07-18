#include "view/ViewInteraction.h"

std::optional<int> levelFromMenuClick(const sf::FloatRect& level1Bounds,
                                      const sf::FloatRect& level2Bounds,
                                      const sf::Vector2f& point) noexcept {
    if (level1Bounds.contains(point)) {
        return 1;
    }
    if (level2Bounds.contains(point)) {
        return 2;
    }
    return std::nullopt;
}

std::optional<int> levelFromMenuKey(sf::Keyboard::Key key) noexcept {
    if (key == sf::Keyboard::Key::Num1 || key == sf::Keyboard::Key::Numpad1) {
        return 1;
    }
    if (key == sf::Keyboard::Key::Num2 || key == sf::Keyboard::Key::Numpad2) {
        return 2;
    }
    return std::nullopt;
}
