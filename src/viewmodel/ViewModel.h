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
    HudInfo hud_info_{};

    PositionType cameraX_ = WorldOrigin;
    PositionType cameraY_ = WorldOrigin;
    ViewportDim viewW_ = DefaultViewWidth;
    ViewportDim viewH_ = DefaultViewHeight;
};

#endif  // MARIO_VIEWMODEL_H
