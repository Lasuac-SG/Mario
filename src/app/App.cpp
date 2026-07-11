#include "app/App.h"

void App::initialize() {
    model_ = std::make_unique<GameModel>();
    vm_ = std::make_unique<ViewModel>(model_.get());

    view_ = std::make_unique<GameView>();

    view_->setCamera(vm_->getCameraX(), vm_->getCameraY());
    view_->setPlayerInfo(vm_->getPlayerInfo());
    view_->setTileInfos(vm_->getTileInfos());

    view_->setNextStepCommand(vm_->getNextStepCommand());
    view_->setResizeCommand(vm_->getResizeCommand());
    view_->getInputHandler()->setActionCommand(vm_->getActionCommand());

    vm_->addNotification(view_->getRenderNotification());
    view_->pushCurrentViewportSize();
}

void App::run() { view_->run(); }
