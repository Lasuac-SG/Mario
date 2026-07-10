//

#ifndef MARIO_VIEW_H
#define MARIO_VIEW_H

#include "view/AssetManager.h"
#include "view/EntityRenderer.h"
#include "common/ICommand.h"

class GameView {
   public:
    GameView();
    ~GameView();
    void run();  // 主循环
    void setActionCommand(ICommandBase* cmd){
        actionParam_ = cmd;
    }
    void setCamera(const PositionType* cameraX, const PositionType* cameraY){
        cameraX_ = cameraX;
        cameraY_ = cameraY;
    }
    void setPlayerInfo(const PlayerInfo* playerInfo){
        playerInfo_ = playerInfo;
    }
    void setTileInfo(const TileInfos * tileInfos){
        tileInfos_ = tileInfos;
    }
    /// 时钟更新命令（View → ViewModel::tick → Model::update）
    void setNextStepCommand(std::function<void(float)>&& cmd) {
        nextStepCommand_ = std::move(cmd);
    }

    /// ViewModel 通知后调用，设置重绘标记（设计文档 §5.2 脏标记模式）
    void invalidate() { needsRedraw_ = true; }

   private:
    void processEvents();
    void processInput();
    std::function<void(float)> nextStepCommand_;
    void render();
    sf::RenderWindow window_;
    std::unique_ptr<EntityRenderer> renderer_;
    AssetManager* assets_;
    ICommandBase* actionParam_;

    const PositionType* cameraX_;
    const PositionType* cameraY_;
    const PlayerInfo* playerInfo_;
    const TileInfos * tileInfos_;

    bool keys_[256]{false};
    bool prevJump_ = false;
    bool prevRestart_ = false;
    bool needsRedraw_ = true;  // 脏标记：收到 ViewModel 通知后置 true

    static constexpr int LOGIC_W = 800;
    static constexpr int LOGIC_H = 600;
};

#endif  // MARIO_VIEW_H