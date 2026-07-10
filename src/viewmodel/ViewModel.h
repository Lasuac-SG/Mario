//

#ifndef MARIO_VIEWMODEL_H
#define MARIO_VIEWMODEL_H

#include "common/EventId.h"
#include "common/EventTrigger.h"
#include "common/Type.h"
#include "model/GameModel.h"
#include "viewmodel/command/Commands.h"

class ViewModel {
   public:
    explicit ViewModel(GameModel* model);
    ~ViewModel();
    ICommandBase& getActionCmd() { return actionCmd_; }
    void tick(float dt);

    const PlayerInfo& getPlayerInfo() const noexcept { return player_info_; }
    const TileInfo& getTileInfo() const noexcept { return tile_info_; }
    PositionType playerX() const { return model_->playerX(); }
    PositionType playerY() const { return model_->playerY(); }
    PositionType playerW() const { return model_->playerW(); }
    PositionType playerH() const { return model_->playerH(); }
    MarioState playerState() const { return model_->playerState(); }
    Direction playerFacing() const { return model_->playerFacing(); }
    void addNotification(Notify_Funtion func);
    int act_Command(InputActionParameter& param) noexcept;

    // 下面的接口应该隐藏
    const GameModel& gameModel() const { return *model_; }  /// 需要隐藏的接口
    EventTrigger vmTrigger;

   private:
    PlayerInfo player_info_;
    TileInfo tile_info_;
    void onModelChanged(EventType ev);

    GameModel* model_;
    uintptr_t funct_calback_Index_;
    InputCommand actionCmd_;
};

#endif  // MARIO_VIEWMODEL_H