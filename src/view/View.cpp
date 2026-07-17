#include "view/View.h"

#include <algorithm>

namespace {

sf::Vector2f effectiveViewportSize(const sf::Vector2u& windowSize) {
    const float rawW = static_cast<float>(windowSize.x);
    const float rawH = static_cast<float>(windowSize.y);

    constexpr float kGrowthFactor = 0.2f;
    const float viewW =
        (rawW <= DefaultViewWidth) ? rawW : DefaultViewWidth + (rawW - DefaultViewWidth) * kGrowthFactor;
    const float viewH =
        (rawH <= DefaultViewHeight) ? rawH : DefaultViewHeight + (rawH - DefaultViewHeight) * kGrowthFactor;

    return {viewW, viewH};
}

}  // namespace

GameView::GameView()
    : window_(sf::VideoMode({800, 600}), "Mario Demo", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize) {
    window_.setVerticalSyncEnabled(false);
    window_.setFramerateLimit(120);
}

void GameView::run() {
    constexpr float kFixedDt = 1.0f / 60.0f;
    constexpr float kMaxFrameDt = 0.1f;
    constexpr int kMaxUpdatesPerFrame = 4;

    sf::Clock clock;
    float accumulator = 0.0f;

    audio_.playBGM("./audio/bgm.ogg");
    audio_.loadAllSFX();

    while (window_.isOpen()) {
        float frameDt = clock.restart().asSeconds();
        frameDt = std::min(frameDt, kMaxFrameDt);
        accumulator += frameDt;

        processWindowEvents();
        if (gameStarted_ && *gameStarted_) {
            input_.pollEvents(window_);
            input_.dispatchInput();
        }

        int updates = 0;
        while (accumulator >= kFixedDt && updates < kMaxUpdatesPerFrame && window_.isOpen()) {
            NextStep(kFixedDt);
            accumulator -= kFixedDt;
            ++updates;
        }
    }
}

void GameView::pushCurrentViewportSize() {
    if (!resizeCommand_) return;

    const auto size = effectiveViewportSize(window_.getSize());
    resizeCommand_(size.x, size.y);
}

void GameView::processWindowEvents() {
    while (const auto ev = window_.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            window_.close();
            continue;
        }

        if (gameOver_ && *gameOver_) {
            if (const auto* moved = ev->getIf<sf::Event::MouseMoved>()) {
                const sf::Vector2f point = window_.mapPixelToCoords({moved->position.x, moved->position.y}, window_.getDefaultView());
                renderer_.setRestartHovered(renderer_.restartButtonBounds(window_).contains(point));
            }

            if (const auto* pressed = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (pressed->button == sf::Mouse::Button::Left) {
                    const sf::Vector2f point = window_.mapPixelToCoords({pressed->position.x, pressed->position.y}, window_.getDefaultView());
                    if (renderer_.restartButtonBounds(window_).contains(point)) {
                        InputActionParameter p(InputAction::RESTART);
                        input_.getActionCommand()->exec(&p);
                    }
                }
            }
        }

        // 菜单模式：鼠标 hover + 点击选关
        if (gameStarted_ && !(*gameStarted_)) {
            if (const auto* moved = ev->getIf<sf::Event::MouseMoved>()) {
                const sf::Vector2f point = window_.mapPixelToCoords(
                    {moved->position.x, moved->position.y}, window_.getDefaultView());
                renderer_.setLevel1Hovered(renderer_.level1Bounds(window_).contains(point));
                renderer_.setLevel2Hovered(renderer_.level2Bounds(window_).contains(point));
            }
            if (const auto* pressed = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (pressed->button == sf::Mouse::Button::Left) {
                    const sf::Vector2f point = window_.mapPixelToCoords(
                        {pressed->position.x, pressed->position.y}, window_.getDefaultView());
                    if (renderer_.level1Bounds(window_).contains(point)) {
                        if (startGameCommand_) startGameCommand_(1);
                    } else if (renderer_.level2Bounds(window_).contains(point)) {
                        if (startGameCommand_) startGameCommand_(2);
                    }
                }
            }
            if (const auto* key = ev->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Num1 || key->code == sf::Keyboard::Key::Numpad1) {
                    if (startGameCommand_) startGameCommand_(1);
                } else if (key->code == sf::Keyboard::Key::Num2 || key->code == sf::Keyboard::Key::Numpad2) {
                    if (startGameCommand_) startGameCommand_(2);
                }
            }
        }

        if (const auto* resized = ev->getIf<sf::Event::Resized>()) {
            if (resizeCommand_) {
                const auto size = effectiveViewportSize(resized->size);
                resizeCommand_(size.x, size.y);
            }
        }
    }
}

void GameView::NextStep(float dt) {
    lastDt_ = dt;
    if (nextStepCommand_) {
        nextStepCommand_(dt);
    }
}

Notify_Funtion GameView::getRenderNotification() {
    return [this](EventType id) {
        // 鍙湪甯歌甯ф洿鏂?甯ф湯鐨勫畬鏁寸姸鎬?鏃舵覆鏌擄紱鍏朵綑缁嗙矑搴︿簨浠舵槸甯т腑閫?鐐逛簨浠讹紝涓嶉┍鍔ㄩ噸缁樸€?
        if (id == static_cast<EventType>(Event::STATE_CHANGED)) {
            renderer_.render(window_, lastDt_);
        }
    };
}
Notify_Funtion GameView::getAudioNotification() {
    return audio_.getNotification();
}
