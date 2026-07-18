#ifndef MARIO_VIEWINTERACTION_H
#define MARIO_VIEWINTERACTION_H

#include <optional>

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"

std::optional<int> levelFromMenuClick(const sf::FloatRect& level1Bounds,
                                      const sf::FloatRect& level2Bounds,
                                      const sf::Vector2f& point) noexcept;

std::optional<int> levelFromMenuKey(sf::Keyboard::Key key) noexcept;

#endif  // MARIO_VIEWINTERACTION_H
