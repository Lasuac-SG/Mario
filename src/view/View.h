//

#ifndef MARIO_VIEW_H
#define MARIO_VIEW_H

#include "view/View.h"
#include "viewmodel/ViewModel.h"
#include "view/EntityRenderer.h"
#include "view/AssetManager.h"


class GameView {
public:
    GameView(GameViewModel* vm, EntityRenderer* renderer);
    ~GameView();

    void run();   // 主循环

private:
    void processEvents();
    void processInput();
    void render();

    sf::RenderWindow window_;
    GameViewModel* vm_;
    EntityRenderer* renderer_;
    AssetManager* assets_;

    bool keys_[256]{false};
    bool prevJump_ = false;
    bool prevRestart_ = false;

    static constexpr int LOGIC_W = 800;
    static constexpr int LOGIC_H = 600;
};

#endif  // MARIO_VIEW_H