//

#include "app/App.h"

void App::initialize() {
    model_ = std::make_unique<GameModel>();
    vm_ = std::make_unique<ViewModel>(model_.get());

    view_ = std::make_unique<GameView>();

    // 1. 属性绑定
    view_->setCamera(vm_->getCameraX(), vm_->getCameraY());
    view_->setPlayerInfo(vm_->getPlayerInfo());
    view_->setTileInfos(vm_->getTileInfos());

    // 2. 命令绑定
    view_->setNextStepCommand(vm_->getNextStepCommand());
    view_->setActionCommand(vm_->getActionCommand());

    // 3. 通知绑定：ViewModel.fire(RENDER_UPDATE)  → render()
    vm_->addNotification(view_->getRenderNotification());
}

void App::run() {
    sf::Clock clock;
    while (view_->isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 1.0f / 30.0f);
        view_->NextStep(dt);
    }
}
