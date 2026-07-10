//

#include "view/View.h"

GameView::GameView()
    : window_(sf::VideoMode({800, 600}), "Mario Demo",
              sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize) {}

void GameView::NextStep(float dt) {
    input_.pollEvents(window_);
    input_.dispatchInput();
    if (nextStepCommand_) {
        nextStepCommand_(dt);  // ViewModel::tick → fire → 通知回调 → renderer_.render()
    }
}

Notify_Funtion GameView::getRenderNotification() {
    return [this](EventType id) {
        if (id == static_cast<EventType>(ViewModelEvent::RENDER_UPDATE)) {
            renderer_.render(window_);
        }
    };
}
