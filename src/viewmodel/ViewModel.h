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
    void tick(float dt);

    // === 只读数据绑定（View 拉取，ViewModel 已从 Model 同步并缓存） ===
    const PlayerInfo& playerInfo() const noexcept { return player_info_; }
    const std::vector<TileInfo>& tileInfos() const noexcept { return tile_infos_; }

    // === 命令接口（View 调用 → 写入 Model） ===
    int act_Command(InputActionParameter& param) noexcept;

    // === 通知订阅 ===
    void addNotification(Notify_Funtion func);
    EventTrigger vmTrigger;

    // /// === 旧接口（保留参考，待清理） ===
    // ICommandBase& getActionCmd() { return actionCmd_; }
    // const PlayerInfo& getPlayerInfo() const noexcept { return player_info_; }
    // const TileInfo& getTileInfo() const noexcept { return tile_info_; }
    // PositionType playerX() const { return model_->playerX(); }
    // PositionType playerY() const { return model_->playerY(); }
    // PositionType playerW() const { return model_->playerW(); }
    // PositionType playerH() const { return model_->playerH(); }
    // MarioState playerState() const { return model_->playerState(); }
    // Direction playerFacing() const { return model_->playerFacing(); }
    // const GameModel& gameModel() const { return *model_; }  // 需要隐藏

   private:
    void onModelChanged(EventType ev);
    void syncFromModel();  // 从 Model 拉取数据，填充 player_info_ / tile_infos_

    GameModel* model_;
    uintptr_t funct_callback_Index_;
    InputCommand actionCmd_;

    PlayerInfo player_info_;
    std::vector<TileInfo> tile_infos_;
};

#endif  // MARIO_VIEWMODEL_H