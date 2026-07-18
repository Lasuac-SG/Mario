#include "common/Type.h"
#include "view/ViewInteraction.h"
#include "view/input/InputHandler.h"

namespace {

const std::unordered_map<char, std::vector<std::string>>& glyphs() {
    static const std::unordered_map<char, std::vector<std::string>> kGlyphs = {
        {'A', {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}},
        {'B', {"11110", "10001", "10001", "11110", "10001", "10001", "11110"}},
        {'C', {"01111", "10000", "10000", "10000", "10000", "10000", "01111"}},
        {'D', {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}},
        {'E', {"11111", "10000", "10000", "11110", "10000", "10000", "11111"}},
        {'F', {"11111", "10000", "10000", "11110", "10000", "10000", "10000"}},
        {'G', {"01111", "10000", "10000", "10011", "10001", "10001", "01110"}},
        {'H', {"10001", "10001", "10001", "11111", "10001", "10001", "10001"}},
        {'I', {"11111", "00100", "00100", "00100", "00100", "00100", "11111"}},
        {'J', {"11111", "00001", "00001", "00001", "10001", "10001", "01110"}},
        {'K', {"10001", "10010", "10100", "11000", "10100", "10010", "10001"}},
        {'L', {"10000", "10000", "10000", "10000", "10000", "10000", "11111"}},
        {'M', {"10001", "11011", "10101", "10101", "10001", "10001", "10001"}},
        {'N', {"10001", "11001", "10101", "10011", "10001", "10001", "10001"}},
        {'O', {"01110", "10001", "10001", "10001", "10001", "10001", "01110"}},
        {'P', {"11110", "10001", "10001", "11110", "10000", "10000", "10000"}},
        {'Q', {"01110", "10001", "10001", "10001", "10101", "10010", "01101"}},
        {'R', {"11110", "10001", "10001", "11110", "10100", "10010", "10001"}},
        {'S', {"01111", "10000", "10000", "01110", "00001", "00001", "11110"}},
        {'T', {"11111", "00100", "00100", "00100", "00100", "00100", "00100"}},
        {'U', {"10001", "10001", "10001", "10001", "10001", "10001", "01110"}},
        {'V', {"10001", "10001", "10001", "10001", "10001", "01010", "00100"}},
        {'W', {"10001", "10001", "10001", "10101", "10101", "10101", "01010"}},
        {'X', {"10001", "01010", "00100", "00100", "00100", "01010", "10001"}},
        {'Y', {"10001", "10001", "01010", "00100", "00100", "00100", "00100"}},
        {'Z', {"11111", "00001", "00010", "00100", "01000", "10000", "11111"}},
        {'0', {"01110", "10001", "10011", "10101", "11001", "10001", "01110"}},
        {'1', {"00100", "01100", "00100", "00100", "00100", "00100", "01110"}},
        {'2', {"01110", "10001", "00001", "00010", "00100", "01000", "11111"}},
        {'3', {"11110", "00001", "00001", "01110", "00001", "00001", "11110"}},
        {'4', {"00010", "00110", "01010", "10010", "11111", "00010", "00010"}},
        {'5', {"11111", "10000", "10000", "11110", "00001", "00001", "11110"}},
        {'6', {"01110", "10000", "10000", "11110", "10001", "10001", "01110"}},
        {'7', {"11111", "00001", "00010", "00100", "01000", "01000", "01000"}},
        {'8', {"01110", "10001", "10001", "01110", "10001", "10001", "01110"}},
        {'9', {"01110", "10001", "10001", "01111", "00001", "00001", "01110"}},
        {'-', {"00000", "00000", "00000", "11111", "00000", "00000", "00000"}},
        {':', {"00000", "00100", "00100", "00000", "00100", "00100", "00000"}},
        {'/', {"00001", "00010", "00100", "01000", "10000", "00000", "00000"}},
        {' ', {"00000", "00000", "00000", "00000", "00000", "00000", "00000"}},
    };
    return kGlyphs;
}

std::string upper(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return text;
}

float measurePixelText(const std::string& text, float scale) {
    constexpr float kPixelGap = 1.0f;
    constexpr float kLetterGap = 2.0f;
    const auto& font = glyphs();

    float width = 0.0f;
    for (char ch : upper(text)) {
        auto it = font.find(ch);
        const std::size_t glyphWidth = (it == font.end()) ? 5 : it->second.front().size();
        width += glyphWidth * (scale + kPixelGap) + kLetterGap * scale;
    }

    return width > 0.0f ? width - kLetterGap * scale : 0.0f;
}

void drawPixelText(sf::RenderWindow& window,
                   const std::string& text,
                   float x,
                   float y,
                   float scale,
                   sf::Color color = sf::Color::White) {
    constexpr float kPixelGap = 1.0f;
    constexpr float kLetterGap = 2.0f;
    const auto& font = glyphs();

    sf::RectangleShape pixel({scale, scale});
    float cursorX = x;
    for (char ch : upper(text)) {
        auto it = font.find(ch);
        if (it == font.end()) {
            cursorX += 6.0f * scale;
            continue;
        }

        const auto& rows = it->second;
        for (std::size_t row = 0; row < rows.size(); ++row) {
            for (std::size_t col = 0; col < rows[row].size(); ++col) {
                if (rows[row][col] != '1') continue;
                pixel.setPosition({cursorX + col * (scale + kPixelGap), y + row * (scale + kPixelGap)});
                pixel.setFillColor(color);
                window.draw(pixel);
            }
        }

        cursorX += rows.front().size() * (scale + kPixelGap) + kLetterGap * scale;
    }
}

std::string inputActionName(InputAction action) {
    switch (action) {
        case InputAction::MOVE_LEFT:
            return "MOVE LEFT";
        case InputAction::MOVE_RIGHT:
            return "MOVE RIGHT";
        case InputAction::STOP:
            return "STOP";
        case InputAction::JUMP:
            return "JUMP";
        case InputAction::RESTART:
            return "RESTART";
        default:
            return "UNKNOWN";
    }
}

class RecordingCommand : public ICommandBase {
   public:
    int exec(ICommandParameter* p) override {
        auto* action = dynamic_cast<InputActionParameter*>(p);
        if (!action) return -1;
        lastAction = inputActionName(action->v_);
        history.push_back("ACTION: " + lastAction);
        if (history.size() > 10) history.erase(history.begin());
        return 0;
    }

