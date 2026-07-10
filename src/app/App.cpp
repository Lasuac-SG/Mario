//

#include "app/App.h"

void App::initialize() {
    model_ = std::make_unique<GameModel>();
    vm_ = std::make_unique<ViewModel>(model_.get());
    renderer_ = std::make_unique<SimpleRenderer>();
    view_ = std::make_unique<GameView>(vm_.get(), renderer_.get());

    model_->reset();
}

void App::run() { view_->run(); }
