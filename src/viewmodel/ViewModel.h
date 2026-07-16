#ifndef MARIO_VIEWMODEL_H
#define MARIO_VIEWMODEL_H

#include "common/EventId.h"
#include "common/EventTrigger.h"
#include "common/Type.h"
#include "model/GameModel.h"
#include "viewmodel/command/Commands.h"

class ViewModel {
   public:
    explicit ViewModel(GameModel* model, ViewportDim viewW = DefaultViewWidth,
                       ViewportDim viewH = DefaultViewHeight);
    ~ViewModel();

    const PlayerInfo* getPlayerInfo() const noexcept { return &player_info_; }
    const TileInfos* getTileInfos() const noexcept { return &tile_infos_; }
    const EnemyInfos* getEnemyInfos() const noexcept { return &enemy_infos_; }
    // 未拾取的金币（复用 TileInfo 承载矩形，type 忽略），供 View 绘制
    const TileInfos* getCoinInfos() const noexcept { return &coin_infos_; }
    const HudInfo* getHudInfo() const noexcept { return &hud_info_; }
    PositionType levelWidthPx() const { return model_->levelWidthPx(); }
    PositionType levelHeightPx() const { return model_->levelHeightPx(); }

    const PositionType* getCameraX() const { return &cameraX_; }
    const PositionType* getCameraY() const { return &cameraY_; }

    ICommandBase* getActionCommand() { return &actionCmd_; }

    void setViewport(ViewportDim w, ViewportDim h);
    void addNotification(Notify_Funtion func);

    std::function<void(float)> getNextStepCommand() {
        return [this](float dt) { this->tick(dt); };
    }

    std::function<void(ViewportDim, ViewportDim)> getResizeCommand() {
        return [this](ViewportDim w, ViewportDim h) { this->setViewport(w, h); };
    }

   private:
    void onModelChanged(EventType ev);
    void syncFromModel();
    void updateCamera();
    void tick(float dt);

    GameModel* model_;
    EventTrigger vmTrigger;
    uintptr_t funct_callback_Index_;
    InputCommand actionCmd_;

    PlayerInfo player_info_;
    TileInfos tile_infos_;
    EnemyInfos enemy_infos_;
    TileInfos coin_infos_;
    HudInfo hud_info_{};

    // 相机中心初值无关紧要：构造时 syncFromModel→updateCamera 会立即覆盖为跟随玩家的值。
    PositionType cameraX_ = 0.0f;
    PositionType cameraY_ = 0.0f;
    ViewportDim viewW_ = DefaultViewWidth;
    ViewportDim viewH_ = DefaultViewHeight;
};

#endif  // MARIO_VIEWMODEL_H
