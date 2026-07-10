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
    explicit ViewModel(GameModel* model, ViewportDim viewW = DefaultViewWidth,
                        ViewportDim viewH = DefaultViewHeight);
    ~ViewModel();
    void tick(float dt);

    // === 只读数据绑定（View 拉取，ViewModel 已从 Model 同步并缓存） ===
    const PlayerInfo& playerInfo() const noexcept { return player_info_; }
    const std::vector<TileInfo>& tileInfos() const noexcept { return tile_infos_; }

    // === 关卡尺寸（透传 Model，供 View 参考） ===
    PositionType levelWidthPx() const { return model_->levelWidthPx(); }
    PositionType levelHeightPx() const { return model_->levelHeightPx(); }

    // === 相机中心（世界坐标，ViewModel 计算，View 直接设到 sf::View） ===
    PositionType cameraX() const { return cameraX_; }
    PositionType cameraY() const { return cameraY_; }

    // === 命令接口（View 调用 → 写入 Model） ===
    int act_Command(InputActionParameter& param) noexcept;
    ICommandBase* getActionCommand(){ return &actionCmd_; }

    // === 通知订阅 ===
    void addNotification(Notify_Funtion func);
    EventTrigger& getEventTrigger(){ return vmTrigger; }
    EventTrigger vmTrigger;

   private:
    void onModelChanged(EventType ev);
    void syncFromModel();  // 从 Model 拉取数据，填充 player_info_ / tile_infos_

    GameModel* model_;
    uintptr_t funct_callback_Index_;
    InputCommand actionCmd_;

    PlayerInfo player_info_;
    std::vector<TileInfo> tile_infos_;

    // 相机状态：syncFromModel() 根据玩家位置和关卡边界更新
    PositionType cameraX_ = WorldOrigin;
    PositionType cameraY_ = WorldOrigin;
    ViewportDim viewW_ = DefaultViewWidth;
    ViewportDim viewH_ = DefaultViewHeight;
};

#endif  // MARIO_VIEWMODEL_H