    std::string lastAction = "NONE";
    std::vector<std::string> history;
};

struct KeyState {
    bool left = false;
    bool right = false;
    bool jump = false;
    bool restart = false;
};

void pushHistory(std::vector<std::string>& history, const std::string& entry) {
    history.push_back(entry);
    if (history.size() > 10) history.erase(history.begin());
}

void syncKeys(InputHandler& input, const KeyState& keys) {
    input.resetKeyStateForTest();
    input.setKeyStateForTest(sf::Keyboard::Key::Left, keys.left);
    input.setKeyStateForTest(sf::Keyboard::Key::A, keys.left);
    input.setKeyStateForTest(sf::Keyboard::Key::Right, keys.right);
    input.setKeyStateForTest(sf::Keyboard::Key::D, keys.right);
    input.setKeyStateForTest(sf::Keyboard::Key::Space, keys.jump);
    input.setKeyStateForTest(sf::Keyboard::Key::W, keys.jump);
    input.setKeyStateForTest(sf::Keyboard::Key::Up, keys.jump);
    input.setKeyStateForTest(sf::Keyboard::Key::R, keys.restart);
}

void drawPanel(sf::RenderWindow& window, const sf::FloatRect& rect, sf::Color fill, sf::Color outline) {
    sf::RectangleShape shape(rect.size);
    shape.setPosition(rect.position);
    shape.setFillColor(fill);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(outline);
    window.draw(shape);
}

void drawKeyBox(sf::RenderWindow& window,
                const sf::FloatRect& rect,
                const std::string& label,
                bool active,
                const std::string& hint) {
    drawPanel(window, rect, active ? sf::Color(74, 162, 88) : sf::Color(44, 52, 66), sf::Color(170, 180, 195));
    drawPixelText(window,
                  label,
                  rect.position.x + (rect.size.x - measurePixelText(label, 2.0f)) * 0.5f,
                  rect.position.y + 12.0f,
                  2.0f);
    drawPixelText(window,
                  hint,
                  rect.position.x + (rect.size.x - measurePixelText(hint, 1.3f)) * 0.5f,
                  rect.position.y + 44.0f,
                  1.3f,
                  sf::Color(218, 223, 230));
}

}  // namespace

