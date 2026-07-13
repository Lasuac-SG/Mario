#include "viewmodel/ViewModel.h"

#include <algorithm>

#include "common/Type.h"

ViewModel::ViewModel(GameModel* model, ViewportDim viewW, ViewportDim viewH)
    : model_(model), actionCmd_(model), viewW_(viewW), viewH_(viewH) {
    funct_callback_Index_ = model_->modelTrigger.add_notification([this](EventType ev) { onModelChanged(ev); });
    syncFromModel();
}

ViewModel::~ViewModel() { model_->modelTrigger.remove_notification(funct_callback_Index_); }

void ViewModel::tick(float dt) { model_->update(dt); }

void ViewModel::onModelChanged(EventType /*ev*/) {
    syncFromModel();
    vmTrigger.fire(static_cast<uint32_t>(ViewModelEvent::RENDER_UPDATE));
}

void ViewModel::syncFromModel() {
    player_info_.x = model_->playerX();
    player_info_.y = model_->playerY();
    player_info_.width = model_->playerW();
    player_info_.height = model_->playerH();
    player_info_.state = model_->playerState();
    player_info_.direction = model_->playerFacing();

    tile_infos_.clear();
    for (const auto& tile : model_->tiles()) {
        tile_infos_.push_back({tile.x, tile.y, tile.w, tile.h, tile.type});
    }

    // 仅同步存活敌人（绝对像素坐标；死亡的不下发，View 即不再绘制）
    enemy_infos_.clear();
    for (const auto& e : model_->enemies()) {
        if (!e.alive()) continue;
        enemy_infos_.push_back({e.x(), e.y(), e.width(), e.height(), e.facing()});
    }

    hud_info_.score = model_->score();
    hud_info_.coins = model_->coins();
    hud_info_.lives = model_->lives();
    hud_info_.timeRemaining = model_->timeRemaining();
    hud_info_.world = model_->world();

    updateCamera();
}

void ViewModel::updateCamera() {
    const PositionType playerCenterX = player_info_.x + player_info_.width / 2.0f;
    const PositionType playerCenterY = player_info_.y + player_info_.height / 2.0f;

    const PositionType halfW = viewW_ / 2.0f;
    const PositionType halfH = viewH_ / 2.0f;
    const PositionType levelW = model_->levelWidthPx();
    const PositionType levelH = model_->levelHeightPx();

    cameraX_ = (levelW > viewW_) ? std::clamp(playerCenterX, halfW, levelW - halfW) : levelW / 2.0f;
    cameraY_ = (levelH > viewH_) ? std::clamp(playerCenterY, halfH, levelH - halfH) : levelH / 2.0f;
}

void ViewModel::setViewport(ViewportDim w, ViewportDim h) {
    if (w <= 0.0f || h <= 0.0f) return;
    viewW_ = w;
    viewH_ = h;
    updateCamera();
    vmTrigger.fire(static_cast<uint32_t>(ViewModelEvent::RENDER_UPDATE));
}

void ViewModel::addNotification(Notify_Funtion func) { vmTrigger.add_notification(std::move(func)); }
