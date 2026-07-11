//
#include "viewmodel/ViewModel.h"

#include "common/Type.h"

ViewModel::ViewModel(GameModel* model, ViewportDim viewW, ViewportDim viewH)
    : model_(model), actionCmd_(model), viewW_(viewW), viewH_(viewH) {
    funct_callback_Index_ = model_->modelTrigger.add_notification([this](EventType ev) { onModelChanged(ev); });
    syncFromModel();  // 初始化时同步一次
}

ViewModel::~ViewModel() { model_->modelTrigger.remove_notification(funct_callback_Index_); }

void ViewModel::tick(float dt) {
    model_->update(dt);
    // model_->update 内部会 fire(STATE_CHANGED)
    // → onModelChanged() → syncFromModel() → fire(RENDER_UPDATE)
}

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

    updateCamera();
}

// 相机中心（世界坐标）：某轴上关卡比视口大 → 跟随玩家并夹在关卡边界内；
// 否则（窗口比关卡还大）→ 关卡整体居中，多出的部分显示为背景。
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

// View 在窗口尺寸变化时下推真实窗口尺寸：更新视口 → 重算相机 → 请求重绘。
void ViewModel::setViewport(ViewportDim w, ViewportDim h) {
    if (w <= 0.0f || h <= 0.0f) return;  // 忽略最小化 / 非法尺寸
    viewW_ = w;
    viewH_ = h;
    updateCamera();
    vmTrigger.fire(static_cast<uint32_t>(ViewModelEvent::RENDER_UPDATE));
}

void ViewModel::addNotification(Notify_Funtion func) { vmTrigger.add_notification(std::move(func)); }
