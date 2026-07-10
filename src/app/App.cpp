//

#include "app/App.h"

void App::initialize() {
    model_ = std::make_unique<GameModel>();
    vm_ = std::make_unique<ViewModel>(model_.get());

    view_ = std::make_unique<GameView>();

    // 1. 属性绑定：View 持有 ViewModel 数据的只读指针
    view_->setActionCommand(vm_->getActionCommand());
    view_->setCamera(vm_->getCameraX(), vm_->getCameraY());
    view_->setPlayerInfo(vm_->getPlayerInfo());
    view_->setTileInfo(vm_->getTileInfos());

    // 2. 命令绑定：View 事件 → ViewModel 方法（设计文档 §3.2.2）
    view_->setNextStepCommand(vm_->getNextStepCommand());

    // 3. ★ 通知绑定：ViewModel.fire(RENDER_UPDATE) → View.invalidate()
    //    贯通 MVVM 通知链路，View 收到通知后设脏标记，下一帧渲染
    vm_->addNotification([this](EventType) { view_->invalidate(); });

    model_->reset();
}

void App::run() { view_->run(); }
