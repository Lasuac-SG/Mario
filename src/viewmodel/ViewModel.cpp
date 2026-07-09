//

#include "viewmodel/ViewModel.h"
#include "model/GameModel.h"

GameViewModel::GameViewModel(GameModel* model)
    : inputCmd_(model) {
    model_ = model;

    // 订阅 Model 状态变更
    modelSubId_ = model_->modelTrigger.add_notification(
        [this](EventType ev) {
            onModelChanged(ev);
        });
}

GameViewModel::~GameViewModel() {
    model_->modelTrigger.remove_notification(modelSubId_);
}

void GameViewModel::onModelChanged(EventType /*ev*/) {
    // Model 状态已更新 → 通知 View 重绘
    vmTrigger.fire(static_cast<uint32_t>(ViewModelEvent::RENDER_UPDATE));
}

void GameViewModel::tick(float dt) {
    model_->update(dt);
}