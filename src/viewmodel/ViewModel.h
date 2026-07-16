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
    const TileInfos* getCoinInfos() const noexcept { return &coin_infos_; }
    const TileInfos* getMushroomInfos() const noexcept { return &mushroom_infos_; }
    const bool* getPlayerBig() const noexcept { return &player_big_; }
    const TileInfo* getGoalInfo() const noexcept { return &goal_info_; }
    const bool* getWon() const noexcept { return &won_; }
    const bool* getGameOver() const noexcept { return &game_over_; }  // 命数耗尽，供 View 播 GameOver/重开按钮
    const bool* getGameStarted() const noexcept { return &started_; }  // false=开始菜单, true=已选图进入游戏
    const HudInfo* getHudInfo() const noexcept { return &hud_info_; }
    PositionType levelWidthPx() const noexcept { return model_->levelWidthPx(); }
    PositionType levelHeightPx() const noexcept { return model_->levelHeightPx(); }

    const PositionType* getCameraX() const noexcept { return &cameraX_; }
    const PositionType* getCameraY() const noexcept { return &cameraY_; }

    ICommandBase* getActionCommand() noexcept { return &actionCmd_; }

    void setViewport(ViewportDim w, ViewportDim h);
    void addNotification(Notify_Funtion func);

    std::function<void(float)> getNextStepCommand() {
        return [this](float dt) { this->tick(dt); };
    }

    std::function<void(ViewportDim, ViewportDim)> getResizeCommand() {
        return [this](ViewportDim w, ViewportDim h) { this->setViewport(w, h); };
    }

    // 开始菜单点击"开始游戏"并选定地图(1/2)后调用：载入该地图并进入游戏。退出游戏为纯 View 行为(关窗)。
    std::function<void(int)> getStartGameCommand() {
        return [this](int mapId) { this->startGame(mapId); };
    }

   private:
    void onModelChanged(EventType ev);
    void syncFromModel();
    void updateCamera() noexcept;
    void tick(float dt);
    void startGame(int mapId);

    GameModel* model_;
    EventTrigger vmTrigger;
    uintptr_t funct_callback_Index_;
    InputCommand actionCmd_;

    PlayerInfo player_info_;
    TileInfos tile_infos_;
    EnemyInfos enemy_infos_;
    TileInfos coin_infos_;
    TileInfos mushroom_infos_;
    bool player_big_ = false;
    TileInfo goal_info_{};
    bool won_ = false;
    bool game_over_ = false;
    bool started_ = false;
    HudInfo hud_info_{};

    PositionType cameraX_ = 0.0f;
    PositionType cameraY_ = 0.0f;
    ViewportDim viewW_ = DefaultViewWidth;
    ViewportDim viewH_ = DefaultViewHeight;
};

#endif  // MARIO_VIEWMODEL_H
