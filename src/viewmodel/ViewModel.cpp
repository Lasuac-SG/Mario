//
#include "viewmodel/ViewModel.h"

#include "common/Type.h"

ViewModel::ViewModel(GameModel* model, ViewportDim viewW, ViewportDim viewH)
    : model_(model), actionCmd_(model), viewW_(viewW), viewH_(viewH) {
    funct_callback_Index_ = model_->modelTrigger.add_notification([this](EventType ev) { onModelChanged(ev); });
    syncFromModel();  // 初始化时同步一次
}

ViewModel::~ViewModel() { model_->modelTrigger.remove_notification(funct_callback_Index_); }

void ViewModel::onModelChanged(EventType /*ev*/) {
    syncFromModel();
    vmTrigger.fire(static_cast<uint32_t>(ViewModelEvent::RENDER_UPDATE));
}

void ViewModel::syncFromModel() {
    // 同步玩家数据
    player_info_.x = model_->playerX();
    player_info_.y = model_->playerY();
    player_info_.width = model_->playerW();
    player_info_.height = model_->playerH();
    player_info_.state = model_->playerState();
    player_info_.direction = model_->playerFacing();

    // 同步瓦片数据
    tile_infos_.clear();
    for (const auto& tile : model_->tiles()) {
        tile_infos_.push_back({tile.x, tile.y, tile.w, tile.h, tile.type});
    }

    // 更新相机：跟随玩家中心，限制在关卡边界内
    const PositionType playerCenterX = player_info_.x + player_info_.width / 2.0f;
    const PositionType playerCenterY = player_info_.y + player_info_.height / 2.0f;

    const PositionType halfW = viewW_ / 2.0f;
    const PositionType halfH = viewH_ / 2.0f;
    const PositionType levelW = model_->levelWidthPx();
    const PositionType levelH = model_->levelHeightPx();

    cameraX_ = std::clamp(playerCenterX, halfW, std::max(levelW - halfW, halfW));
    cameraY_ = std::clamp(playerCenterY, halfH, std::max(levelH - halfH, halfH));
}

void ViewModel::addNotification(Notify_Funtion func) { vmTrigger.add_notification(std::move(func)); }
