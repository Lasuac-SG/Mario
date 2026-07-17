#ifndef MARIO_VIEW_H
#define MARIO_VIEW_H

#include "common/Type.h"
#include "view/audio/AudioManager.h"
#include "view/input/InputHandler.h"
#include "view/renderer/GameRenderer.h"

class GameView {
   public:
    GameView();
    ~GameView() = default;

    void run();
    Notify_Funtion getRenderNotification();
    Notify_Funtion getAudioNotification();

    void setCamera(const PositionType* cx, const PositionType* cy) { renderer_.setCamera(cx, cy); }
    void setPlayerInfo(const PlayerInfo* p) { renderer_.setPlayerInfo(p); }
    void setTileInfos(const TileInfos* t) { renderer_.setTileInfos(t); }
    void setCoinInfos(const TileInfos* c) { renderer_.setCoinInfos(c); }
    void setMushroomInfos(const TileInfos* m) { renderer_.setMushroomInfos(m); }
    void setPlayerBig(const bool* b) { renderer_.setPlayerBig(b); }
    void setEnemyInfos(const EnemyInfos* e) { renderer_.setEnemyInfos(e); }
    void setGoalInfo(const TileInfo* g) { renderer_.setGoalInfo(g); }
    void setWon(const bool* w) { won_ = w; renderer_.setWon(w); }
    void setGameOver(const bool* g) {
        gameOver_ = g;
        renderer_.setGameOver(g);
    }
    void setGameStarted(const bool* gs) {
        gameStarted_ = gs;
        renderer_.setGameStarted(gs);
    }
    void setHudInfo(const HudInfo* h) { renderer_.setHudInfo(h); }

    void setNextStepCommand(std::function<void(float)>&& cmd) { nextStepCommand_ = std::move(cmd); }
    void setResizeCommand(std::function<void(ViewportDim, ViewportDim)>&& cmd) { resizeCommand_ = std::move(cmd); }
    void setStartGameCommand(std::function<void(int)>&& cmd) { startGameCommand_ = std::move(cmd); }
    void setJumpReleaseCommand(std::function<void()>&& cmd) { jumpReleaseCommand_ = std::move(cmd); }
    void setBackToMenuCommand(std::function<void()>&& cmd) { backToMenuCommand_ = std::move(cmd); }
    void pushCurrentViewportSize();

    InputHandler* getInputHandler() { return &input_; }

   private:
    void processWindowEvents();
    void NextStep(float dt);

    sf::RenderWindow window_;
    AudioManager audio_;
    InputHandler input_;
    GameRenderer renderer_;
    const bool* gameOver_ = nullptr;
    const bool* won_ = nullptr;
    const bool* gameStarted_ = nullptr;
    std::function<void(float)> nextStepCommand_;
    std::function<void(ViewportDim, ViewportDim)> resizeCommand_;
    std::function<void(int)> startGameCommand_;
    std::function<void()> jumpReleaseCommand_;
    std::function<void()> backToMenuCommand_;
    float lastDt_ = 0.0f;
};

#endif  // MARIO_VIEW_H
