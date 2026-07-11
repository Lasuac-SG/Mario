//

#include "view/View.h"

GameView::GameView()
    : window_(sf::VideoMode({800, 600}), "Mario Demo",
              sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize) {}

void GameView::run() {
    sf::Clock clock;  // 驱动时钟归属 View（拥有窗口者拥有主循环）
    while (window_.isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 1.0f / 30.0f);  // 防止 delta 螺旋
        NextStep(dt);
    }
}

void GameView::NextStep(float dt) {
    lastDt_ = dt;  // 记住外部时钟下推的 dt，供通知触发的 render 使用
    input_.pollEvents(window_);
    input_.dispatchInput();
    if (nextStepCommand_) {
        nextStepCommand_(dt);  // ViewModel::tick → fire → 通知回调 → renderer_.render()
    }
}

Notify_Funtion GameView::getRenderNotification() {
    return [this](EventType id) {
        if (id == static_cast<EventType>(ViewModelEvent::RENDER_UPDATE)) {
            renderer_.render(window_, lastDt_);
        }
    };
}
