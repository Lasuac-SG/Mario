//

#include "app/App.h"

void App::initialize() {
    model_ = std::make_unique<GameModel>();
    vm_ = std::make_unique<ViewModel>(model_.get());

    view_ = std::make_unique<GameView>();
    view_->setActionCommand(vm_->getActionCommand());
    view_->setCamera(vm_->getCameraX(), vm_->getCameraY());
    view_->setPlayerInfo(vm_->getPlayerInfo());
    view_->setTileInfo(vm_->getTileInfos());
    view_->setUpdateFunction(vm_->getUpdateFrameFunction());

    model_->reset();
}

void App::run() { view_->run(); }
