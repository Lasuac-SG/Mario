//

#ifndef MARIO_VIEW_H
#define MARIO_VIEW_H

#include "view/AssetManager.h"
#include "view/EntityRenderer.h"
#include "viewmodel/ViewModel.h"
#include "common/ICommand.h"

class GameView {
   public:
    GameView(ViewModel* vm, EntityRenderer* renderer);
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
    void setUpdateFunction(std::function<void(float)> func) {
        updateFrameFunction_ = func;
    }

   private:
    void processEvents();
    void processInput();
    std::function<void(float)> updateFrameFunction_;
    void render();
    sf::RenderWindow window_;
    ViewModel* vm_;
    EntityRenderer* renderer_;
    AssetManager* assets_;
    ICommandBase* actionParam_;

    const PositionType* cameraX_;
    const PositionType* cameraY_;
    const PlayerInfo* playerInfo_;
    const TileInfos * tileInfos_;

    bool keys_[256]{false};
    bool prevJump_ = false;
    bool prevRestart_ = false;

    static constexpr int LOGIC_W = 800;
    static constexpr int LOGIC_H = 600;
};

#endif  // MARIO_VIEW_H