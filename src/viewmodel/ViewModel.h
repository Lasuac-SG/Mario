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

    // === 只读数据绑定（View 拉取，ViewModel 已从 Model 同步并缓存） ===
    const PlayerInfo* getPlayerInfo() const noexcept { return &player_info_; }
    const TileInfos* getTileInfos() const noexcept { return &tile_infos_; }
    // === 关卡尺寸（透传 Model，供 View 参考） ===
    PositionType levelWidthPx() const { return model_->levelWidthPx(); }
    PositionType levelHeightPx() const { return model_->levelHeightPx(); }

    // === 相机中心（世界坐标，ViewModel 计算，View 直接设到 sf::View） ===
    const PositionType* getCameraX() const { return &cameraX_; }
    const PositionType* getCameraY() const { return &cameraY_; }

    // === 命令接口（View 调用 → 写入 Model） ===
    ICommandBase* getActionCommand(){ return &actionCmd_; }

    // === 通知订阅 ===
    void addNotification(Notify_Funtion func);
    EventTrigger& getEventTrigger(){ return vmTrigger; }
    EventTrigger vmTrigger;

    // 按设计文档模式：返回 std::function 作为 next_step command
    std::function<void(float)> getNextStepCommand() {
        return [this](float dt) { this->tick(dt); };
    }

   private:
    void onModelChanged(EventType ev);
    void syncFromModel();  // 从 Model 拉取数据，填充 player_info_ / tile_infos_
    void tick(float dt);

    GameModel* model_;
    uintptr_t funct_callback_Index_;
    InputCommand actionCmd_;

    PlayerInfo player_info_;
    TileInfos tile_infos_;

    // 相机状态：syncFromModel() 根据玩家位置和关卡边界更新
    PositionType cameraX_ = WorldOrigin;
    PositionType cameraY_ = WorldOrigin;
    ViewportDim viewW_ = DefaultViewWidth;
    ViewportDim viewH_ = DefaultViewHeight;
};

#endif  // MARIO_VIEWMODEL_H