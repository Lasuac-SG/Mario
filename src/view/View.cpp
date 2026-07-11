//

#include "view/View.h"

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

        input_.pollEvents(window_);
        input_.dispatchInput();

        int updates = 0;
        while (accumulator >= kFixedDt && updates < kMaxUpdatesPerFrame && window_.isOpen()) {
            lastDt_ = kFixedDt;
            if (nextStepCommand_) {
                nextStepCommand_(kFixedDt);
            }
            accumulator -= kFixedDt;
            ++updates;
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
