#ifndef MARIO_VIEW_H
#define MARIO_VIEW_H

#include "common/Type.h"
#include "view/input/InputHandler.h"
#include "view/renderer/GameRenderer.h"

class GameView {
   public:
    GameView();
    ~GameView() = default;

    void run();
    Notify_Funtion getRenderNotification();

    void setCamera(const PositionType* cx, const PositionType* cy) { renderer_.setCamera(cx, cy); }
    void setPlayerInfo(const PlayerInfo* p) { renderer_.setPlayerInfo(p); }
    void setTileInfos(const TileInfos* t) { renderer_.setTileInfos(t); }
    void setEnemyInfos(const EnemyInfos* e) { renderer_.setEnemyInfos(e); }
    void setHudInfo(const HudInfo* h) { renderer_.setHudInfo(h); }

    void setNextStepCommand(std::function<void(float)>&& cmd) { nextStepCommand_ = std::move(cmd); }
    void setResizeCommand(std::function<void(ViewportDim, ViewportDim)>&& cmd) { resizeCommand_ = std::move(cmd); }
    void pushCurrentViewportSize();

    InputHandler* getInputHandler() { return &input_; }

   private:
    void processWindowEvents();
    void NextStep(float dt);

    sf::RenderWindow window_;
    InputHandler input_;
    GameRenderer renderer_;
    std::function<void(float)> nextStepCommand_;
    std::function<void(ViewportDim, ViewportDim)> resizeCommand_;
    float lastDt_ = 0.0f;
};

#endif  // MARIO_VIEW_H
