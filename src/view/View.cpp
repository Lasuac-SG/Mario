#include "view/View.h"

#include <algorithm>

namespace {

sf::Vector2f effectiveViewportSize(const sf::Vector2u& windowSize) {
    const float rawW = static_cast<float>(windowSize.x);
    const float rawH = static_cast<float>(windowSize.y);

    constexpr float kGrowthFactor = 0.2f;
    const float viewW = (rawW <= DefaultViewWidth) ? rawW : DefaultViewWidth + (rawW - DefaultViewWidth) * kGrowthFactor;
    const float viewH = (rawH <= DefaultViewHeight) ? rawH : DefaultViewHeight + (rawH - DefaultViewHeight) * kGrowthFactor;

    return {viewW, viewH};
}

}  // namespace

GameView::GameView()
    : window_(sf::VideoMode({800, 600}), "Mario Demo",
              sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize) {
    window_.setVerticalSyncEnabled(false);
    window_.setFramerateLimit(120);
}

void GameView::run() {
    constexpr float kFixedDt = 1.0f / 60.0f;
    constexpr float kMaxFrameDt = 0.1f;
    constexpr int kMaxUpdatesPerFrame = 4;

    sf::Clock clock;
    float accumulator = 0.0f;

    while (window_.isOpen()) {
        float frameDt = clock.restart().asSeconds();
        frameDt = std::min(frameDt, kMaxFrameDt);
        accumulator += frameDt;

        processWindowEvents();
        input_.pollEvents(window_);
        input_.dispatchInput();

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
        if (id == static_cast<EventType>(ViewModelEvent::RENDER_UPDATE)) {
            renderer_.render(window_, lastDt_);
        }
    };
}
