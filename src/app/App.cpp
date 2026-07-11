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
    view_->getInputHandler()->setActionCommand(vm_->getActionCommand());

    // 3. 通知绑定：ViewModel.fire(RENDER_UPDATE)  → render()
    vm_->addNotification(view_->getRenderNotification());
}

void App::run() { view_->run(); }
