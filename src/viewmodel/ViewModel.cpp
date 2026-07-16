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

void ViewModel::startGame(int mapId) {
    model_->startGame(mapId);  // 载入所选地图并复位(经 modelTrigger 回调 syncFromModel)
    started_ = true;
    vmTrigger.fire(static_cast<EventType>(Event::STATE_CHANGED));  // 离开菜单并绘制新关卡
}

void ViewModel::onModelChanged(EventType ev) {
    syncFromModel();
    // 统一向上转发 common::Event 事件；由 View 按事件类型自行决定处理
    // （渲染只认 STATE_CHANGED，其余细粒度事件留给音效/特效等订阅者）。
    vmTrigger.fire(ev);
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

    enemy_infos_.clear();
    for (const auto& e : model_->enemies()) {
        if (!e.alive()) continue;
        enemy_infos_.push_back({e.x(), e.y(), e.width(), e.height(), e.facing()});
    }

    coin_infos_.clear();
    for (const auto& c : model_->coinItems()) {
        if (!c.alive) continue;
        coin_infos_.push_back({c.x, c.y, c.w, c.h, TileType::EMPTY});
    }

    mushroom_infos_.clear();
    for (const auto& m : model_->mushrooms()) {
        if (!m.active()) continue;
        mushroom_infos_.push_back({m.x(), m.y(), m.width(), m.height(), TileType::EMPTY});
    }
    player_big_ = model_->playerBig();

    goal_info_.x = model_->goalX();
    goal_info_.y = model_->goalY();
    goal_info_.w = model_->goalW();
    goal_info_.h = model_->goalH();
    goal_info_.type = TileType::EMPTY;
    won_ = model_->won();
    game_over_ = model_->gameOver();

    hud_info_.score = model_->score();
    hud_info_.coins = model_->coins();
    hud_info_.lives = model_->lives();
    hud_info_.timeRemaining = model_->timeRemaining();
    hud_info_.world = model_->world();

    updateCamera();
}

void ViewModel::updateCamera() noexcept {
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
    // 视口变化需重绘：发一个常规帧更新事件驱动 View 渲染
    vmTrigger.fire(static_cast<EventType>(Event::STATE_CHANGED));
}

void ViewModel::addNotification(Notify_Funtion func) { vmTrigger.add_notification(std::move(func)); }