int main() {
    sf::RenderWindow window(sf::VideoMode({1100, 760}), "Input Integration Test Window", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    InputHandler input;
    RecordingCommand command;
    input.setActionCommand(&command);

    KeyState keys;
    std::vector<std::string> uiHistory = {
        "CLICK LEVEL 1 OR LEVEL 2",
        "PRESS LEFT RIGHT SPACE OR R",
    };
    std::string lastMenuResult = "NONE";

    const sf::FloatRect level1({80.0f, 110.0f}, {390.0f, 120.0f});
    const sf::FloatRect level2({80.0f, 260.0f}, {390.0f, 120.0f});
    bool hoverLevel1 = false;
    bool hoverLevel2 = false;

    while (window.isOpen()) {
        while (const auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
                continue;
            }

            if (const auto* moved = ev->getIf<sf::Event::MouseMoved>()) {
                const sf::Vector2f point(static_cast<float>(moved->position.x), static_cast<float>(moved->position.y));
                hoverLevel1 = level1.contains(point);
                hoverLevel2 = level2.contains(point);
            }

            if (const auto* clicked = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (clicked->button == sf::Mouse::Button::Left) {
                    const sf::Vector2f point(static_cast<float>(clicked->position.x), static_cast<float>(clicked->position.y));
                    const auto selected = levelFromMenuClick(level1, level2, point);
                    if (selected) {
                        lastMenuResult = "LEVEL " + std::to_string(*selected);
                        pushHistory(uiHistory, "MOUSE CLICK -> " + lastMenuResult);
                    } else {
                        pushHistory(uiHistory, "MOUSE CLICK -> OUTSIDE");
                    }
                }
            }

            if (const auto* pressed = ev->getIf<sf::Event::KeyPressed>()) {
                switch (pressed->code) {
                    case sf::Keyboard::Key::Left:
                    case sf::Keyboard::Key::A:
                        keys.left = true;
                        break;
                    case sf::Keyboard::Key::Right:
                    case sf::Keyboard::Key::D:
                        keys.right = true;
                        break;
                    case sf::Keyboard::Key::Space:
                    case sf::Keyboard::Key::W:
                    case sf::Keyboard::Key::Up:
                        keys.jump = true;
                        break;
                    case sf::Keyboard::Key::R:
                        keys.restart = true;
                        break;
                    default:
                        break;
                }

                const auto selected = levelFromMenuKey(pressed->code);
                if (selected) {
                    lastMenuResult = "LEVEL " + std::to_string(*selected);
                    pushHistory(uiHistory, "KEY SELECT -> " + lastMenuResult);
                }

                syncKeys(input, keys);
                input.dispatchInput();
            }

            if (const auto* released = ev->getIf<sf::Event::KeyReleased>()) {
                switch (released->code) {
                    case sf::Keyboard::Key::Left:
                    case sf::Keyboard::Key::A:
                        keys.left = false;
                        break;
                    case sf::Keyboard::Key::Right:
                    case sf::Keyboard::Key::D:
                        keys.right = false;
                        break;
                    case sf::Keyboard::Key::Space:
                    case sf::Keyboard::Key::W:
                    case sf::Keyboard::Key::Up:
                        keys.jump = false;
                        break;
                    case sf::Keyboard::Key::R:
                        keys.restart = false;
                        break;
                    default:
                        break;
                }

                syncKeys(input, keys);
                input.dispatchInput();
            }
        }

        window.clear(sf::Color(18, 24, 38));

        drawPixelText(window, "INPUT INTEGRATION TEST WINDOW", 60.0f, 30.0f, 2.5f, sf::Color(245, 242, 230));
        drawPixelText(window, "REAL CLICK AND KEYBOARD DEMO", 60.0f, 68.0f, 1.6f, sf::Color(169, 180, 196));

        drawPanel(window, level1, hoverLevel1 ? sf::Color(112, 86, 32) : sf::Color(79, 60, 24), sf::Color(230, 198, 102));
        drawPixelText(window, "LEVEL 1 HOTZONE", level1.position.x + 30.0f, level1.position.y + 28.0f, 2.3f);
        drawPixelText(window, "CLICK HERE OR PRESS 1", level1.position.x + 34.0f, level1.position.y + 72.0f, 1.5f, sf::Color(245, 233, 188));

        drawPanel(window, level2, hoverLevel2 ? sf::Color(44, 79, 112) : sf::Color(28, 52, 74), sf::Color(120, 206, 255));
        drawPixelText(window, "LEVEL 2 HOTZONE", level2.position.x + 30.0f, level2.position.y + 28.0f, 2.3f);
        drawPixelText(window, "CLICK HERE OR PRESS 2", level2.position.x + 34.0f, level2.position.y + 72.0f, 1.5f, sf::Color(206, 239, 255));

        drawPixelText(window, "MENU RESULT: " + lastMenuResult, 80.0f, 420.0f, 1.8f, sf::Color(255, 236, 150));
        drawPixelText(window, "LAST INPUT ACTION: " + command.lastAction, 80.0f, 456.0f, 1.8f, sf::Color(152, 227, 166));

        drawKeyBox(window, {{560.0f, 110.0f}, {140.0f, 90.0f}}, "LEFT", keys.left, "A / LEFT");
        drawKeyBox(window, {{720.0f, 110.0f}, {140.0f, 90.0f}}, "RIGHT", keys.right, "D / RIGHT");
        drawKeyBox(window, {{880.0f, 110.0f}, {140.0f, 90.0f}}, "JUMP", keys.jump, "SPACE / W");
        drawKeyBox(window, {{720.0f, 225.0f}, {140.0f, 90.0f}}, "RESTART", keys.restart, "R");

        drawPixelText(window, "EVENT HISTORY", 560.0f, 360.0f, 2.0f, sf::Color(245, 242, 230));
        sf::FloatRect historyPanel({540.0f, 400.0f}, {500.0f, 280.0f});
        drawPanel(window, historyPanel, sf::Color(24, 31, 48), sf::Color(95, 111, 139));

        int line = 0;
        for (const auto& entry : uiHistory) {
            drawPixelText(window, entry, historyPanel.position.x + 16.0f, historyPanel.position.y + 16.0f + line * 24.0f, 1.3f);
            ++line;
        }
        for (const auto& entry : command.history) {
            drawPixelText(window,
                          entry,
                          historyPanel.position.x + 16.0f,
                          historyPanel.position.y + 16.0f + line * 24.0f,
                          1.3f,
                          sf::Color(168, 237, 175));
            ++line;
        }

        drawPixelText(window, "PRESS ESC TO EXIT", 80.0f, 700.0f, 1.4f, sf::Color(170, 180, 196));
        window.display();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            window.close();
        }
    }

    return 0;
}
