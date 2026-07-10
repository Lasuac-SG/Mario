//

#include "app/App.h"

void App::initialize() {
    model_ = std::make_unique<GameModel>();
    vm_ = std::make_unique<ViewModel>(model_.get());
    renderer_ = std::make_unique<SimpleRenderer>();
    view_ = std::make_unique<GameView>(vm_.get(), renderer_.get());
    view_->setActionCommand(vm_->getActionCommand());
    view_->setCamera(vm_->getCameraX(), vm_->getCameraY());
    model_->reset();
}

void App::run() { view_->run(); }
